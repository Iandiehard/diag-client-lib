/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
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

#include "client/tcp/tcp_client.h"
#include "core/include/result.h"
#include "socket/tcp/tcp_socket.h"

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
   * @brief  Definitions of different connection state
   */
  enum class SocketHandlerState : std::uint8_t {
    kSocketConnected = 0U,   /**< Connected to remote server */
    kSocketDisconnected = 1U /**< Disconnected from remote server */
  };

  /**
   * @brief  Type alias for tcp unsecured socket
   */
  using TcpSocket = boost_support::socket::tcp::TcpSocket;

  /**
   * @brief  Type alias for tcp client with unsecured socket
   */
  using TcpClient = boost_support::client::tcp::TcpClient<TcpSocket>;

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = TcpClient::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = TcpClient::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = TcpClient::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(TcpMessagePtr)>;

  /**
   * @brief         Constructs an instance of TcpSocketHandler
   * @param[in]     socket
   *                The socket used
   */
  explicit TcpSocketHandler(TcpSocket socket);

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TcpSocketHandler(const TcpSocketHandler &other) noexcept = delete;
  TcpSocketHandler &operator=(const TcpSocketHandler &other) noexcept = delete;

  /**
   * @brief  Move assignment and Move constructor
   */
  TcpSocketHandler(TcpSocketHandler &&other) noexcept;
  TcpSocketHandler &operator=(TcpSocketHandler &&other) noexcept;

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
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler);

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num);

  /**
   * @brief         Function to disconnect from remote host if already connected
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> DisconnectFromHost();

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     tcp_message
   *                The tcp message
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(TcpMessageConstPtr tcp_message);

  /**
   * @brief         Function to get the current state of socket handler
   * @return        The socket handler state
   */
  SocketHandlerState GetSocketHandlerState() const;

 private:
  /**
   * @brief  Store the client object
   */
  std::unique_ptr<TcpClient> tcp_client_;

  /**
   * @brief  Store the state of handler
   */
  std::atomic<SocketHandlerState> state_;

  /**
   * @brief  Store the handler
   */
  HandlerRead handler_read_;
};
}  // namespace sockets
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
