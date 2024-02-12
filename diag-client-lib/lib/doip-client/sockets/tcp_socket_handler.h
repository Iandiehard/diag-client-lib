/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_

#include <string_view>

#include "boost-support/client/tcp/tcp_client.h"
#include "boost-support/client/tcp/tcp_message.h"
#include "core/include/result.h"

namespace doip_client {
namespace sockets {

/**
 * @brief  Class used to create a tcp socket for handling transmission and reception of tcp message from driver
 */
template<typename TcpClientType>
class TcpSocketHandlerImpl final {
 public:
  /**
   * @brief  Type alias for tcp client
   */
  using TcpClient = TcpClientType;

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = boost_support::client::tcp::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = boost_support::client::tcp::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = boost_support::client::tcp::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(TcpMessagePtr)>;

  /**
   * @brief         Constructs an instance of TcpSocketHandler
   * @param[in]     socket
   *                The socket used
   */
  explicit TcpSocketHandlerImpl(TcpClient tcp_client) noexcept : tcp_client_{std::move(tcp_client)} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TcpSocketHandlerImpl(const TcpSocketHandlerImpl &other) noexcept = delete;
  TcpSocketHandlerImpl &operator=(const TcpSocketHandlerImpl &other) noexcept = delete;

  /**
   * @brief  Move assignment and Move constructor
   */
  TcpSocketHandlerImpl(TcpSocketHandlerImpl &&other) noexcept = default;
  TcpSocketHandlerImpl &operator=(TcpSocketHandlerImpl &&other) noexcept = default;

  /**
   * @brief         Destruct an instance of TcpSocketHandler
   */
  ~TcpSocketHandlerImpl() = default;

  /**
   * @brief        Function to start the socket handler
   */
  void Initialize() noexcept { tcp_client_.Initialize(); }

  /**
   * @brief        Function to stop the socket handler
   */
  void DeInitialize() noexcept { tcp_client_.DeInitialize(); }

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) { tcp_client_.SetReadHandler(std::move(read_handler)); }

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
    return tcp_client_.ConnectToHost(host_ip_address, host_port_num);
  }

  /**
   * @brief         Function to disconnect from remote host if already connected
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> DisconnectFromHost() { return tcp_client_.DisconnectFromHost(); }

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     message
   *                The message to be sent
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(TcpMessageConstPtr message) { return tcp_client_.Transmit(std::move(message)); }

 private:
  /**
   * @brief  Store the client object
   */
  TcpClientType tcp_client_;

  /**
   * @brief  Store the handler
   */
  HandlerRead handler_read_;
};

/**
 * @brief  Type alias of socket handler with tcp client from boost support
 */
using TcpSocketHandler = TcpSocketHandlerImpl<boost_support::client::tcp::TcpClient>;

}  // namespace sockets
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
