/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/udp_channel/doip_udp_channel_handler.h"

#include <utility>

#include "common/common_doip_types.h"
#include "common/logger.h"
#include "core/include/span.h"

namespace doip_client {
namespace channel {
namespace udp_channel {

/**
 * @brief  Generic DoIP Header NACK codes
 */
constexpr std::uint8_t kDoip_GenericHeader_IncorrectPattern{0x00};
constexpr std::uint8_t kDoip_GenericHeader_UnknownPayload{0x01};
constexpr std::uint8_t kDoip_GenericHeader_MessageTooLarge{0x02};
constexpr std::uint8_t kDoip_GenericHeader_OutOfMemory{0x03};
constexpr std::uint8_t kDoip_GenericHeader_InvalidPayloadLen{0x04};

DoipUdpChannelHandler::DoipUdpChannelHandler(sockets::UdpSocketHandler &udp_socket_handler_broadcast,
                                             sockets::UdpSocketHandler &udp_socket_handler_unicast,
                                             DoipUdpChannel &channel)
    : vehicle_discovery_handler_{udp_socket_handler_broadcast, channel},
      vehicle_identification_handler_{udp_socket_handler_unicast, channel} {}

auto DoipUdpChannelHandler::SendVehicleIdentificationRequest(
    uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  // Get the udp handler type from payload
  std::uint8_t const handler_type{vehicle_identification_request->GetPayload()[BYTE_POS_ZERO]};
  // deserialize and send to proper handler
  switch (handler_type) {
    case 0U:
      // 0U -> Vehicle Identification Req
      ret_val =
          vehicle_identification_handler_.HandleVehicleIdentificationRequest(std::move(vehicle_identification_request));
      break;
    case 1U:
      // 1U -> Power Mode Req
      break;
  }
  return ret_val;
}

auto DoipUdpChannelHandler::HandleMessageUnicast(UdpMessagePtr udp_rx_message) noexcept -> void {
  std::uint8_t nack_code{};
  DoipMessage doip_rx_message{DoipMessage::MessageType::kUdp, udp_rx_message->GetHostIpAddress(),
                              udp_rx_message->GetHostPortNumber(),
                              core_type::Span<std::uint8_t>{udp_rx_message->GetRxBuffer()}};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto DoipUdpChannelHandler::HandleMessageBroadcast(UdpMessagePtr udp_rx_message) noexcept -> void {
  uint8_t nack_code;
  DoipMessage doip_rx_message{DoipMessage::MessageType::kUdp, udp_rx_message->GetHostIpAddress(),
                              udp_rx_message->GetHostPortNumber(),
                              core_type::Span<std::uint8_t>{udp_rx_message->GetRxBuffer()}};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    vehicle_discovery_handler_.ProcessVehicleAnnouncementResponse(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto DoipUdpChannelHandler::ProcessDoIPHeader(DoipMessage &doip_rx_message, std::uint8_t &nack_code) noexcept -> bool {
  bool ret_val{false};
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion) &&
       (doip_rx_message.GetInverseProtocolVersion() == static_cast<std::uint8_t>(~kDoip_ProtocolVersion))) ||
      ((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion_Def) &&
       (doip_rx_message.GetInverseProtocolVersion() == static_cast<std::uint8_t>(~kDoip_ProtocolVersion_Def)))) {
    /* Check the supported payload type */
    if (doip_rx_message.GetPayloadType() == kDoip_VehicleAnnouncement_ResType) {
      /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
      if (doip_rx_message.GetPayloadLength() <= kDoip_Protocol_MaxPayload) {
        /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
        if (doip_rx_message.GetPayloadLength() <= kUdpChannelLength) {
          /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00019] */
          if (ProcessDoIPPayloadLength(doip_rx_message.GetPayloadLength(), doip_rx_message.GetPayloadType())) {
            ret_val = true;
          } else {
            // Send NACK code 0x04, close the socket
            nack_code = kDoip_GenericHeader_InvalidPayloadLen;
            // socket closure ??
          }
        } else {
          // Send NACK code 0x03, discard message
          nack_code = kDoip_GenericHeader_OutOfMemory;
        }
      } else {
        // Send NACK code 0x02, discard message
        nack_code = kDoip_GenericHeader_MessageTooLarge;
      }
    } else {  // Send NACK code 0x01, discard message
      nack_code = kDoip_GenericHeader_UnknownPayload;
    }
  } else {  // Send NACK code 0x00, close the socket
    nack_code = kDoip_GenericHeader_IncorrectPattern;
    // socket closure
  }
  return ret_val;
}

auto DoipUdpChannelHandler::ProcessDoIPPayloadLength(std::uint32_t payload_len, std::uint16_t payload_type) noexcept
    -> bool {
  bool ret_val{false};
  switch (payload_type) {
    case kDoip_VehicleAnnouncement_ResType: {
      if (payload_len <= kDoip_VehicleAnnouncement_ResMaxLen) ret_val = true;
      break;
    }
    default:
      // do nothing
      break;
  }
  return ret_val;
}

void DoipUdpChannelHandler::ProcessDoIPPayload(DoipMessage &doip_payload, DoipMessage::RxSocketType) {
  std::lock_guard<std::mutex> const lck(channel_handler_lock);
  switch (doip_payload.GetPayloadType()) {
    case kDoip_VehicleAnnouncement_ResType: {
      vehicle_identification_handler_.ProcessVehicleIdentificationResponse(doip_payload);
      break;
    }
    default:
      /* do nothing */
      break;
  }
}

}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client
