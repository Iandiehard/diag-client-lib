/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H

#include <string>
#include <string_view>
#include <optional>

#include "common/common_doip_types.h"
#include "socket/tcp/tcp_client.h"
#include "core/include/result.h"

namespace doip_client {
// forward declaration
namespace tcpChannel {
class TcpChannel;
}

namespace tcpSocket {

/*
 @ Class Name        : tcp_SocketHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */
class TcpSocketHandler final {
 public:
  /**
   * @brief  Definitions of different socket state
   */
  enum class TcpSocketState : std::uint8_t {
    kSocketOffline = 0U,         /**< Socket offline state */
    kSocketOnline = 1U,          /**< Socket online state */
    kSocketConnected = 2U,       /**< Socket connected to remote server */
    kSocketDisconnected = 4U     /**< Socket disconnected from remote server */
  };

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = boost_support::socket::tcp::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = boost_support::socket::tcp::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = boost_support::socket::tcp::TcpMessageConstPtr;

  /**
   * @brief         Constructs an instance of TcpSocketHandler
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     channel
   *                The reference to tcp transport handler
   */
  TcpSocketHandler(std::string_view local_ip_address, tcpChannel::TcpChannel &channel);

  /**
   * @brief         Destruct an instance of TcpSocketHandler
   */
  ~TcpSocketHandler() = default;

  /**
   * @brief        Function to start the socket handler
   */
  void Start();

  /**
   * @brief        Function to stop the socket handler
   */
  void Stop();

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        The
   */
  core_type::Result<void> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num);

  // Disconnect from host
  core_type::Result<void> DisconnectFromHost();

  // Transmit function
  core_type::Result<void> Transmit(TcpMessageConstPtr tcp_message);

 private:
  /**
   * @brief  Type alias for tcp client socket
   */
  using TcpSocket = boost_support::socket::tcp::TcpClientSocket;

  // local Ip address
  std::string local_ip_address_;
  // local port number
  std::uint16_t local_port_num_;
  // tcp socket
  std::optional<TcpSocket> tcp_socket_;
  // store tcp channel reference
  tcpChannel::TcpChannel &channel_;
};
}  // namespace tcpSocket
}  // namespace doip_client
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H
