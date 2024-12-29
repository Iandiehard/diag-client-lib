/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOIP_CLIENT_CONNECTION_UDP_CONNECTION_UDP_CONNECTION_H_
#define DOIP_CLIENT_CONNECTION_UDP_CONNECTION_UDP_CONNECTION_H_

#include "doip-client/channel/udp_channel/doip_udp_channel.h"
#include "doip-client/sockets/udp_socket_handler.h"
#include "uds_transport-layer-api/connection.h"

namespace doip_client {
namespace connection {
namespace udp_connection {

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
  DoipUdpConnection(uds_transport::ConversionHandler const &conversation_handler,
                    std::string_view udp_ip_address, std::uint16_t port_num);

  /**
   * @brief         Destruct an instance of DoipUdpConnection
   */
  ~DoipUdpConnection() final = default;

  /**
   * @brief        Function to initialize the connection
   * @return       The initialization result
   */
  InitializationResult Initialize() override;

  /**
   * @brief        Function to start the connection
   */
  void Start() override;

  /**
   * @brief        Function to stop the connection
   */
  void Stop() override;

  /**
   * @brief        Function to check if connected to host remote server
   * @return       True if connection, False otherwise
   */
  bool IsConnectToHost() override;

  /**
   * @brief       Function to establish connection to remote host server
   * @param[in]   message
   *              The connection message
   * @return      Connection result
   */
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(
      uds_transport::UdsMessageConstPtr) override;

  /**
   * @brief       Function to disconnect from remote host server
   * @return      Disconnection result
   */
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() override;

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
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
  IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                  uds_transport::UdsMessage::Address target_addr,
                  uds_transport::UdsMessage::TargetAddressType type,
                  uds_transport::ChannelID channel_id, std::size_t size,
                  uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                  core_type::Span<std::uint8_t const> payload_info) override;

  /**
   * @brief       Function to transmit a valid Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request.
   */
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      uds_transport::UdsMessageConstPtr message) override;

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(uds_transport::UdsMessagePtr message) override;

 private:
  /**
   * @brief        Store the reference to doip udp channel
   */
  channel::udp_channel::DoipUdpChannel doip_udp_channel_;
};

}  // namespace udp_connection
}  // namespace connection
}  // namespace doip_client
#endif  // DOIP_CLIENT_CONNECTION_UDP_CONNECTION_UDP_CONNECTION_H_
