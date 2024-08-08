/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/server/tls/tls_server.h"

#include "boost-support/connection/tcp/tcp_connection.h"
#include "boost-support/socket/tls/tls_socket.h"

namespace boost_support {
namespace server {
namespace tls {

class TlsServer::TlsServerImpl final {
 public:
  /**
   * @brief     Type alias for secured socket
   */
  using TlsSocket = socket::tls::TlsSocket;

  /**
   * @brief     Type alias for secured tcp connection
   */
  using TcpConnectionSecured =
      connection::tcp::TcpConnection<connection::tcp::ConnectionType::kServer, TlsSocket>;

  /**
   * @brief         Constructs an instance of TcpServerImpl
   * @param[in]     tcp_socket
   *                The underlying tcp socket required for communication
   */
  explicit TlsServerImpl(TlsSocket tcp_socket) noexcept : tcp_connection_{std::move(tcp_socket)} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TlsServerImpl(const TlsServerImpl &other) noexcept = delete;
  TlsServerImpl &operator=(const TlsServerImpl &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  TlsServerImpl(TlsServerImpl &&other) noexcept = delete;
  TlsServerImpl &operator=(TlsServerImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of TcpServerImpl
   */
  ~TlsServerImpl() noexcept = default;

  /**
   * @brief         Initialize the TcpServerImpl
   */
  void Initialize() noexcept { tcp_connection_.Initialize(); }

  /**
   * @brief         De-initialize the TcpServerImpl
   */
  void DeInitialize() noexcept { tcp_connection_.DeInitialize(); }

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) noexcept {
    tcp_connection_.SetReadHandler(std::move(read_handler));
  }

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     tcp_message
   *                The tcp message
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(MessageConstPtr tcp_message) {
    return tcp_connection_.Transmit(std::move(tcp_message));
  }

 private:
  /**
   * @brief      Store the tcp connection
   */
  TcpConnectionSecured tcp_connection_;
};

TlsServer::TlsServer(TlsServer::TlsSocket tls_socket) noexcept
    : tls_server_impl_{std::make_unique<TlsServerImpl>(std::move(tls_socket))} {}

TlsServer::TlsServer(TlsServer &&other) noexcept = default;

TlsServer &TlsServer::operator=(TlsServer &&other) noexcept = default;

TlsServer::~TlsServer() noexcept = default;

void TlsServer::Initialize() noexcept { tls_server_impl_->Initialize(); }

void TlsServer::DeInitialize() noexcept { tls_server_impl_->DeInitialize(); }

void TlsServer::SetReadHandler(TlsServer::HandlerRead read_handler) noexcept {
  tls_server_impl_->SetReadHandler(std::move(read_handler));
}

core_type::Result<void> TlsServer::Transmit(TlsServer::MessageConstPtr tcp_message) {
  return tls_server_impl_->Transmit(std::move(tcp_message));
}
}  // namespace tls
}  // namespace server
}  // namespace boost_support
