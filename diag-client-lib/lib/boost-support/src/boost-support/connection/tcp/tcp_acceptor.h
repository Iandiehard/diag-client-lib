/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_TCP_TCP_ACCEPTOR_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_TCP_TCP_ACCEPTOR_H_

// includes
#include <optional>
#include <boost/asio.hpp>

#include "boost-support/connection/tcp/tcp_connection.h"
#include "boost-support/socket/tcp/tcp_socket.h"

namespace boost_support {
namespace connection {
namespace tcp {

class TcpAcceptor final {
 public:
  /**
   * @brief  Type alias for tcp unsecured socket
   */
  using TcpSocket = socket::tcp::TcpSocket;

  /**
   * @brief  Type alias for tcp server connection
   */
  using Connection = TcpConnection<ConnectionType::kServer, TcpSocket>;

  /**
   * @brief  Type alias for read handler, invoked on reception of tcp message
   */
  using HandlerRead = Connection::HandlerRead;

  /**
   * @brief         Constructs an instance of TcpAcceptor
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     io_context
   *                The I/O context from which connection is created
   */
  TcpAcceptor(std::string_view local_ip_address, std::uint16_t local_port_num,
              boost::asio::io_context io_context) noexcept;

  /**
   * @brief         Destruct an instance of TcpAcceptor
   */
  ~TcpAcceptor() noexcept = default;

  /**
   * @brief         Get the tcp connection to communicate
   * @details       This blocks until new connection is accepted
   * @param[in]     read_handler
   *                The handler invoked on reception of tcp message
   * @return        Tcp connection object on success, else nothing
   */
  std::optional<Connection> GetConnection(HandlerRead read_handler) noexcept;

 private:
  /**
   * @brief  Type alias for tcp acceptor
   */
  using Acceptor = boost::asio::ip::tcp::acceptor;

  /**
   * @brief  Store the tcp acceptor
   */
  Acceptor acceptor_;
};
}  // namespace tcp
}  // namespace connection
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_TCP_TCP_ACCEPTOR_H_
