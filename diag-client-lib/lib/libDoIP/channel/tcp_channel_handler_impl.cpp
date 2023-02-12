/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "channel/tcp_channel_handler_impl.h"

#include "channel/tcp_channel.h"
#include "handler/tcp_transport_handler.h"

namespace ara {
namespace diag {
namespace doip {
namespace tcpChannelHandlerImpl {
auto RoutingActivationHandler::ProcessDoIPRoutingActivationResponse(
  DoipMessage &doip_payload) noexcept -> void {
  RoutingActivationChannelState final_state{RoutingActivationChannelState::kRoutingActivationFailed};
  if (channel_
        .GetChannelState()
        .GetRoutingActivationStateContext()
        .GetActiveState()
        .GetState() == RoutingActivationChannelState::kWaitForRoutingActivationRes) {
    // get the logical address of client
    uint16_t client_address = (uint16_t) ((doip_payload.payload[BYTE_POS_ZERO] << 8) & 0xFF00) |
                              (uint16_t) (doip_payload.payload[BYTE_POS_ONE] & 0x00FF);
    // get the logical address of Server
    uint16_t server_address = (uint16_t) ((doip_payload.payload[BYTE_POS_TWO] << 8) & 0xFF00) |
                              (uint16_t) (doip_payload.payload[BYTE_POS_THREE] & 0x00FF);
    // get the ack code
    uint8_t act_type = doip_payload.payload[BYTE_POS_FOUR];
    switch (act_type) {
      case kDoip_RoutingActivation_ResCode_RoutingSuccessful: {
        // routing successful
        final_state = RoutingActivationChannelState::kRoutingActivationSuccessful;
      }
        break;
      case kDoip_RoutingActivation_ResCode_ConfirmtnRequired: {
        // trigger routing activation after sometime, not implemented yet
      }
        break;
      default:
        // failure, do nothing
        break;
    }
    channel_
      .GetChannelState()
      .GetRoutingActivationStateContext()
      .TransitionTo(final_state);
    channel_.WaitCancel();
  } else {
    /* ignore */
  }
}

auto RoutingActivationHandler::SendRoutingActivationRequest(
  uds_transport::UdsMessageConstPtr &message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult
    ret_val{uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  TcpMessagePtr doip_routing_act_req{std::make_unique<TcpMessage>()};
  // reserve bytes in vector
  doip_routing_act_req->txBuffer_.reserve(kDoipheadrSize + kDoip_RoutingActivation_ReqMinLen);
  // create header
  CreateDoipGenericHeader(doip_routing_act_req->txBuffer_,
                          kDoip_RoutingActivation_ReqType,
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

auto RoutingActivationHandler::CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader,
                                                       uint16_t payloadType,
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

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticAckMessageResponse(
  DoipMessage &doip_payload) noexcept -> void {
  DiagnosticMessageChannelState final_state{DiagnosticMessageChannelState::kDiagnosticNegativeAckRecvd};
  if (channel_
        .GetChannelState()
        .GetDiagnosticMessageStateContext()
        .GetActiveState()
        .GetState() == DiagnosticMessageChannelState::kWaitForDiagnosticAck) {
    // check the logical address of Server
    uint16_t server_address = (uint16_t) (((doip_payload.payload[BYTE_POS_ZERO] & 0xFF) << 8) |
                                          (doip_payload.payload[BYTE_POS_ONE] & 0xFF));
    // check the logical address of client
    uint16_t client_address = (uint16_t) (((doip_payload.payload[BYTE_POS_TWO] & 0xFF) << 8) |
                                          (doip_payload.payload[BYTE_POS_THREE] & 0xFF));
    if (doip_payload.payload_type == kDoip_DiagMessagePosAck_Type) {
      // get the ack code
      uint8_t ack_code = doip_payload.payload[BYTE_POS_FOUR];
      if (ack_code == kDoip_DiagnosticMessage_PosAckCode_Confirm) {
        final_state = DiagnosticMessageChannelState::kDiagnosticPositiveAckRecvd;
      } else {
        // do nothing
      }
    } else if (doip_payload.payload_type == kDoip_DiagMessageNegAck_Type) {
      // do nothing
    } else {
      // do nothing
    }
    channel_
      .GetChannelState()
      .GetDiagnosticMessageStateContext()
      .TransitionTo(final_state);
    channel_.WaitCancel();
  } else {
    // ignore
  }
}

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticMessageResponse(
  DoipMessage &doip_payload) noexcept -> void {
  if (channel_
        .GetChannelState()
        .GetDiagnosticMessageStateContext()
        .GetActiveState()
        .GetState() == DiagnosticMessageChannelState::kWaitForDiagnosticResponse) {
    // create the payload to send to upper layer
    std::vector<uint8_t> payload_info;
    // check the logical address of Server
    auto server_address = (uint16_t) (((doip_payload.payload[BYTE_POS_ZERO] & 0xFF) << 8) |
                                      (doip_payload.payload[BYTE_POS_ONE] & 0xFF));
    // check the logical address of client
    auto client_address = (uint16_t) (((doip_payload.payload[BYTE_POS_TWO] & 0xFF) << 8) |
                                      (doip_payload.payload[BYTE_POS_THREE] & 0xFF));
    // payload except the address
    payload_info.resize(doip_payload.payload.size() - 4U);
    // copy to application buffer
    (void) std::copy(doip_payload.payload.begin() + 4,
                     doip_payload.payload.end(), payload_info.begin());
    // Indicate upper layer about incoming data
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>
      ret_val{tcp_transport_handler_.IndicateMessage(
      static_cast<ara::diag::uds_transport::UdsMessage::Address>(server_address),
      static_cast<ara::diag::uds_transport::UdsMessage::Address>(client_address),
      ara::diag::uds_transport::UdsMessage::TargetAddressType::kPhysical,
      0U,
      static_cast<std::size_t>(doip_payload.payload.size() - 4U),
      0U,
      "DoIPTcp",
      payload_info)};
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
        // other errors
        // set to idle
        // raise error
      }
      channel_
        .GetChannelState()
        .GetDiagnosticMessageStateContext()
        .TransitionTo(DiagnosticMessageChannelState::kDiagIdle);
    }
  } else {
    // ignore
  }
}

auto DiagnosticMessageHandler::SendDiagnosticRequest(
  uds_transport::UdsMessageConstPtr &message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult
    ret_val{ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  TcpMessagePtr doip_diag_req = std::make_unique<TcpMessage>();
  // reserve bytes in vector
  doip_diag_req->txBuffer_.reserve(kDoipheadrSize + kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
  // create header
  CreateDoipGenericHeader(doip_diag_req->txBuffer_,
                          kDoip_DiagMessage_Type,
                          kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
  // Add source address
  doip_diag_req->txBuffer_.push_back((uint8_t) ((message->GetSa() & 0xFF00) >> 8));
  doip_diag_req->txBuffer_.push_back((uint8_t) (message->GetSa() & 0x00FF));
  // Add target address
  doip_diag_req->txBuffer_.push_back((uint8_t) ((message->GetTa() & 0xFF00) >> 8));
  doip_diag_req->txBuffer_.push_back((uint8_t) (message->GetTa() & 0x00FF));
  // Add data bytes
  for (std::uint8_t byte: message->GetPayload()) {
    doip_diag_req->txBuffer_.push_back(byte);
  }
  // transmit
  if (!(tcp_socket_handler_.Transmit(std::move(doip_diag_req)))) {
    // do nothing
  } else {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

auto DiagnosticMessageHandler::CreateDoipGenericHeader(
  std::vector<uint8_t> &doipHeader,
  uint16_t payloadType,
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

auto TcpChannelHandlerImpl::SendRoutingActivationRequest(
  uds_transport::UdsMessageConstPtr &message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return (routing_activation_handler_.SendRoutingActivationRequest(message));
}

auto TcpChannelHandlerImpl::SendDiagnosticRequest(
  uds_transport::UdsMessageConstPtr &message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  return (diagnostic_message_handler_.SendDiagnosticRequest(message));
}

auto TcpChannelHandlerImpl::HandleMessage(TcpMessagePtr tcp_rx_message) noexcept -> void {
  uint8_t nackCode;
  DoipMessage doip_rx_message;
  doip_rx_message.protocol_version = tcp_rx_message->rxBuffer_[0];
  doip_rx_message.protocol_version_inv = tcp_rx_message->rxBuffer_[1];
  doip_rx_message.payload_type = GetDoIPPayloadType(tcp_rx_message->rxBuffer_);
  doip_rx_message.payload_length = GetDoIPPayloadLength(tcp_rx_message->rxBuffer_);
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nackCode)) {
    doip_rx_message.payload.resize(tcp_rx_message->rxBuffer_.size() - kDoipheadrSize);
    // copy payload locally
    (void) std::copy(tcp_rx_message->rxBuffer_.begin() + kDoipheadrSize,
                     tcp_rx_message->rxBuffer_.begin() + kDoipheadrSize + tcp_rx_message->rxBuffer_.size(),
                     doip_rx_message.payload.begin());
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    // SendDoIPNACKMessage(nackCode);
    (void) nackCode;
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
          if (ProcessDoIPPayloadLength(
            doip_rx_message.payload_length, doip_rx_message.payload_type)) {
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
      if (payloadLen <= (uint32_t) kDoip_RoutingActivation_ResMaxLen)
        ret_val = true;
      break;
    }
    case kDoip_DiagMessagePosAck_Type:
    case kDoip_DiagMessageNegAck_Type: {
      if (payloadLen >= (uint32_t) kDoip_DiagMessageAck_ResMinLen)
        ret_val = true;
      break;
    }
    case kDoip_DiagMessage_Type: {
      // Req - [20-11][AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00122]
      if (payloadLen >= (uint32_t) (kDoip_DiagMessage_ReqResMinLen + 1u))
        ret_val = true;
      break;
    }
    case kDoip_AliveCheck_ReqType: {
      if (payloadLen <= (uint32_t) kDoip_RoutingActivation_ResMaxLen)
        ret_val = true;
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
}  // namespace doip
}  // namespace diag
}  // namespace ara