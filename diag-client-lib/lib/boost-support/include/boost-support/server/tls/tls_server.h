/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_SERVER_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_SERVER_H_

#include <functional>
#include <memory>

#include "boost-support/message/tcp/tcp_message.h"
#include "core/include/result.h"

namespace boost_support {
namespace socket {
namespace tls {
class TlsSocket;
}  // namespace tls
}  // namespace socket

namespace server {
namespace tls {

/**
 * @brief    Server that manages unsecured/ secured tcp connection
 */
class TlsServer final {
 public:
  /**
   * @brief  Type alias for tcp unsecured socket
   */
  using TlsSocket = socket::tls::TlsSocket;

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
   * @brief         Constructs an instance of TlsServer
   * @param[in]     tls_socket
   *                The underlying tls socket required for communication
   */
  explicit TlsServer(TlsSocket tls_socket) noexcept;

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TlsServer(const TlsServer &other) noexcept = delete;
  TlsServer &operator=(const TlsServer &other) noexcept = delete;

  /**
   * @brief         Move assignment and move constructor
   */
  TlsServer(TlsServer &&other) noexcept;
  TlsServer &operator=(TlsServer &&other) noexcept;

  /**
   * @brief         Destruct an instance of TcpServer
   */
  ~TlsServer() noexcept;

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
  class TlsServerImpl;

  /**
   * @brief    Unique pointer to tcp server implementation
   */
  std::unique_ptr<TlsServerImpl> tls_server_impl_;
};

}  // namespace tls
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_SERVER_H_
