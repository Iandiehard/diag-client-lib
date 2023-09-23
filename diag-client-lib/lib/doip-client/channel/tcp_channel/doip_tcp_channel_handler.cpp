/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/tcp_channel/doip_tcp_channel_handler.h"

#include "channel/tcp_channel.h"
#include "common/logger.h"
#include "core/include/span.h"
#include "handler/tcp_transport_handler.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

std::ostream &operator<<(std::ostream &msg, DiagnosticMessageHandler::DiagAckType diag_ack_type) {
  switch (diag_ack_type.ack_type_) {
    case kDoip_DiagnosticMessage_NegAckCode_InvalidSA:
      msg << "invalid source address.";
      break;
    case kDoip_DiagnosticMessage_NegAckCode_UnknownTA:
      msg << "unknown target address.";
      break;
    case kDoip_DiagnosticMessage_NegAckCode_MessageTooLarge:
      msg << "diagnostic message too large.";
      break;
    case kDoip_DiagnosticMessage_NegAckCode_OutOfMemory:
      msg << "server out of memory.";
      break;
    case kDoip_DiagnosticMessage_NegAckCode_TargetUnreachable:
      msg << "target unreachable.";
      break;
    case kDoip_DiagnosticMessage_NegAckCode_UnknownNetwork:
      msg << "unknown network.";
      break;
    case kDoip_DiagnosticMessage_NegAckCode_TPError:
      msg << "transport protocol error.";
      break;
    default:
      msg << "unknown reason.";
      break;
  }
  return msg;
}

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept -> void {
  DiagnosticMessageChannelState final_state{DiagnosticMessageChannelState::kDiagnosticNegativeAckRecvd};
  if (channel_.GetChannelState().GetDiagnosticMessageStateContext().GetActiveState().GetState() ==
      DiagnosticMessageChannelState::kWaitForDiagnosticAck) {
    // get the ack code
    DiagAckType const diag_ack_type{doip_payload.GetPayload()[0u]};
    if (doip_payload.GetPayloadType() == kDoip_DiagMessagePosAck_Type) {
      if (diag_ack_type.ack_type_ == kDoip_DiagnosticMessage_PosAckCode_Confirm) {
        final_state = DiagnosticMessageChannelState::kDiagnosticPositiveAckRecvd;
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&doip_payload](std::stringstream &msg) {
              msg << "Diagnostic message positively acknowledged from remote server "
                  << " (0x" << std::hex << doip_payload.GetServerAddress() << ")";
            });
      } else {
        // do nothing
      }
    } else if (doip_payload.GetPayloadType() == kDoip_DiagMessageNegAck_Type) {
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
          __FILE__, __LINE__, __func__,
          [&diag_ack_type](std::stringstream &msg) { msg << "Diagnostic request denied due to " << diag_ack_type; });
    } else {
      // do nothing
    }
    channel_.GetChannelState().GetDiagnosticMessageStateContext().TransitionTo(final_state);
    channel_.GetSyncTimer().CancelWait();
  } else {
    // ignore
  }
}

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticMessageResponse(DoipMessage &doip_payload) noexcept -> void {
  if (channel_.GetChannelState().GetDiagnosticMessageStateContext().GetActiveState().GetState() ==
      DiagnosticMessageChannelState::kWaitForDiagnosticResponse) {
    // Indicate upper layer about incoming data
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
        channel_.IndicateMessage(doip_payload.GetServerAddress(), doip_payload.GetClientAddress(),
                                 uds_transport::UdsMessage::TargetAddressType::kPhysical, 0U,
                                 doip_payload.GetPayload().size(), 0u, "DoIPTcp", doip_payload.GetPayload())};
    if (ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending) {
      // keep channel alive since pending request received, do not change channel state
    } else {
      // Check result and udsMessagePtr
      if ((ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
          (ret_val.second != nullptr)) {
        // copy to application buffer
        (void) std::copy(doip_payload.GetPayload().begin(), doip_payload.GetPayload().end(),
                         ret_val.second->GetPayload().begin());
        channel_.HandleMessage(std::move(ret_val.second));
      } else {
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
            __FILE__, __LINE__, __func__,
            [](std::stringstream &msg) { msg << "Diagnostic message response ignored due to unknown error"; });
      }
      channel_.GetChannelState().GetDiagnosticMessageStateContext().TransitionTo(
          DiagnosticMessageChannelState::kDiagIdle);
    }
  } else {
    // ignore
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          msg << "Diagnostic message response ignored due to channel in state: "
              << static_cast<int>(
                     channel_.GetChannelState().GetDiagnosticMessageStateContext().GetActiveState().GetState());
        });
  }
}

