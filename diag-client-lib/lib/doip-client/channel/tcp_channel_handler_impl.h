/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
//includes
#include <mutex>

#include "channel/tcp_channel_state_impl.h"
#include "common/common_doip_types.h"
#include "common/doip_message.h"
#include "core/include/common_header.h"
#include "sockets/tcp_socket_handler.h"

namespace doip_client {
// forward declaration
namespace tcpChannel {
class tcpChannel;
}

namespace tcpTransport {
class TcpTransportHandler;
}

namespace tcpChannelHandlerImpl {

using RoutingActivationChannelState = tcpChannelStateImpl::routingActivationState;
using DiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;
using TcpMessage = tcpSocket::TcpMessage;
using TcpMessagePtr = tcpSocket::TcpMessagePtr;

/*
 @ Class Name        : RoutingActivationHandler
 @ Class Description : Class used as a handler to process routing activation messages
 */
class RoutingActivationHandler {
public:
  // strong type holding activation type
  struct RoutingActivationAckType {
    std::uint8_t act_type_;
  };

public:
  // ctor
  RoutingActivationHandler(tcpSocket::TcpSocketHandler &tcp_socket_handler, tcpChannel::tcpChannel &channel)
      : tcp_socket_handler_{tcp_socket_handler},
        channel_{channel} {}

  // dtor
  ~RoutingActivationHandler() = default;

  // Function to process Routing activation response
  auto ProcessDoIPRoutingActivationResponse(DoipMessage &doip_payload) noexcept -> void;

  // Function to send Routing activation request
  auto SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr &message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

private:
  void CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType, uint32_t payloadLen);

private:
  // socket reference
  tcpSocket::TcpSocketHandler &tcp_socket_handler_;
  // channel reference
  tcpChannel::tcpChannel &channel_;
};

/*
 @ Class Name        : DiagnosticMessageHandler
 @ Class Description : Class used as a handler to process diagnostic messages
 */
class DiagnosticMessageHandler {
public:
  // strong type acknowledgement type
  struct DiagAckType {
    std::uint8_t ack_type_;
  };

public:
  // ctor
  DiagnosticMessageHandler(tcpSocket::TcpSocketHandler &tcp_socket_handler,
                           tcpTransport::TcpTransportHandler &tcp_transport_handler, tcpChannel::tcpChannel &channel)
      : tcp_socket_handler_{tcp_socket_handler},
        tcp_transport_handler_{tcp_transport_handler},
        channel_{channel} {}

  // dtor
  ~DiagnosticMessageHandler() = default;

  // Function to process Routing activation response
  auto ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept -> void;

  // Function to process Diagnostic message response
  auto ProcessDoIPDiagnosticMessageResponse(DoipMessage &doip_payload) noexcept -> void;

  // Function to send Diagnostic request
  auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr &message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

private:
  static auto CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType,
                                      uint32_t payloadLen) noexcept -> void;

  // socket reference
  tcpSocket::TcpSocketHandler &tcp_socket_handler_;
  // transport handler reference
  tcpTransport::TcpTransportHandler &tcp_transport_handler_;
  // channel reference
  tcpChannel::tcpChannel &channel_;
};

/*
 @ Class Name        : TcpChannelHandlerImpl
 @ Class Description : Class to handle received messages from lower layer
 */
class TcpChannelHandlerImpl {
public:
  // ctor
  TcpChannelHandlerImpl(tcpSocket::TcpSocketHandler &tcp_socket_handler,
                        tcpTransport::TcpTransportHandler &tcp_transport_handler, tcpChannel::tcpChannel &channel)
      : routing_activation_handler_{tcp_socket_handler, channel},
        diagnostic_message_handler_{tcp_socket_handler, tcp_transport_handler, channel} {}

  // dtor
  ~TcpChannelHandlerImpl() = default;

  // Function to trigger Routing activation request
  auto SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr &message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  // Function to send Diagnostic request
  auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr &message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  // process message
  auto HandleMessage(TcpMessagePtr tcp_rx_message) noexcept -> void;

private:
  // Function to process DoIP Header
  auto ProcessDoIPHeader(DoipMessage &doip_rx_message, uint8_t &nackCode) noexcept -> bool;

  // Function to verify payload length of various payload type
  auto ProcessDoIPPayloadLength(uint32_t payloadLen, uint16_t payloadType) noexcept -> bool;

  // Function to process DoIP payload responses
  auto ProcessDoIPPayload(DoipMessage &doip_payload) noexcept -> void;

  // handler to process routing activation req/ resp
  RoutingActivationHandler routing_activation_handler_;
  // handler to process diagnostic message req/ resp
  DiagnosticMessageHandler diagnostic_message_handler_;
  // mutex to protect critical section
  std::mutex channel_handler_lock;
};
}  // namespace tcpChannelHandlerImpl
}  // namespace doip_client

#endif  //DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
