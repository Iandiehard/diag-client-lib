/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
//includes
#include "connection/connection_manager.h"

#include "handler/tcp_transport_handler.h"
#include "handler/udp_transport_handler.h"

namespace doip_client {
namespace connection {
/*
 @ Class Name        : DoipConnection
 @ Class Description : Class to create connection to tcp & udp handler                              
 */
// ctor
DoipTcpConnection::DoipTcpConnection(uds_transport::ConversionHandler &conversion, std::string_view tcp_ip_address,
                                     uint16_t port_num)
    : uds_transport::Connection(1, conversion),
      tcp_transport_handler_(std::make_unique<tcpTransport::TcpTransportHandler>(tcp_ip_address, port_num, 1, *this)) {}

// Initialize
DoipTcpConnection::InitializationResult DoipTcpConnection::Initialize() {
  (void) tcp_transport_handler_->Initialize();
  return (InitializationResult::kInitializeOk);
}

// Start the Tp Handlers
void DoipTcpConnection::Start() { tcp_transport_handler_->Start(); }

// Stop the Tp handlers
void DoipTcpConnection::Stop() { tcp_transport_handler_->Stop(); }

// Check if already connected to host
bool DoipTcpConnection::IsConnectToHost() { return (tcp_transport_handler_->IsConnectToHost()); }

// Connect to host server
uds_transport::UdsTransportProtocolMgr::ConnectionResult DoipTcpConnection::ConnectToHost(
    uds_transport::UdsMessageConstPtr message) {
  return (tcp_transport_handler_->ConnectToHost(std::move(message)));
}

// Disconnect from host server
uds_transport::UdsTransportProtocolMgr::DisconnectionResult DoipTcpConnection::DisconnectFromHost() {
  return (tcp_transport_handler_->DisconnectFromHost());
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DoipTcpConnection::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                   uds_transport::UdsMessage::Address target_addr,
                                   uds_transport::UdsMessage::TargetAddressType type,
                                   uds_transport::ChannelID channel_id, std::size_t size,
                                   uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                                   std::vector<uint8_t> payloadInfo) {
  // Send Indication to conversion
  return (conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                        payloadInfo));
}

// Function to transmit the uds message
uds_transport::UdsTransportProtocolMgr::TransmissionResult DoipTcpConnection::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  uds_transport::ChannelID channel_id = 0;
  return (tcp_transport_handler_->Transmit(std::move(message), channel_id));
}

// Hands over a valid message to conversion
void DoipTcpConnection::HandleMessage(uds_transport::UdsMessagePtr message) {
  // send full message to conversion
  conversation_.HandleMessage(std::move(message));
}

/*
 @ Class Name        : DoipUdpConnection
 @ Class Description : Class to create connection to udp handler                              
 */
// ctor
DoipUdpConnection::DoipUdpConnection(uds_transport::ConversionHandler &conversation, std::string_view udp_ip_address,
                                     uint16_t port_num)
    : uds_transport::Connection(1, conversation),
      udp_transport_handler_{std::make_unique<udpTransport::UdpTransportHandler>(udp_ip_address, port_num, *this)} {}

// Initialize
DoipUdpConnection::InitializationResult DoipUdpConnection::Initialize() {
  (void) udp_transport_handler_->Initialize();
  return InitializationResult::kInitializeOk;
}

// Start the Tp Handlers
void DoipUdpConnection::Start() { udp_transport_handler_->Start(); }

// Stop the Tp handlers
void DoipUdpConnection::Stop() { udp_transport_handler_->Stop(); }

// Check if already connected to host
bool DoipUdpConnection::IsConnectToHost() { return false; }

// Connect to host server
uds_transport::UdsTransportProtocolMgr::ConnectionResult DoipUdpConnection::ConnectToHost(
    uds_transport::UdsMessageConstPtr message) {
  (void) message;
  return (uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed);
}

// Disconnect from host server
uds_transport::UdsTransportProtocolMgr::DisconnectionResult DoipUdpConnection::DisconnectFromHost() {
  return (uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed);
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DoipUdpConnection::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                   uds_transport::UdsMessage::Address target_addr,
                                   uds_transport::UdsMessage::TargetAddressType type,
                                   uds_transport::ChannelID channel_id, std::size_t size,
                                   uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                                   std::vector<uint8_t> payloadInfo) {
  // Send Indication to conversion
  return (conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                        payloadInfo));
}

// Function to transmit the uds message
uds_transport::UdsTransportProtocolMgr::TransmissionResult DoipUdpConnection::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  uds_transport::ChannelID channel_id = 0;
  return (udp_transport_handler_->Transmit(std::move(message), channel_id));
}

// Hands over a valid message to conversion
void DoipUdpConnection::HandleMessage(uds_transport::UdsMessagePtr message) {
  // send full message to conversion
  conversation_.HandleMessage(std::move(message));
}

// Function to create new connection to handle doip request and response
std::shared_ptr<DoipTcpConnection> DoipConnectionManager::FindOrCreateTcpConnection(
    uds_transport::ConversionHandler &conversation, std::string_view tcp_ip_address, uint16_t port_num) {
  return (std::make_shared<DoipTcpConnection>(conversation, tcp_ip_address, port_num));
}

std::shared_ptr<DoipUdpConnection> DoipConnectionManager::FindOrCreateUdpConnection(
    uds_transport::ConversionHandler &conversation, std::string_view udp_ip_address, uint16_t port_num) {
  return (std::make_shared<DoipUdpConnection>(conversation, udp_ip_address, port_num));
}
}  // namespace connection
}  // namespace doip_client
