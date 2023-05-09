/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LIBDOIP_CONNECTION_CONNECTION_MANAGER_H
#define LIB_LIBDOIP_CONNECTION_CONNECTION_MANAGER_H

#include <string_view>

#include "common/common_doip_types.h"

namespace doip_client {

//forward declaration
namespace tcpTransport {
class TcpTransportHandler;
}

namespace udpTransport {
class UdpTransportHandler;
}

namespace connection {
/*
 @ Class Name        : DoipTcpConnection
 @ Class Description : Class to create connection to tcp handler
 */
class DoipTcpConnection final : public uds_transport::Connection {
public:
  // type alias for initialization result
  using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

  // ctor
  DoipTcpConnection(const std::shared_ptr<uds_transport::ConversionHandler> &conversion,
                    std::string_view tcp_ip_address, uint16_t port_num);

  // dtor
  ~DoipTcpConnection() override = default;

  // Initialize
  InitializationResult Initialize() override;

  // Start the connection
  void Start() override;

  // Stop the connection
  void Stop() override;

  // Check if already connected to host
  bool IsConnectToHost() override;

  // Connect to host using the ip address
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(
      uds_transport::UdsMessageConstPtr message) override;

  // Disconnect from Host Server
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() override;

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo) override;

  // Transmit tcp
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      uds_transport::UdsMessageConstPtr message) override;

  // Hands over a valid message to conversion
  void HandleMessage(uds_transport::UdsMessagePtr message) override;

private:
  // Tcp Transport Handler
  std::unique_ptr<tcpTransport::TcpTransportHandler> tcp_transport_handler_;
};

/*
 @ Class Name        : DoipUdpConnection
 @ Class Description : Class to create connection to udp handler
 */
class DoipUdpConnection final : public uds_transport::Connection {
public:
  // type alias for initialization result
  using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

  // ctor
  DoipUdpConnection(const std::shared_ptr<uds_transport::ConversionHandler> &conversation,
                    std::string_view udp_ip_address, uint16_t port_num);

  // dtor
  ~DoipUdpConnection() override = default;

  // Initialize
  InitializationResult Initialize() override;

  // Start the connection
  void Start() override;

  // Stop the connection
  void Stop() override;

  // Check if already connected to host
  bool IsConnectToHost() override;

  // Connect to host using the ip address
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(
      uds_transport::UdsMessageConstPtr message) override;

  // Disconnect from Host Server
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() override;

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo) override;

  // Transmit tcp
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      uds_transport::UdsMessageConstPtr message) override;

  // Hands over a valid message to conversion
  void HandleMessage(uds_transport::UdsMessagePtr message) override;

private:
  // Udp Transport Handler
  std::unique_ptr<udpTransport::UdpTransportHandler> udp_transport_handler_;
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
  std::shared_ptr<DoipTcpConnection> FindOrCreateTcpConnection(
      const std::shared_ptr<uds_transport::ConversionHandler> &conversation, std::string_view tcp_ip_address,
      uint16_t port_num);

  // Function to create new connection to handle doip udp request and response
  std::shared_ptr<DoipUdpConnection> FindOrCreateUdpConnection(
      const std::shared_ptr<uds_transport::ConversionHandler> &conversation, std::string_view udp_ip_address,
      uint16_t port_num);
};
}  // namespace connection
}  // namespace doip_client

#endif  // LIB_LIBDOIP_CONNECTION_CONNECTION_MANAGER_H