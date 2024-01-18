/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_CONTEXT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_CONTEXT_H_

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "socket/tls/tls_version.h"

namespace boost_support {
namespace socket {
namespace tls {

/**
 * @brief       Class used to create a tcp socket for handling transmission and reception of tcp message from driver
 */
template<TlsVersionType TlsVersion>
class TlsContext {
 public:
  /**
   * @brief  Type alias for boost ssl context
   */
  using Context = boost::asio::ssl::context;

 public:
  /**
   * @brief         Constructs an instance of TlsContext
   * @param[in]     ca_certification_path
   *                The path to root CA certificate
   * @param[in]     io_context
   *                The I/O context required to create socket
   */
  template<TlsVersionType T = TlsVersion, std::enable_if_t<(T == TlsVersionType::kTls12), bool> = true>
  TlsContext(std::string_view ca_certification_path, boost::asio::io_context &io_context) noexcept
      : context_{boost::asio::ssl::context::tlsv12_client} {
    // Load the root CA certificates
    context_.load_verify_file(std::string{ca_certification_path});
  }

  template<TlsVersionType T = TlsVersion, std::enable_if_t<(T == TlsVersionType::kTls13), bool> = true>
  TlsContext(std::string_view ca_certification_path, boost::asio::io_context &io_context) noexcept
      : context_{boost::asio::ssl::context::tlsv13_client} {
    // Load the root CA certificates
    context_.load_verify_file(std::string{ca_certification_path});
  }

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TlsContext(const TlsContext &other) noexcept = delete;
  TlsContext &operator=(const TlsContext &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  TlsContext(TlsContext &&other) noexcept = delete;
  TlsContext &operator=(TlsContext &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of TcpSocket
   */
  ~TlsContext() noexcept = default;

  /**
   * @brief         Function to get the ssl context reference
   * @return        The reference to ssl context
   */
  Context const &GetContext() noexcept { return context_; }

 private:
  /**
   * @brief  Store the boost ssl context
   */
  Context context_;
};
}  // namespace tls
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_CONTEXT_H_
