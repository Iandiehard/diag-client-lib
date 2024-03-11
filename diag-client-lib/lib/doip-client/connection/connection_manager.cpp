/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "connection/connection_manager.h"

#include "channel/tcp_channel/doip_tcp_channel.h"
#include "channel/udp_channel/doip_udp_channel.h"
#include "sockets/socket_handler.h"
#include "uds_transport/conversation_handler.h"

namespace doip_client {
namespace connection {

/**
 * @brief    Doip Tcp Connection handle connection between two layers
 */
class DoipTcpConnection final : public uds_transport::Connection {
 public:
  /**
   * @brief   Type alias for Initialization result
   */
  using InitializationResult = uds_transport::Connection::InitializationResult;

  /**
   * @brief   Type alias for Tcp client used by socket handler
   */
  using TcpClient = sockets::TcpSocketHandler::Client;

  /**
   * @brief       Constructor to create a new tcp connection
   * @param[in]   conversation_handler
   *              The reference to conversation handler
   * @param[in]   tcp_ip_address
   *              The local tcp ip address
   * @param[in]   port_num
   *              The local port number
   * @param[in]   io_context
   *              The reference to io context
   */
  DoipTcpConnection(uds_transport::ConversionHandler const &conversation_handler, std::string_view tcp_ip_address,
                    std::uint16_t port_num)
      : uds_transport::Connection{1u, conversation_handler},
        doip_tcp_channel_{sockets::TcpSocketHandler{TcpClient{tcp_ip_address, port_num}}, *this} {}

  /**
   * @brief         Destruct an instance of DoipTcpConnection
   */
  ~DoipTcpConnection() final = default;

  /**
   * @brief        Function to initialize the connection
   * @return       The initialization result
   */
  InitializationResult Initialize() override { return InitializationResult::kInitializeOk; }

  /**
   * @brief        Function to start the connection
   */
  void Start() override { doip_tcp_channel_.Start(); }

  /**
   * @brief        Function to stop the connection
   */
  void Stop() override { doip_tcp_channel_.Stop(); }

  /**
   * @brief        Function to check if connected to host remote server
   * @return       True if connected, False otherwise
   */
  bool IsConnectToHost() override { return (doip_tcp_channel_.IsConnectedToHost()); }

  /**
   * @brief       Function to establish connection to remote host server
   * @param[in]   message
   *              The connection message
   * @return      Connection result
   */
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(
      uds_transport::UdsMessageConstPtr message) override {
    return (doip_tcp_channel_.ConnectToHost(std::move(message)));
  }

  /**
   * @brief       Function to disconnect from remote host server
   * @return      Disconnection result
   */
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() override {
    return (doip_tcp_channel_.DisconnectFromHost());
  }

  /**
   * @brief       Function to indicate a start of reception of message
   * @details     This is called to indicate the reception of new message by underlying transport protocol handler
   * @param[in]   source_addr
   *              The UDS source address of message
   * @param[in]   target_addr
   *              The UDS target address of message
   * @param[in]   type
   *              The indication whether its is phys/func request
   * @param[in]   channel_id
   *              The transport protocol channel on which message start happened
   * @param[in]   size
   *              The size in bytes of the UdsMessage starting from SID
   * @param[in]   priority
   *              The priority of the given message, used for prioritization of conversations
   * @param[in]   protocol_kind
   *              The identifier of protocol kind associated to message
   * @param[in]   payload_info
   *              The view onto the first received payload bytes, if any. This view shall be used only within this function call.
   *              It is recommended that the TP provides at least the first two bytes of the request message,
   *              so the DM can identify a functional TesterPresent
   * @return      std::pair< IndicationResult, UdsMessagePtr >
   *              The pair of IndicationResult and a pointer to UdsMessage owned/created by DM core and returned
   *              to the handler to get filled
   */
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
      core_type::Span<std::uint8_t const> payload_info) override {
    return (conversation_handler_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority,
                                                  protocol_kind, payload_info));
  }

  /**
   * @brief       Function to transmit a valid Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request.
   */
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      uds_transport::UdsMessageConstPtr message) override {
    return doip_tcp_channel_.Transmit(std::move(message));
  }

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(uds_transport::UdsMessagePtr message) override {
    conversation_handler_.HandleMessage(std::move(message));
  }

 private:
  /**
   * @brief        Store the doip tcp channel
   */
  channel::tcp_channel::DoipTcpChannel doip_tcp_channel_;
};

/**
 * @brief    Doip Udp Connection handle connection between two layers
 */
