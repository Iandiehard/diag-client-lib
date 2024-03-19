/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TCP_ACCEPTOR_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TCP_ACCEPTOR_H_

#include "boost-support/server/tcp/tcp_server.h"

namespace boost_support {
namespace server {
namespace tcp {

/**
 * @brief    The acceptor to create new tcp servers
 */
class TcpAcceptor final {
 public:
  /**
   * @brief         Constructs an instance of Acceptor
   * @details       Tcp connection shall be accepted on this ip address and port
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     maximum_connection
   *                The maximum number of accepted connection allowed
   */
  TcpAcceptor(std::string_view local_ip_address, std::uint16_t local_port_num,
              std::uint8_t maximum_connection) noexcept;

  /**
   * @brief         Destruct an instance of TcpAcceptor
   */
  ~TcpAcceptor() noexcept;

  /**
   * @brief         Get a tcp server ready to communicate
   * @details       This blocks until new server is created
   * @return        Tcp server object on success, else nothing
   */
  std::optional<TcpServer> GetTcpServer() noexcept;

 private:
  /**
   * @brief    Forward declaration of tcp acceptor implementation
   */
  class TcpAcceptorImpl;

  /**
   * @brief    Unique pointer to tcp acceptor implementation
   */
  std::unique_ptr<TcpAcceptorImpl> tcp_acceptor_impl_;
};
}  // namespace tcp
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TCP_ACCEPTOR_H_
