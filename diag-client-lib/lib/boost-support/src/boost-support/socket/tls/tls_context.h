/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_CONTEXT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_CONTEXT_H_

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "boost-support/client/tls/tls_version.h"
#include "boost-support/server/tls/tls_version.h"

namespace boost_support {
namespace socket {
namespace tls {

/**
 * @brief       Tls context class responsible for setting cipher suite and loading certificates
 */
class TlsContext final {
 public:
  /**
   * @brief  Type alias for Tls client with version 1.2
   */
  using Tls12VersionClient = client::tls::TlsVersion12;

  /**
   * @brief  Type alias for Tls client with version 1.3
   */
  using Tls13VersionClient = client::tls::TlsVersion13;

  /**
   * @brief  Type alias for Tls server with version 1.2
   */
  using Tls12VersionServer = server::tls::TlsVersion12;

  /**
   * @brief  Type alias for Tls server with version 1.3
   */
  using Tls13VersionServer = server::tls::TlsVersion13;

  /**
   * @brief  Type alias for boost ssl context
   */
  using SslContext = boost::asio::ssl::context;

 public:
  /**
   * @brief         Constructs an instance of TlsContext
   * @param[in]     client
   *                The Tls 1.2 version client
   * @param[in]     ca_certification_path
   *                The path to root CA certificate
   */
  TlsContext(Tls12VersionClient client, std::string_view ca_certification_path) noexcept;

  /**
   * @brief         Constructs an instance of TlsContext
   * @param[in]     client
   *                The Tls 1.2 version client
   * @param[in]     ca_certification_path
   *                The path to root CA certificate
   */
  TlsContext(Tls13VersionClient client, std::string_view ca_certification_path) noexcept;

  /**
   * @brief         Constructs an instance of TlsContext
   * @param[in]     server
   *                The Tls 1.2 version server
   * @param[in]     certificate_path
   *                The path to root CA certificate
   * @param[in]     private_key_path
   *                The path to private key
   */
  TlsContext(Tls12VersionServer server, std::string_view certificate_path,
             std::string_view private_key_path) noexcept;

  /**
   * @brief         Constructs an instance of TlsContext
   * @param[in]     server
   *                The Tls 1.2 version server
   * @param[in]     certificate_path
   *                The path to root CA certificate
   * @param[in]     private_key_path
   *                The path to private key
   */
  TlsContext(Tls13VersionServer server, std::string_view certificate_path,
             std::string_view private_key_path) noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TlsContext(const TlsContext &other) noexcept = delete;
  TlsContext &operator=(const TlsContext &other) noexcept = delete;

  /**
   * @brief  Defaulted move assignment and move constructor
   */
  TlsContext(TlsContext &&other) noexcept = default;
  TlsContext &operator=(TlsContext &&other) noexcept = default;

  /**
   * @brief         Destruct an instance of TcpSocket
   */
  ~TlsContext() noexcept = default;

  /**
   * @brief         Function to get the ssl context reference
   * @return        The reference to ssl context
   */
  SslContext &GetContext() noexcept { return ssl_context_; }

 private:
  /**
   * @brief  Store the boost ssl context
   */
  SslContext ssl_context_;
};
}  // namespace tls
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_CONTEXT_H_
