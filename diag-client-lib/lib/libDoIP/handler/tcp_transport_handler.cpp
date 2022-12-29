/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "handler/tcp_transport_handler.h"
#include "connection/connection_manager.h"

namespace ara{
namespace diag{
namespace doip{
namespace tcpTransport{


// ctor
TcpTransportHandler::TcpTransportHandler(
  kDoip_String &local_ip_address, 
  uint16_t port_num, 
  uint8_t total_tcp_channel_req,
  connection::DoipTcpConnection& doip_connection)
    : doip_connection_{doip_connection},
      tcp_channel_(std::make_unique<ara::diag::doip::tcpChannel::tcpChannel>(local_ip_address, *this)) {
  (void)port_num;
  (void)total_tcp_channel_req;
}

// dtor
TcpTransportHandler::~TcpTransportHandler() {}

// Initialize
ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult
  TcpTransportHandler::Initialize() {
    return (tcp_channel_->Initialize());
}

// start handler
void TcpTransportHandler::Start() {
  tcp_channel_->Start();
}

// stop handler
void TcpTransportHandler::Stop() {
  tcp_channel_->Stop();
}

// Connect to remote Host
ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
  TcpTransportHandler::ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) {
    return(tcp_channel_->ConnectToHost(std::move(message)));
}

// Disconnect from remote host
ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
  TcpTransportHandler::DisconnectFromHost() {
    return(tcp_channel_->DisconnectFromHost());
}

// Transmit 
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
        TcpTransportHandler::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message,
                                            ara::diag::uds_transport::ChannelID channel_id) {
  // find the corresponding channel

  // Trigger transmit
  return(tcp_channel_->Transmit(std::move(message)));
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
  ara::diag::uds_transport::UdsMessagePtr>
  TcpTransportHandler::IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
    ara::diag::uds_transport::UdsMessage::Address target_addr,
    ara::diag::uds_transport::UdsMessage::TargetAddressType type,
    ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size,
    ara::diag::uds_transport::Priority priority,
    ara::diag::uds_transport::ProtocolKind protocol_kind,
    std::vector<uint8_t> payloadInfo) {
  return (doip_connection_.IndicateMessage(source_addr,
                                            target_addr, 
                                            type, 
                                            channel_id, 
                                            size,
                                            priority, 
                                            protocol_kind, 
                                            payloadInfo));
}

// Hands over a valid received Uds message (currently this is only a request type) from transport
// layer to session layer                
void TcpTransportHandler::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
  doip_connection_.HandleMessage(std::move(message));
}

} // tcpTransport
} // doip
} // diag
} // ara  
