/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TCP_TCP_SERVER_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TCP_TCP_SERVER_H_

#include <functional>
#include <memory>

#include "boost-support/message/tcp/tcp_message.h"
#include "core/include/result.h"

namespace boost_support {
namespace socket {
namespace tcp {
class TcpSocket;
}  // namespace tcp
}  // namespace socket

namespace server {
namespace tcp {

/**
 * @brief    Server that manages unsecured/ secured tcp connection
 */
class TcpServer final {
 public:
  /**
   * @brief  Type alias for tcp unsecured socket
   */
  using TcpSocket = socket::tcp::TcpSocket;

  /**
   * @brief  Type alias for Tcp message
   */
  using Message = boost_support::message::tcp::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using MessagePtr = boost_support::message::tcp::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using MessageConstPtr = boost_support::message::tcp::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(MessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TcpServer
   * @param[in]     tcp_socket
   *                The underlying tcp socket required for communication
   */
  explicit TcpServer(TcpSocket tcp_socket) noexcept;

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TcpServer(const TcpServer &other) noexcept = delete;
  TcpServer &operator=(const TcpServer &other) noexcept = delete;

  /**
   * @brief         Move assignment and move constructor
   */
  TcpServer(TcpServer &&other) noexcept;
  TcpServer &operator=(TcpServer &&other) noexcept;

  /**
   * @brief         Destruct an instance of TcpServer
   */
  ~TcpServer() noexcept;

  /**
   * @brief         Initialize the server
   */
  void Initialize() noexcept;

  /**
   * @brief         De-initialize the server
   */
  void DeInitialize() noexcept;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) noexcept;

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     tcp_message
   *                The tcp message
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(MessageConstPtr tcp_message);

 private:
  /**
   * @brief    Forward declaration of tcp server implementation
   */
  class TcpServerImpl;

  /**
   * @brief    Unique pointer to tcp server implementation
   */
  std::unique_ptr<TcpServerImpl> tcp_server_impl_;
};

}  // namespace tcp
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TCP_TCP_SERVER_H_
