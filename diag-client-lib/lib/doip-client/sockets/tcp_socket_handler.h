/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_

#include <atomic>
#include <optional>
#include <string>
#include <string_view>

#include "core/include/result.h"
#include "socket/tcp/tcp_client.h"

namespace doip_client {
// forward declaration
namespace channel {
namespace tcp_channel {
class DoipTcpChannel;
}  // namespace tcp_channel
}  // namespace channel

namespace sockets {

/**
 * @brief  Class used to create a tcp socket for handling transmission and reception of tcp message from driver
 */
class TcpSocketHandler final {
 public:
  /**
   * @brief  Definitions of different socket state
   */
  enum class SocketHandlerState : std::uint8_t {
    kSocketOffline = 0U,     /**< Socket offline state */
    kSocketOnline = 1U,      /**< Socket online state */
    kSocketConnected = 2U,   /**< Socket connected to remote server */
    kSocketDisconnected = 4U /**< Socket disconnected from remote server */
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
   * @brief  Type alias for Tcp message
   */
  using TcpChannel = channel::tcp_channel::DoipTcpChannel;

  /**
   * @brief         Constructs an instance of TcpSocketHandler
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     channel
   *                The reference to tcp transport handler
   */
  TcpSocketHandler(std::string_view local_ip_address, TcpChannel &channel);

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

  /**
   * @brief         Function to disconnect from remote host if already connected
   * @return        The
   */
  core_type::Result<void> DisconnectFromHost();

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     tcp_message
   *                The tcp message
   * @return        The
   */
  core_type::Result<void> Transmit(TcpMessageConstPtr tcp_message);

  /**
   * @brief         Function to get the current state of socket handler
   * @return        The socket handler state
   */
  SocketHandlerState GetSocketHandlerState() const;

 private:
  /**
   * @brief  Type alias for tcp client socket
   */
  using TcpSocket = boost_support::socket::tcp::TcpClientSocket;

  /**
   * @brief  Store the local ip address
   */
  std::string local_ip_address_;

  /**
   * @brief  Store the local port number
   */
  std::uint16_t local_port_num_;

  /**
   * @brief  Store the socket object
   */
  std::optional<TcpSocket> tcp_socket_;

  /**
   * @brief  Store the reference to tcp channel
   */
  TcpChannel &channel_;

  /**
   * @brief  Store the state of handler
   */
  std::atomic<SocketHandlerState> state_;
};
}  // namespace sockets
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
