/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/server/tcp/tcp_server.h"

#include "boost-support/connection/tcp/tcp_connection.h"
#include "boost-support/socket/tcp/tcp_socket.h"

namespace boost_support {
namespace server {
namespace tcp {

class TcpServer::TcpServerImpl final {
 public:
  /**
   * @brief  Type alias for tcp server connection
   */
  using TcpConnection =
      connection::tcp::TcpConnection<connection::tcp::ConnectionType::kServer, TcpSocket>;

  /**
   * @brief         Constructs an instance of TcpServerImpl
   * @param[in]     tcp_socket
   *                The underlying tcp socket required for communication
   */
  explicit TcpServerImpl(TcpSocket tcp_socket) noexcept : tcp_connection_{std::move(tcp_socket)} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TcpServerImpl(const TcpServerImpl &other) noexcept = delete;
  TcpServerImpl &operator=(const TcpServerImpl &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  TcpServerImpl(TcpServerImpl &&other) noexcept = delete;
  TcpServerImpl &operator=(TcpServerImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of TcpServerImpl
   */
  ~TcpServerImpl() noexcept = default;

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
  TcpConnection tcp_connection_;
};

TcpServer::TcpServer(TcpServer::TcpSocket tcp_socket) noexcept
    : tcp_server_impl_{std::make_unique<TcpServerImpl>(std::move(tcp_socket))} {}

TcpServer::TcpServer(TcpServer &&other) noexcept = default;

TcpServer &TcpServer::operator=(TcpServer &&other) noexcept = default;

TcpServer::~TcpServer() noexcept = default;

void TcpServer::Initialize() noexcept { tcp_server_impl_->Initialize(); }

void TcpServer::DeInitialize() noexcept { tcp_server_impl_->DeInitialize(); }

void TcpServer::SetReadHandler(TcpServer::HandlerRead read_handler) noexcept {
  tcp_server_impl_->SetReadHandler(std::move(read_handler));
}

core_type::Result<void> TcpServer::Transmit(TcpServer::MessageConstPtr tcp_message) {
  return tcp_server_impl_->Transmit(std::move(tcp_message));
}
}  // namespace tcp
}  // namespace server
}  // namespace boost_support
