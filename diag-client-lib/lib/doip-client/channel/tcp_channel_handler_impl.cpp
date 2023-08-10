/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/tcp_channel_handler_impl.h"

#include "channel/tcp_channel.h"
#include "common/logger.h"
#include "handler/tcp_transport_handler.h"

namespace doip_client {
namespace tcpChannelHandlerImpl {

std::ostream &operator<<(std::ostream &msg, RoutingActivationHandler::RoutingActivationAckType act_type) {
  switch (act_type.act_type_) {
    case kDoip_RoutingActivation_ResCode_UnknownSA:
      msg << "unknown source address.";
      break;
    case kDoip_RoutingActivation_ResCode_AllSocktActive:
      msg << "all Socket active.";
      break;
    case kDoip_RoutingActivation_ResCode_DifferentSA:
      msg << "SA different on already connected socket.";
      break;
    case kDoip_RoutingActivation_ResCode_ActiveSA:
      msg << "SA active on different socket.";
      break;
    case kDoip_RoutingActivation_ResCode_AuthentnMissng:
      msg << "missing authentication.";
      break;
    case kDoip_RoutingActivation_ResCode_ConfirmtnRejectd:
      msg << "rejected confirmation.";
      break;
    case kDoip_RoutingActivation_ResCode_UnsupportdActType:
      msg << "unsupported routing activation type.";
      break;
    case kDoip_RoutingActivation_ResCode_TLSRequired:
      msg << "required TLS socket.";
      break;
    default:
      msg << "unknown reason.";
      break;
  }
  msg << " (0x" << std::hex << static_cast<int>(act_type.act_type_) << ")";
  return msg;
}

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

auto RoutingActivationHandler::ProcessDoIPRoutingActivationResponse(DoipMessage &doip_payload) noexcept -> void {
  RoutingActivationChannelState final_state{RoutingActivationChannelState::kRoutingActivationFailed};
  if (channel_.GetChannelState().GetRoutingActivationStateContext().GetActiveState().GetState() ==
      RoutingActivationChannelState::kWaitForRoutingActivationRes) {
    // get the logical address of client
    uint16_t client_address = (uint16_t) ((doip_payload.payload[BYTE_POS_ZERO] << 8) & 0xFF00) |
                              (uint16_t) (doip_payload.payload[BYTE_POS_ONE] & 0x00FF);
    // get the logical address of Server
    uint16_t server_address = (uint16_t) ((doip_payload.payload[BYTE_POS_TWO] << 8) & 0xFF00) |
                              (uint16_t) (doip_payload.payload[BYTE_POS_THREE] & 0x00FF);
    // get the ack code
    RoutingActivationAckType const rout_act_type{doip_payload.payload[BYTE_POS_FOUR]};
    switch (rout_act_type.act_type_) {
      case kDoip_RoutingActivation_ResCode_RoutingSuccessful: {
        // routing successful
        final_state = RoutingActivationChannelState::kRoutingActivationSuccessful;
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&server_address](std::stringstream &msg) {
              msg << "RoutingActivation successfully activated in remote server with logical Address"
                  << " (0x" << std::hex << server_address << ")";
            });
      } break;
      case kDoip_RoutingActivation_ResCode_ConfirmtnRequired: {
        // trigger routing activation after sometime, not implemented yet
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&server_address](std::stringstream &msg) {
              msg << "RoutingActivation is activated, confirmation required in remote server with logical Address"
                  << " (0x" << std::hex << server_address << ")";
            });
      } break;
      default:
        // failure, do nothing
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
            __FILE__, __LINE__, __func__,
            [&rout_act_type](std::stringstream &msg) { msg << "Routing activation denied due to " << rout_act_type; });
        break;
    }
    channel_.GetChannelState().GetRoutingActivationStateContext().TransitionTo(final_state);
    channel_.GetSyncTimer().CancelWait();
  } else {
    /* ignore */
  }
}

