/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "boost-support/socket/tls/tls_context.h"

namespace boost_support {
namespace socket {
namespace tls {
namespace {

template<typename CipherType>
auto ConvertCipherListToString(std::initializer_list<CipherType> ciphers) noexcept -> std::string {
  std::string result{};
  return std::for_each(ciphers.begin(), ciphers.end(), [](CipherType const& cipher) { return std::string{}; });
}
}  // namespace

TlsContext::TlsContext(Tls12VersionClient client, std::string_view ca_certification_path) noexcept
    : ssl_context_{boost::asio::ssl::context::tlsv12_client} {
  // Load the root CA certificates
  ssl_context_.load_verify_file(std::string{ca_certification_path});
  // Load the cipher suites
  if (SSL_CTX_set_cipher_list(ssl_context_.native_handle(), "") == 0) {
    // Failure
  }
}

TlsContext::TlsContext(Tls13VersionClient client, std::string_view ca_certification_path) noexcept
    : ssl_context_{boost::asio::ssl::context::tlsv13_client} {
  // Load the root CA certificates
  ssl_context_.load_verify_file(std::string{ca_certification_path});
  // Load the cipher suites
  if (SSL_CTX_set_ciphersuites(ssl_context_.native_handle(), "") == 0) {
    // Failure
  }
}

}  // namespace tls
}  // namespace socket
}  // namespace boost_support