class DoipUdpConnection final : public uds_transport::Connection {
 public:
  /**
   * @brief   Type alias for Initialization result
   */
  using InitializationResult = uds_transport::Connection::InitializationResult;

  /**
   * @brief   Type alias for Tcp client used by socket handler
   */
  using UdpClient = sockets::UdpSocketHandler::Client;

  /**
   * @brief       Constructor to create a new udp connection
   * @param[in]   conversation_handler
   *              The reference to conversation handler
   * @param[in]   udp_ip_address
   *              The local tcp ip address
   * @param[in]   port_num
   *              The local port number
   */
  DoipUdpConnection(uds_transport::ConversionHandler const &conversation_handler, std::string_view udp_ip_address,
                    std::uint16_t port_num)
      : uds_transport::Connection(1, conversation_handler),
        doip_udp_channel_{sockets::UdpSocketHandler{UdpClient{udp_ip_address, port_num}},
                          sockets::UdpSocketHandler{UdpClient{udp_ip_address, port_num}}, *this} {}

  /**
   * @brief         Destruct an instance of DoipUdpConnection
   */
  ~DoipUdpConnection() final = default;

  /**
   * @brief        Function to initialize the connection
   * @return       The initialization result
   */
  InitializationResult Initialize() override { return InitializationResult::kInitializeOk; }

  /**
   * @brief        Function to start the connection
   */
  void Start() override { doip_udp_channel_.Start(); }

  /**
   * @brief        Function to stop the connection
   */
  void Stop() override { doip_udp_channel_.Stop(); }

  /**
   * @brief        Function to check if connected to host remote server
   * @return       True if connection, False otherwise
   */
  bool IsConnectToHost() override { return false; }

  /**
   * @brief       Function to establish connection to remote host server
   * @param[in]   message
   *              The connection message
   * @return      Connection result
   */
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(uds_transport::UdsMessageConstPtr) override {
    return (uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed);
  }

  /**
   * @brief       Function to disconnect from remote host server
   * @return      Disconnection result
   */
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() override {
    return (uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed);
  }

  /**
   * @brief       Function to indicate a start of reception of message
   * @details     This is called to indicate the reception of new message by underlying transport protocol handler
   * @param[in]   source_addr
   *              The UDS source address of message
   * @param[in]   target_addr
   *              The UDS target address of message
   * @param[in]   type
   *              The indication whether its is phys/func request
   * @param[in]   channel_id
   *              The transport protocol channel on which message start happened
   * @param[in]   size
   *              The size in bytes of the UdsMessage starting from SID
   * @param[in]   priority
   *              The priority of the given message, used for prioritization of conversations
   * @param[in]   protocol_kind
   *              The identifier of protocol kind associated to message
   * @param[in]   payload_info
   *              The view onto the first received payload bytes, if any. This view shall be used only within this function call.
   *              It is recommended that the TP provides at least the first two bytes of the request message,
   *              so the DM can identify a functional TesterPresent
   * @return      std::pair< IndicationResult, UdsMessagePtr >
   *              The pair of IndicationResult and a pointer to UdsMessage owned/created by DM core and returned
   *              to the handler to get filled
   */
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
      core_type::Span<std::uint8_t const> payload_info) override {
    // Send Indication to conversion
    return (conversation_handler_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority,
                                                  protocol_kind, payload_info));
  }

  /**
   * @brief       Function to transmit a valid Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request.
   */
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      uds_transport::UdsMessageConstPtr message) override {
    return (doip_udp_channel_.Transmit(std::move(message)));
  }

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(uds_transport::UdsMessagePtr message) override {
    // send full message to conversion
    conversation_handler_.HandleMessage(std::move(message));
  }

 private:
  /**
   * @brief        Store the reference to doip udp channel
   */
  channel::udp_channel::DoipUdpChannel doip_udp_channel_;
};

ConnectionManager::ConnectionManager() noexcept : io_context_{} {}

std::unique_ptr<uds_transport::Connection> ConnectionManager::CreateTcpConnection(
    uds_transport::ConversionHandler const &conversation, std::string_view tcp_ip_address, std::uint16_t port_num) {
  return std::make_unique<DoipTcpConnection>(conversation, tcp_ip_address, port_num);
}

std::unique_ptr<uds_transport::Connection> ConnectionManager::CreateUdpConnection(
    uds_transport::ConversionHandler const &conversation, std::string_view udp_ip_address, std::uint16_t port_num) {
  return std::make_unique<DoipUdpConnection>(conversation, udp_ip_address, port_num);
}

}  // namespace connection
}  // namespace doip_client
