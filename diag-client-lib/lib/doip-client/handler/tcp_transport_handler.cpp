/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "handler/tcp_transport_handler.h"

#include "connection/connection_manager.h"

namespace doip_client {
namespace tcp_transport {
// ctor
TcpTransportHandler::TcpTransportHandler(std::string_view local_ip_address, uint16_t port_num,
                                         uint8_t total_tcp_channel_req, uds_transport::Connection &connection)
    : connection_{connection},
      tcp_channel_(std::make_unique<tcpChannel::TcpChannel>(local_ip_address, *this)) {
  (void) port_num;
  (void) total_tcp_channel_req;
}

// dtor
TcpTransportHandler::~TcpTransportHandler() = default;

// Initialize
uds_transport::UdsTransportProtocolHandler::InitializationResult TcpTransportHandler::Initialize() {
  return (tcp_channel_->Initialize());
}

void TcpTransportHandler::Start() { tcp_channel_->Start(); }

void TcpTransportHandler::Stop() { tcp_channel_->Stop(); }

bool TcpTransportHandler::IsConnectToHost() { return (tcp_channel_->IsConnectToHost()); }

uds_transport::UdsTransportProtocolMgr::ConnectionResult TcpTransportHandler::ConnectToHost(
    uds_transport::UdsMessageConstPtr message) {
  return (tcp_channel_->ConnectToHost(std::move(message)));
}

uds_transport::UdsTransportProtocolMgr::DisconnectionResult TcpTransportHandler::DisconnectFromHost() {
  return (tcp_channel_->DisconnectFromHost());
}

uds_transport::UdsTransportProtocolMgr::TransmissionResult TcpTransportHandler::Transmit(
    uds_transport::UdsMessageConstPtr message, uds_transport::ChannelID channel_id) {
  // find the corresponding channel
  // Trigger transmit
  return (tcp_channel_->Transmit(std::move(message)));
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
TcpTransportHandler::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                     uds_transport::UdsMessage::Address target_addr,
                                     uds_transport::UdsMessage::TargetAddressType type,
                                     uds_transport::ChannelID channel_id, std::size_t size,
                                     uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                                     core_type::Span<std::uint8_t> payloadInfo) {
  return (connection_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                      payloadInfo));
}

// Hands over a valid received Uds message (currently this is only a request type) from transport
// layer to session layer
void TcpTransportHandler::HandleMessage(uds_transport::UdsMessagePtr message) {
  connection_.HandleMessage(std::move(message));
}

}  // namespace tcp_transport
}  // namespace doip_client
