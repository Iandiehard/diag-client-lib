/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/channel/udp_channel/channel_handler/udp_channel_handler.h"

#include <utility>

#include "core/include/span.h"
#include "doip-client/logger/logger.h"
#include "doip-client/message/doip_types.h"

namespace doip_client {
namespace channel {
namespace udp_channel {
namespace channel_handler {
namespace {
/**
 * @brief  Vehicle Identification type
 */
constexpr std::uint16_t kDoipVehicleAnnouncementRes{0x0004};

/**
 * @brief  Payload length excluding header
 */
constexpr std::uint32_t kDoipVehicleAnnouncementResMaxLen{33u};

/**
 * @brief  Generic DoIP Header NACK codes
 */
constexpr std::uint8_t kDoip_GenericHeader_IncorrectPattern{0x00};
constexpr std::uint8_t kDoip_GenericHeader_UnknownPayload{0x01};
constexpr std::uint8_t kDoip_GenericHeader_MessageTooLarge{0x02};
constexpr std::uint8_t kDoip_GenericHeader_OutOfMemory{0x03};
constexpr std::uint8_t kDoip_GenericHeader_InvalidPayloadLen{0x04};

}  // namespace

UdpChannelHandler::UdpChannelHandler(sockets::UdpSocketHandler &udp_socket_handler_broadcast,
                                     sockets::UdpSocketHandler &udp_socket_handler_unicast,
                                     DoipUdpChannel &channel)
    : vehicle_discovery_handler_{udp_socket_handler_broadcast, channel},
      vehicle_identification_handler_{udp_socket_handler_unicast, channel} {}

auto UdpChannelHandler::SendVehicleIdentificationRequest(
    uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  // Get the udp handler type from payload
  std::uint8_t const handler_type{vehicle_identification_request->GetPayload()[0u]};
  // deserialize and send to proper handler
  switch (handler_type) {
    case 0U:
      // 0U -> Vehicle Identification Req
      ret_val = vehicle_identification_handler_.HandleVehicleIdentificationRequest(
          std::move(vehicle_identification_request));
      break;
    case 1U:
      // 1U -> Power Mode Req
      break;
  }
  return ret_val;
}

auto UdpChannelHandler::HandleMessageUnicast(UdpMessagePtr udp_rx_message) noexcept -> void {
  std::uint8_t nack_code{};
  DoipMessage doip_rx_message{DoipMessage::MessageType::kUdp, udp_rx_message->GetHostIpAddress(),
                              udp_rx_message->GetHostPortNumber(), udp_rx_message->GetPayload()};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto UdpChannelHandler::HandleMessageBroadcast(UdpMessagePtr udp_rx_message) noexcept -> void {
  uint8_t nack_code;
  DoipMessage doip_rx_message{DoipMessage::MessageType::kUdp, udp_rx_message->GetHostIpAddress(),
                              udp_rx_message->GetHostPortNumber(), udp_rx_message->GetPayload()};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    vehicle_discovery_handler_.ProcessVehicleAnnouncementResponse(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto UdpChannelHandler::ProcessDoIPHeader(DoipMessage &doip_rx_message,
                                          std::uint8_t &nack_code) noexcept -> bool {
  bool ret_val{false};
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.GetProtocolVersion() == message::kDoipProtocolVersion) &&
       (doip_rx_message.GetInverseProtocolVersion() ==
        static_cast<std::uint8_t>(~message::kDoipProtocolVersion))) ||
      ((doip_rx_message.GetProtocolVersion() == message::kDoipProtocolVersionDefault) &&
       (doip_rx_message.GetInverseProtocolVersion() ==
        static_cast<std::uint8_t>(~message::kDoipProtocolVersionDefault)))) {
    /* Check the supported payload type */
    if (doip_rx_message.GetPayloadType() == kDoipVehicleAnnouncementRes) {
      /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
      if (doip_rx_message.GetPayloadLength() <= message::kDoipProtocolMaxPayload) {
        /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
        if (doip_rx_message.GetPayloadLength() <= message::kUdpChannelLength) {
          /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00019] */
          if (ProcessDoIPPayloadLength(doip_rx_message.GetPayloadLength(),
                                       doip_rx_message.GetPayloadType())) {
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

auto UdpChannelHandler::ProcessDoIPPayloadLength(std::uint32_t payload_len,
                                                 std::uint16_t payload_type) noexcept -> bool {
  bool ret_val{false};
  switch (payload_type) {
    case kDoipVehicleAnnouncementRes: {
      if (payload_len <= kDoipVehicleAnnouncementResMaxLen) { ret_val = true; }
    } break;

    default:
      // do nothing
      break;
  }
  return ret_val;
}

void UdpChannelHandler::ProcessDoIPPayload(DoipMessage &doip_payload, DoipMessage::RxSocketType) {
  std::lock_guard<std::mutex> const lck(channel_handler_lock);
  switch (doip_payload.GetPayloadType()) {
    case kDoipVehicleAnnouncementRes: {
      vehicle_identification_handler_.ProcessVehicleIdentificationResponse(doip_payload);
      break;
    }
    default:
      /* do nothing */
      break;
  }
}
}  // namespace channel_handler
}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client
