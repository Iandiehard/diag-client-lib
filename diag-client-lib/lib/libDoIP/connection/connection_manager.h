/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LIBDOIP_CONNECTION_CONNECTION_MANAGER_H
#define LIB_LIBDOIP_CONNECTION_CONNECTION_MANAGER_H
//includes
#include "common/common_doip_types.h"

namespace ara {
namespace diag {
namespace doip {
//forward declaration
namespace tcpTransport {
class TcpTransportHandler;
}

namespace udpTransport {
class UdpTransportHandler;
}

using InitializationResult = ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult;

namespace connection {
/*
 @ Class Name        : DoipTcpConnection
 @ Class Description : Class to create connection to tcp handler
 */
class DoipTcpConnection : public ara::diag::connection::Connection {
public:
  // ctor
  DoipTcpConnection(
    const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion,
    kDoip_String &tcp_ip_address,
    uint16_t port_num);
  
  // dtor
  ~DoipTcpConnection() = default;
  
  // Initialize
  InitializationResult Initialize() override;
  
  // Start the connection
  void Start() override;
  
  // Stop the connection
  void Stop() override;
  
  // Connect to host using the ip address
  ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
  ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) override;
  
  // Disconnect from Host Server
  ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
  DisconnectFromHost() override;
  
  // Indicate message Diagnostic message reception over TCP to user
  std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
    ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(
    ara::diag::uds_transport::UdsMessage::Address source_addr,
    ara::diag::uds_transport::UdsMessage::Address target_addr,
    ara::diag::uds_transport::UdsMessage::TargetAddressType type,
    ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size,
    ara::diag::uds_transport::Priority priority,
    ara::diag::uds_transport::ProtocolKind protocol_kind,
    std::vector<uint8_t> payloadInfo) override;
  
  // Transmit tcp
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
  Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) override;
  
  // Hands over a valid message to conversion
  void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) override;

private:
  // Tcp Transport Handler
  std::unique_ptr<ara::diag::doip::tcpTransport::TcpTransportHandler> tcp_transport_handler_;
};

/*
 @ Class Name        : DoipUdpConnection
 @ Class Description : Class to create connection to udp handler
 */
class DoipUdpConnection : public ara::diag::connection::Connection {
public:
  // ctor
  DoipUdpConnection(
    const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion,
    kDoip_String &udp_ip_address,
    uint16_t port_num);
  
  // dtor
  ~DoipUdpConnection() = default;
  
  // Initialize
  InitializationResult Initialize() override;
  
  // Start the connection
  void Start() override;
  
  // Stop the connection
  void Stop() override;
  
  // Connect to host using the ip address
  ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
  ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) override;
  
  // Disconnect from Host Server
  ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
  DisconnectFromHost() override;
  
  // Indicate message Diagnostic message reception over TCP to user
  std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
    ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(
    ara::diag::uds_transport::UdsMessage::Address source_addr,
    ara::diag::uds_transport::UdsMessage::Address target_addr,
    ara::diag::uds_transport::UdsMessage::TargetAddressType type,
    ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size,
    ara::diag::uds_transport::Priority priority,
    ara::diag::uds_transport::ProtocolKind protocol_kind,
    std::vector<uint8_t> payloadInfo) override;
  
  // Transmit tcp
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
  Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) override;
  
  // Hands over a valid message to conversion
  void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) override;

private:
  // Udp Transport Handler
  std::unique_ptr<ara::diag::doip::udpTransport::UdpTransportHandler> udp_transport_handler_;
};

/*
 @ Class Name        : DoipConnectionManager
 @ Class Description : Class manages Doip Connection                              
 */
class DoipConnectionManager {
public:
  // ctor
  DoipConnectionManager() = default;
  
  // dtor
  ~DoipConnectionManager() = default;
  
  // Function to create new connection to handle doip tcp request and response
  std::shared_ptr<DoipTcpConnection>
  FindOrCreateTcpConnection(
    const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversation,
    kDoip_String &tcp_ip_address,
    uint16_t port_num);
  
  // Function to create new connection to handle doip udp request and response
  std::shared_ptr<DoipUdpConnection>
  FindOrCreateUdpConnection(
    const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversation,
    kDoip_String &udp_ip_address,
    uint16_t port_num);
};
}  // namespace connection
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // LIB_LIBDOIP_CONNECTION_CONNECTION_MANAGER_H