/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_ACCEPTOR_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_ACCEPTOR_H_

#include "boost-support/server/tls/tls_server.h"

namespace boost_support {
namespace server {
namespace tls {

/**
 * @brief    The acceptor to create new tcp servers
 */
class TlsAcceptor final {
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
  TlsAcceptor(std::string_view local_ip_address, std::uint16_t local_port_num,
              std::uint8_t maximum_connection) noexcept;

  /**
   * @brief         Destruct an instance of TcpAcceptor
   */
  ~TlsAcceptor() noexcept;

  /**
   * @brief         Get a tls server ready to communicate
   * @details       This blocks until new server is created
   * @return        Tls server object on success, else nothing
   */
  std::optional<TlsServer> GetTlsServer() noexcept;

 private:
  /**
   * @brief    Forward declaration of tls acceptor implementation
   */
  class TlsAcceptorImpl;

  /**
   * @brief    Unique pointer to tls acceptor implementation
   */
  std::unique_ptr<TlsAcceptorImpl> tls_acceptor_impl_;
};
}  // namespace tls
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_ACCEPTOR_H_