auto RoutingActivationHandler::SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr &message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  TcpMessagePtr doip_routing_act_req{std::make_unique<TcpMessage>()};
  // reserve bytes in vector
  doip_routing_act_req->txBuffer_.reserve(kDoipheadrSize + kDoip_RoutingActivation_ReqMinLen);
  // create header
  CreateDoipGenericHeader(doip_routing_act_req->txBuffer_, kDoip_RoutingActivation_ReqType,
                          kDoip_RoutingActivation_ReqMinLen);
  // Add source address
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) ((message->GetSa() & 0xFF00) >> 8));
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) (message->GetSa() & 0x00FF));
  // Add activation type
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) kDoip_RoutingActivation_ReqActType_Default);
  // Add reservation byte , default zeroes
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) 0x00);
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) 0x00);
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) 0x00);
  doip_routing_act_req->txBuffer_.emplace_back((uint8_t) 0x00);
  // transmit
  if (tcp_socket_handler_.Transmit(std::move(doip_routing_act_req))) {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

void RoutingActivationHandler::CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType,
                                                       uint32_t payloadLen) {
  doipHeader.push_back(kDoip_ProtocolVersion);
  doipHeader.push_back(~((uint8_t) kDoip_ProtocolVersion));
  doipHeader.push_back((uint8_t) ((payloadType & 0xFF00) >> 8));
  doipHeader.push_back((uint8_t) (payloadType & 0x00FF));
  doipHeader.push_back((uint8_t) ((payloadLen & 0xFF000000) >> 24));
  doipHeader.push_back((uint8_t) ((payloadLen & 0x00FF0000) >> 16));
  doipHeader.push_back((uint8_t) ((payloadLen & 0x0000FF00) >> 8));
  doipHeader.push_back((uint8_t) (payloadLen & 0x000000FF));
}

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept -> void {
  DiagnosticMessageChannelState final_state{DiagnosticMessageChannelState::kDiagnosticNegativeAckRecvd};
  if (channel_.GetChannelState().GetDiagnosticMessageStateContext().GetActiveState().GetState() ==
      DiagnosticMessageChannelState::kWaitForDiagnosticAck) {
    // check the logical address of Server
    std::uint16_t server_address =
        (uint16_t) (((doip_payload.payload[BYTE_POS_ZERO] & 0xFF) << 8) | (doip_payload.payload[BYTE_POS_ONE] & 0xFF));
    // check the logical address of client
    std::uint16_t client_address =
        (uint16_t) (((doip_payload.payload[BYTE_POS_TWO] & 0xFF) << 8) | (doip_payload.payload[BYTE_POS_THREE] & 0xFF));

    // get the ack code
    DiagAckType const diag_ack_type{doip_payload.payload[BYTE_POS_FOUR]};
    if (doip_payload.payload_type == kDoip_DiagMessagePosAck_Type) {
      if (diag_ack_type.ack_type_ == kDoip_DiagnosticMessage_PosAckCode_Confirm) {
        final_state = DiagnosticMessageChannelState::kDiagnosticPositiveAckRecvd;
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&server_address](std::stringstream &msg) {
              msg << "Diagnostic message positively acknowledged from remote server "
                  << " (0x" << std::hex << server_address << ")";
            });
      } else {
        // do nothing
      }
    } else if (doip_payload.payload_type == kDoip_DiagMessageNegAck_Type) {
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
    // check the logical address of Server
    uds_transport::UdsMessage::Address server_address{static_cast<uds_transport::UdsMessage::Address>(
        ((doip_payload.payload[BYTE_POS_ZERO] & 0xFF) << 8) | (doip_payload.payload[BYTE_POS_ONE] & 0xFF))};
    // check the logical address of client
    uds_transport::UdsMessage::Address client_address{static_cast<uds_transport::UdsMessage::Address>(
        ((doip_payload.payload[BYTE_POS_TWO] & 0xFF) << 8) | (doip_payload.payload[BYTE_POS_THREE] & 0xFF))};

    // payload except the server nd client address
    core_type::Span<std::uint8_t> payload_info{&doip_payload.payload[4u], doip_payload.payload.size() - 4u};

    // Indicate upper layer about incoming data
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
        tcp_transport_handler_.IndicateMessage(server_address, client_address,
                                               uds_transport::UdsMessage::TargetAddressType::kPhysical, 0U,
                                               payload_info.size(), 0U, "DoIPTcp", payload_info)};
    if (ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending) {
      // keep channel alive since pending request received, do not change channel state
    } else {
      // Check result and udsMessagePtr
      if ((ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
          (ret_val.second != nullptr)) {
        // copy to application buffer
        (void) std::copy(payload_info.begin(), payload_info.end(), ret_val.second->GetPayload().begin());
        tcp_transport_handler_.HandleMessage(std::move(ret_val.second));
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
  doip_diag_req->txBuffer_.reserve(kDoipheadrSize + kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
  // create header
  CreateDoipGenericHeader(doip_diag_req->txBuffer_, kDoip_DiagMessage_Type,
                          kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
  // Add source address
  doip_diag_req->txBuffer_.push_back((uint8_t) ((message->GetSa() & 0xFF00) >> 8));
  doip_diag_req->txBuffer_.push_back((uint8_t) (message->GetSa() & 0x00FF));
  // Add target address
  doip_diag_req->txBuffer_.push_back((uint8_t) ((message->GetTa() & 0xFF00) >> 8));
  doip_diag_req->txBuffer_.push_back((uint8_t) (message->GetTa() & 0x00FF));
  // Add data bytes
  for (std::uint8_t byte: message->GetPayload()) { doip_diag_req->txBuffer_.push_back(byte); }
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

auto TcpChannelHandlerImpl::SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr &message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return (routing_activation_handler_.SendRoutingActivationRequest(message));
}

auto TcpChannelHandlerImpl::SendDiagnosticRequest(uds_transport::UdsMessageConstPtr &message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return (diagnostic_message_handler_.SendDiagnosticRequest(message));
}

auto TcpChannelHandlerImpl::HandleMessage(TcpMessagePtr tcp_rx_message) noexcept -> void {
  std::uint8_t nack_code{};
  DoipMessage doip_rx_message{};
  // Todo : move below assignments directly to construction and make doipMessage class mutable
  doip_rx_message.protocol_version = tcp_rx_message->rxBuffer_[0u];
  doip_rx_message.protocol_version_inv = tcp_rx_message->rxBuffer_[1u];
  doip_rx_message.payload_type = GetDoIPPayloadType(tcp_rx_message->rxBuffer_);
  doip_rx_message.payload_length = GetDoIPPayloadLength(tcp_rx_message->rxBuffer_);
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    doip_rx_message.payload = core_type::Span<std::uint8_t>{&tcp_rx_message->rxBuffer_[kDoipheadrSize],
                                                            tcp_rx_message->rxBuffer_.size() - kDoipheadrSize};
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto TcpChannelHandlerImpl::ProcessDoIPHeader(DoipMessage &doip_rx_message, uint8_t &nackCode) noexcept -> bool {
  bool ret_val = false;
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.protocol_version == kDoip_ProtocolVersion) &&
       (doip_rx_message.protocol_version_inv == (uint8_t) (~(kDoip_ProtocolVersion)))) ||
      ((doip_rx_message.protocol_version == kDoip_ProtocolVersion_Def) &&
       (doip_rx_message.protocol_version_inv == (uint8_t) (~(kDoip_ProtocolVersion_Def))))) {
    /* Check the supported payload type */
    if ((doip_rx_message.payload_type == kDoip_RoutingActivation_ResType) ||
        (doip_rx_message.payload_type == kDoip_DiagMessagePosAck_Type) ||
        (doip_rx_message.payload_type == kDoip_DiagMessageNegAck_Type) ||
        (doip_rx_message.payload_type == kDoip_DiagMessage_Type) ||
        (doip_rx_message.payload_type == kDoip_AliveCheck_ReqType)) {
      /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
      if (doip_rx_message.payload_length <= kDoip_Protocol_MaxPayload) {
        /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
        if (doip_rx_message.payload_length <= kTcpChannelLength) {
          /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00019] */
          if (ProcessDoIPPayloadLength(doip_rx_message.payload_length, doip_rx_message.payload_type)) {
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

auto TcpChannelHandlerImpl::ProcessDoIPPayloadLength(uint32_t payloadLen, uint16_t payloadType) noexcept -> bool {
  bool ret_val{false};
  switch (payloadType) {
    case kDoip_RoutingActivation_ResType: {
      if (payloadLen <= (uint32_t) kDoip_RoutingActivation_ResMaxLen) ret_val = true;
      break;
    }
    case kDoip_DiagMessagePosAck_Type:
    case kDoip_DiagMessageNegAck_Type: {
      if (payloadLen >= (uint32_t) kDoip_DiagMessageAck_ResMinLen) ret_val = true;
      break;
    }
    case kDoip_DiagMessage_Type: {
      // Req - [20-11][AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00122]
      if (payloadLen >= (uint32_t) (kDoip_DiagMessage_ReqResMinLen + 1u)) ret_val = true;
      break;
    }
    case kDoip_AliveCheck_ReqType: {
      if (payloadLen <= (uint32_t) kDoip_RoutingActivation_ResMaxLen) ret_val = true;
      break;
    }
    default:
      // do nothing
      break;
  }
  return ret_val;
}

auto TcpChannelHandlerImpl::GetDoIPPayloadType(std::vector<uint8_t> payload) noexcept -> uint16_t {
  return ((uint16_t) (((payload[BYTE_POS_TWO] & 0xFF) << 8) | (payload[BYTE_POS_THREE] & 0xFF)));
}

auto TcpChannelHandlerImpl::GetDoIPPayloadLength(std::vector<uint8_t> payload) noexcept -> uint32_t {
  return ((uint32_t) ((payload[BYTE_POS_FOUR] << 24) & 0xFF000000) |
          (uint32_t) ((payload[BYTE_POS_FIVE] << 16) & 0x00FF0000) |
          (uint32_t) ((payload[BYTE_POS_SIX] << 8) & 0x0000FF00) | (uint32_t) ((payload[BYTE_POS_SEVEN] & 0x000000FF)));
}

auto TcpChannelHandlerImpl::ProcessDoIPPayload(DoipMessage &doip_payload) noexcept -> void {
  std::lock_guard<std::mutex> const lck(channel_handler_lock);
  switch (doip_payload.payload_type) {
    case kDoip_RoutingActivation_ResType: {
      // Process RoutingActivation response
      routing_activation_handler_.ProcessDoIPRoutingActivationResponse(doip_payload);
      break;
    }
    case kDoip_DiagMessage_Type: {
      // Process Diagnostic Message Response
      diagnostic_message_handler_.ProcessDoIPDiagnosticMessageResponse(doip_payload);
      break;
    }
    case kDoip_DiagMessagePosAck_Type:
    case kDoip_DiagMessageNegAck_Type: {
      // Process positive or negative diag ack message
      diagnostic_message_handler_.ProcessDoIPDiagnosticAckMessageResponse(doip_payload);
      break;
    }
    default:
      /* do nothing */
      break;
  }
}
}  // namespace tcpChannelHandlerImpl
}  // namespace doip_client
