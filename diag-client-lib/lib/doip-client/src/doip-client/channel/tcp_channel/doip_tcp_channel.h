/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_H_
#define DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_H_

#include <memory>
#include <utility>

#include "channel_handler/tcp_channel_handler.h"
#include "doip-client/channel/tcp_channel/channel_handler/tcp_channel_handler.h"
#include "doip-client/sockets/socket_handler.h"
#include "doip-client/sockets/tcp_socket_handler.h"
#include "uds_transport-layer-api/connection.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

/**
 * @brief       Class to manage a tcp channel as per DoIP protocol
 */
class DoipTcpChannel final {
 public:
  /**
   * @brief  Type alias for Tcp socket handler
   */
  using TcpSocketHandler = sockets::TcpSocketHandler;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = sockets::TcpSocketHandler::MessagePtr;

 public:
  /**
   * @brief         Constructs an instance of TcpChannel
   * @param[in]     tcp_socket_handler
   *                The tcp socket handler
   * @param[in]     connection
   *                The reference to tcp transport handler
   */
  DoipTcpChannel(TcpSocketHandler tcp_socket_handler, uds_transport::Connection &connection);

  /**
   * @brief         Destruct an instance of TcpChannel
   */
  ~DoipTcpChannel() = default;

  /**
   * @brief        Function to start the channel
   */
  void Start();

  /**
   * @brief        Function to stop the channel
   */
  void Stop();

  /**
   * @brief        Function to check if connected to host remote server
   * @return       True if connection, False otherwise
   */
  bool IsConnectedToHost() const;

  /**
   * @brief       Function to establish connection to remote host server
   * @param[in]   message
   *              The connection message
   * @return      Connection result
   */
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(
      uds_transport::UdsMessageConstPtr message);

  /**
   * @brief       Function to disconnect from remote host server
   * @return      Disconnection result
   */
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost();

  /**
   * @brief       Function to transmit a valid Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request.
   */
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      uds_transport::UdsMessageConstPtr message);

  /**
   * @brief       Function to process the received Tcp message from socket layer
   * @param[in]   tcp_rx_message
   *              The Tcp message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the channel here
   */
  void ProcessReceivedTcpMessage(TcpMessagePtr tcp_rx_message);

 private:
  /**
   * @brief  Store the tcp socket handler
   */
  TcpSocketHandler tcp_socket_handler_;

  /**
   * @brief  Store the doip channel handler
   */
  channel_handler::TcpChannelHandler tcp_channel_handler_;
};

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  // DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_H_