auto DiagnosticMessageHandler::SendDiagnosticRequest(uds_transport::UdsMessageConstPtr &message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  TcpMessagePtr doip_diag_req = std::make_unique<TcpMessage>();
  // reserve bytes in vector
  doip_diag_req->GetTxBuffer().reserve(kDoipheadrSize + kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
  // create header
  CreateDoipGenericHeader(doip_diag_req->GetTxBuffer(), kDoip_DiagMessage_Type,
                          kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
  // Add source address
  doip_diag_req->GetTxBuffer().push_back((uint8_t) ((message->GetSa() & 0xFF00) >> 8));
  doip_diag_req->GetTxBuffer().push_back((uint8_t) (message->GetSa() & 0x00FF));
  // Add target address
  doip_diag_req->GetTxBuffer().push_back((uint8_t) ((message->GetTa() & 0xFF00) >> 8));
  doip_diag_req->GetTxBuffer().push_back((uint8_t) (message->GetTa() & 0x00FF));
  // Add data bytes
  for (std::uint8_t byte: message->GetPayload()) { doip_diag_req->GetTxBuffer().push_back(byte); }
  // transmit
  if (!(tcp_socket_handler_.Transmit(std::move(doip_diag_req)))) {
    // do nothing
  } else {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

auto DiagnosticMessageHandler::CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType,
                                                       uint32_t payloadLen) noexcept -> void {
  doipHeader.push_back(kDoip_ProtocolVersion);
  doipHeader.push_back(~((uint8_t) kDoip_ProtocolVersion));
  doipHeader.push_back((uint8_t) ((payloadType & 0xFF00) >> 8));
  doipHeader.push_back((uint8_t) (payloadType & 0x00FF));
  doipHeader.push_back((uint8_t) ((payloadLen & 0xFF000000) >> 24));
  doipHeader.push_back((uint8_t) ((payloadLen & 0x00FF0000) >> 16));
  doipHeader.push_back((uint8_t) ((payloadLen & 0x0000FF00) >> 8));
  doipHeader.push_back((uint8_t) (payloadLen & 0x000000FF));
}

auto DoipTcpChannelHandler::SendRoutingActivationRequest(
    uds_transport::UdsMessageConstPtr routing_activation_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return routing_activation_handler_.HandleRoutingActivationRequest(routing_activation_request);
}

auto DoipTcpChannelHandler::SendDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return (diagnostic_message_handler_.SendDiagnosticRequest(diagnostic_request));
}

auto DoipTcpChannelHandler::HandleMessage(TcpMessagePtr tcp_rx_message) noexcept -> void {
  std::uint8_t nack_code{};
  DoipMessage doip_rx_message{DoipMessage::MessageType::kTcp, tcp_rx_message->GetHostIpAddress(),
                              tcp_rx_message->GetHostPortNumber(),
                              core_type::Span<std::uint8_t>{tcp_rx_message->GetRxBuffer()}};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto DoipTcpChannelHandler::ProcessDoIPHeader(DoipMessage &doip_rx_message, std::uint8_t &nackCode) noexcept -> bool {
  bool ret_val = false;
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion) &&
       (doip_rx_message.GetInverseProtocolVersion() == (std::uint8_t)(~(kDoip_ProtocolVersion)))) ||
      ((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion_Def) &&
       (doip_rx_message.GetInverseProtocolVersion() == (std::uint8_t)(~(kDoip_ProtocolVersion_Def))))) {
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
            nackCode = kDoip_GenericHeader_InvalidPayloadLen;
            // socket closure ??
          }
        } else {
          // Send NACK code 0x03, discard message
          nackCode = kDoip_GenericHeader_OutOfMemory;
        }
      } else {
        // Send NACK code 0x02, discard message
        nackCode = kDoip_GenericHeader_MessageTooLarge;
      }
    } else {  // Send NACK code 0x01, discard message
      nackCode = kDoip_GenericHeader_UnknownPayload;
    }
  } else {  // Send NACK code 0x00, close the socket
    nackCode = kDoip_GenericHeader_IncorrectPattern;
    // socket closure
  }
  return ret_val;
}

auto DoipTcpChannelHandler::ProcessDoIPPayloadLength(std::uint32_t payload_length, std::uint16_t payload_type) noexcept
    -> bool {
  bool ret_val{false};
  switch (payload_type) {
    case kDoip_RoutingActivation_ResType: {
      if (payload_length <= kDoip_RoutingActivation_ResMaxLen) ret_val = true;
      break;
    }
    case kDoip_DiagMessagePosAck_Type:
    case kDoip_DiagMessageNegAck_Type: {
      if (payload_length >= kDoip_DiagMessageAck_ResMinLen) ret_val = true;
      break;
    }
    case kDoip_DiagMessage_Type: {
      // Req - [20-11][AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00122]
      if (payload_length >= (kDoip_DiagMessage_ReqResMinLen + 1u)) ret_val = true;
      break;
    }
    case kDoip_AliveCheck_ReqType: {
      if (payload_length <= kDoip_RoutingActivation_ResMaxLen) ret_val = true;
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
