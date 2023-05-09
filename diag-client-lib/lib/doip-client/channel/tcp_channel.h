/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H
//includes
#include <string_view>

#include "channel/tcp_channel_handler_impl.h"
#include "channel/tcp_channel_state_impl.h"
#include "common/common_doip_types.h"
#include "common_header.h"
#include "sockets/tcp_socket_handler.h"
#include "utility/sync_timer.h"

namespace doip_client {
// forward declaration
namespace tcpTransport {
class TcpTransportHandler;
}

namespace tcpChannel {
// typedefs
using TcpMessage = tcpSocket::TcpMessage;
using TcpMessagePtr = tcpSocket::TcpMessagePtr;
using TcpMessageConstPtr = tcpSocket::TcpMessageConstPtr;
using TcpRoutingActivationChannelState = tcpChannelStateImpl::routingActivationState;
using TcpDiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;
using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;
using SyncTimerState = SyncTimer::TimerState;

/*
 @ Class Name        : tcpChannel
 @ Class Description : Class used to handle Doip Tcp Channel                              
 @ Threads           : Per Tcp channel one threads will be spawned (one for tcp socket)
 */
class tcpChannel {
public:
  //  socket state
  enum class tcpSocketState : std::uint8_t { kSocketOffline = 0U, kSocketOnline };

  //ctor
  tcpChannel(std::string_view localIpaddress, tcpTransport::TcpTransportHandler &tcpTransport_Handler);

  //dtor
  ~tcpChannel();

  // Initialize
  uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();

  //Start
  void Start();

  // Stop
  void Stop();

  // Check if already connected to host
  bool IsConnectToHost();

  // Function to connect to host
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(uds_transport::UdsMessageConstPtr message);

  // Function to disconnect from host
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost();

  // Function to Hand over all the message received
  void HandleMessage(TcpMessagePtr tcp_rx_message);

  // Function to trigger transmission
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(uds_transport::UdsMessageConstPtr message);

  // Function to get the channel context
  auto GetChannelState() noexcept -> tcpChannelStateImpl::TcpChannelStateImpl & { return tcp_channel_state_; }

  // Function to wait for response
  void WaitForResponse(std::function<void()> &&timeout_func, std::function<void()> &&cancel_func, int msec);

  // Function to cancel the synchronous wait
  void WaitCancel();

private:
  // Function to handle the routing states
  uds_transport::UdsTransportProtocolMgr::ConnectionResult HandleRoutingActivationState(
      uds_transport::UdsMessageConstPtr &message);

  // Function to handle the diagnostic request response state
  uds_transport::UdsTransportProtocolMgr::TransmissionResult HandleDiagnosticRequestState(
      uds_transport::UdsMessageConstPtr &message);

private:
  // tcp socket handler
  std::unique_ptr<tcpSocket::TcpSocketHandler> tcp_socket_handler_;
  // tcp socket state
  tcpSocketState tcp_socket_state_{tcpSocketState::kSocketOffline};
  // tcp channel state
  tcpChannelStateImpl::TcpChannelStateImpl tcp_channel_state_;
  // tcp channel handler
  tcpChannelHandlerImpl::TcpChannelHandlerImpl tcp_channel_handler_;
  // sync timer
  SyncTimer sync_timer_;
};

}  // namespace tcpChannel
}  // namespace doip_client
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H
