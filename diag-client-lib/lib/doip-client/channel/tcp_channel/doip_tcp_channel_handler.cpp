/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/tcp_channel/doip_tcp_channel_handler.h"

#include <utility>

#include "channel/tcp_channel/doip_tcp_channel.h"
#include "common/common_doip_types.h"
#include "common/logger.h"
#include "core/include/span.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace {

/**
 * @brief  Routing Activation response Type
 */
constexpr std::uint16_t kDoip_RoutingActivation_ResType{0x0006};

/**
 * @brief  Diagnostic message type
 */
constexpr std::uint16_t kDoip_DiagMessage_Type{0x8001};
constexpr std::uint16_t kDoip_DiagMessagePosAck_Type{0x8002};
constexpr std::uint16_t kDoip_DiagMessageNegAck_Type{0x8003};

/**
 * @brief  Alive check type
 */
constexpr std::uint16_t kDoip_AliveCheck_ReqType{0x0007};
constexpr std::uint16_t kDoip_AliveCheck_ResType{0x0008};

/**
 * @brief  Generic DoIP Header NACK codes
 */
constexpr std::uint8_t kDoip_GenericHeader_IncorrectPattern{0x00};
constexpr std::uint8_t kDoip_GenericHeader_UnknownPayload{0x01};
constexpr std::uint8_t kDoip_GenericHeader_MessageTooLarge{0x02};
constexpr std::uint8_t kDoip_GenericHeader_OutOfMemory{0x03};
constexpr std::uint8_t kDoip_GenericHeader_InvalidPayloadLen{0x04};

/**
 * @brief  Diagnostic Message request/response lengths
 */
constexpr std::uint8_t kDoip_DiagMessage_ReqResMinLen = 4U;  // considering SA and TA
constexpr std::uint8_t kDoip_DiagMessageAck_ResMinLen = 5U;  // considering SA, TA, Ack code

/**
 * @brief  Routing Activation request lengths
 */
// constexpr std::uint32_t kDoip_RoutingActivation_ReqMinLen{7u};   //without OEM specific use byte
// constexpr std::uint32_t kDoip_RoutingActivation_ReqMaxLen{11u};  //with OEM specific use byte
constexpr std::uint32_t kDoipRoutingActivationResMinLen{9u};   // without OEM specific use byte
constexpr std::uint32_t kDoipRoutingActivationResMaxLen{13u};  //with OEM specific use byte

}  // namespace

DoipTcpChannelHandler::DoipTcpChannelHandler(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel)
    : routing_activation_handler_{tcp_socket_handler},
      diagnostic_message_handler_{tcp_socket_handler, channel} {}

void DoipTcpChannelHandler::Start() {
  routing_activation_handler_.Start();
  diagnostic_message_handler_.Start();
}

void DoipTcpChannelHandler::Stop() {
  routing_activation_handler_.Stop();
  diagnostic_message_handler_.Stop();
}

void DoipTcpChannelHandler::Reset() {
  routing_activation_handler_.Reset();
  diagnostic_message_handler_.Reset();
}

auto DoipTcpChannelHandler::SendRoutingActivationRequest(
    uds_transport::UdsMessageConstPtr routing_activation_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::ConnectionResult {
  return routing_activation_handler_.HandleRoutingActivationRequest(std::move(routing_activation_request));
}

auto DoipTcpChannelHandler::SendDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return diagnostic_message_handler_.HandleDiagnosticRequest(std::move(diagnostic_request));
}

auto DoipTcpChannelHandler::HandleMessage(TcpMessagePtr tcp_rx_message) noexcept -> void {
  std::uint8_t nack_code{};
  DoipMessage doip_rx_message{DoipMessage::MessageType::kTcp, tcp_rx_message->GetHostIpAddress(),
                              tcp_rx_message->GetHostPortNumber(), tcp_rx_message->GetPayload()};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto DoipTcpChannelHandler::IsRoutingActivated() noexcept -> bool {
  return routing_activation_handler_.IsRoutingActivated();
}

auto DoipTcpChannelHandler::ProcessDoIPHeader(DoipMessage &doip_rx_message, std::uint8_t &nack_code) noexcept -> bool {
  bool ret_val = false;
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion) &&
       (doip_rx_message.GetInverseProtocolVersion() == static_cast<std::uint8_t>(~kDoip_ProtocolVersion))) ||
      ((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion_Def) &&
       (doip_rx_message.GetInverseProtocolVersion() == static_cast<std::uint8_t>(~kDoip_ProtocolVersion_Def)))) {
    /* Check the supported payload type */
    if ((doip_rx_message.GetPayloadType() == kDoip_RoutingActivation_ResType) ||
        (doip_rx_message.GetPayloadType() == kDoip_DiagMessagePosAck_Type) ||
        (doip_rx_message.GetPayloadType() == kDoip_DiagMessageNegAck_Type) ||
        (doip_rx_message.GetPayloadType() == kDoip_DiagMessage_Type) ||
        (doip_rx_message.GetPayloadType() == kDoip_AliveCheck_ReqType)) {
      /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
      if (doip_rx_message.GetPayloadLength() <= kDoip_Protocol_MaxPayload) {
        /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
        if (doip_rx_message.GetPayloadLength() <= kTcpChannelLength) {
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

auto DoipTcpChannelHandler::ProcessDoIPPayloadLength(std::uint32_t payload_length, std::uint16_t payload_type) noexcept
    -> bool {
  bool ret_val{false};
  switch (payload_type) {
    case kDoip_RoutingActivation_ResType: {
      if (payload_length <= kDoipRoutingActivationResMaxLen) ret_val = true;
      break;
    }
    case kDoip_DiagMessagePosAck_Type:
    case kDoip_DiagMessageNegAck_Type: {
      if (payload_length >= kDoip_DiagMessageAck_ResMinLen) ret_val = true;
      break;
    }
    case kDoip_DiagMessage_Type: {
      // Atleast 1 byte of data must be present
      if (payload_length >= (kDoip_DiagMessage_ReqResMinLen + 1u)) ret_val = true;
      break;
    }
    case kDoip_AliveCheck_ReqType: {
      if (payload_length <= kDoipRoutingActivationResMaxLen) ret_val = true;
      break;
    }
    default:
      // do nothing
      break;
  }
  return ret_val;
}

void DoipTcpChannelHandler::ProcessDoIPPayload(DoipMessage &doip_payload) noexcept {
  std::lock_guard<std::mutex> const lck(channel_handler_lock);
  switch (doip_payload.GetPayloadType()) {
    case kDoip_RoutingActivation_ResType:
      // Process RoutingActivation response
      routing_activation_handler_.ProcessDoIPRoutingActivationResponse(doip_payload);
      break;
    case kDoip_DiagMessage_Type:
      // Process Diagnostic Message Response
      diagnostic_message_handler_.ProcessDoIPDiagnosticMessageResponse(doip_payload);
      break;
    case kDoip_DiagMessagePosAck_Type:
    case kDoip_DiagMessageNegAck_Type:
      // Process positive or negative diag ack message
      diagnostic_message_handler_.ProcessDoIPDiagnosticAckMessageResponse(doip_payload);
      break;
    default:
      /* do nothing */
      break;
  }
}
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
