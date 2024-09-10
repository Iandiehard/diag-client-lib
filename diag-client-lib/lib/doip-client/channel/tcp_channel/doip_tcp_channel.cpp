/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/tcp_channel/doip_tcp_channel.h"

#include <utility>

#include "common/logger.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

DoipTcpChannel::DoipTcpChannel(TcpSocketHandler tcp_socket_handler,
                               uds_transport::Connection &connection)
    : tcp_socket_handler_{std::move(tcp_socket_handler)},
      tcp_channel_handler_{tcp_socket_handler_, *this},
      connection_{connection} {}

void DoipTcpChannel::Start() {
  // Set the handler to receive data from socket handler
  tcp_socket_handler_.SetReadHandler(
      [this](TcpMessagePtr tcp_message) { ProcessReceivedTcpMessage(std::move(tcp_message)); });
  // Start the socket and channel handler
  tcp_socket_handler_.Initialize();
  tcp_channel_handler_.Start();
}

void DoipTcpChannel::Stop() {
  tcp_socket_handler_.DeInitialize();
  tcp_channel_handler_.Stop();
}

bool DoipTcpChannel::IsConnectedToHost() { return tcp_socket_handler_.IsConnectedToHost(); }

uds_transport::UdsTransportProtocolMgr::ConnectionResult DoipTcpChannel::ConnectToHost(
    uds_transport::UdsMessageConstPtr message) {
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};

  uds_transport::UdsMessage::IpAddress const kHostIpAddress{message->GetHostIpAddress()};
  uds_transport::UdsMessage::PortNumber const kHostPortNumber{message->GetHostPortNumber()};
  // Initiate connecting to server
  if (tcp_socket_handler_.ConnectToHost(kHostIpAddress, kHostPortNumber)) {
    // Once connected, Send routing activation req and get response
    ret_val = tcp_channel_handler_.SendRoutingActivationRequest(std::move(message));
  } else {  // failure
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [&kHostIpAddress, &kHostPortNumber](std::stringstream &msg) {
          msg << "Doip Tcp socket connect failed for remote endpoints : "
              << "<Ip: " << kHostIpAddress << ", Port: " << kHostPortNumber << ">";
        });
  }
  return ret_val;
}

uds_transport::UdsTransportProtocolMgr::DisconnectionResult DoipTcpChannel::DisconnectFromHost() {
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed};
  if (tcp_socket_handler_.DisconnectFromHost()) {
    if (tcp_channel_handler_.IsRoutingActivated()) {
      // Reset the handler
      tcp_channel_handler_.Reset();
    }
    ret_val = uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionOk;
  }
  return ret_val;
}

void DoipTcpChannel::ProcessReceivedTcpMessage(TcpMessagePtr tcp_rx_message) {
  tcp_channel_handler_.HandleMessage(std::move(tcp_rx_message));
}

uds_transport::UdsTransportProtocolMgr::TransmissionResult DoipTcpChannel::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  // Routing activation should be active before sending diag request
  if (tcp_channel_handler_.IsRoutingActivated()) {
    ret_val = tcp_channel_handler_.SendDiagnosticRequest(std::move(message));
  } else {
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [](std::stringstream &msg) {
          msg << "Routing Activation required, please connect to server first";
        });
  }
  return ret_val;
}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DoipTcpChannel::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                uds_transport::UdsMessage::Address target_addr,
                                uds_transport::UdsMessage::TargetAddressType type,
                                uds_transport::ChannelID channel_id, std::size_t size,
                                uds_transport::Priority priority,
                                uds_transport::ProtocolKind protocol_kind,
                                core_type::Span<std::uint8_t const> payload_info) {
  return connection_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority,
                                     protocol_kind, payload_info);
}

void DoipTcpChannel::HandleMessage(uds_transport::UdsMessagePtr message) {
  connection_.HandleMessage(std::move(message));
}

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
