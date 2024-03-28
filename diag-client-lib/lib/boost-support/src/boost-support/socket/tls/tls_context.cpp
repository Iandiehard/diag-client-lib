/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "boost-support/socket/tls/tls_context.h"

#include <numeric>

#include "boost-support/client/tls/tls_cipher_list.h"
#include "boost-support/server/tls/tls_cipher_list.h"

namespace boost_support {
namespace socket {
namespace tls {
namespace {

auto ToOpenSslString(client::tls::Tls12CipherSuites cipher) noexcept -> std::string {
  using Tls12CipherSuites = client::tls::Tls12CipherSuites;
  std::string result{};

  switch (cipher) {
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
      result.append("ECDHE-ECDSA-AES128-GCM-SHA256");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384:
      result.append("ECDHE-ECDSA-AES256-GCM-SHA384");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256:
      result.append("ECDHE-ECDSA-CHACHA20-POLY1305");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
      result.append("ECDHE-ECDSA-AES128-SHA256");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384:
      result.append("ECDHE-ECDSA-AES256-SHA384");
      break;
  }

  return result;
}

auto ToOpenSslString(client::tls::Tls13CipherSuites cipher) noexcept -> std::string {
  using Tls13CipherSuites = client::tls::Tls13CipherSuites;
  std::string result{};

  switch (cipher) {
    case Tls13CipherSuites::TLS_AES_128_GCM_SHA256:
      result.append("TLS_AES_128_GCM_SHA256");
      break;
    case Tls13CipherSuites::TLS_AES_256_GCM_SHA384:
      result.append("TLS_AES_256_GCM_SHA384");
      break;
    case Tls13CipherSuites::TLS_CHACHA20_POLY1305_SHA256:
      result.append("TLS_CHACHA20_POLY1305_SHA256");
      break;
    case Tls13CipherSuites::TLS_AES_128_CCM_SHA256:
      result.append("TLS_AES_128_CCM_SHA256");
      break;
    case Tls13CipherSuites::TLS_AES_128_CCM_8_SHA256:
      result.append("TLS_AES_128_CCM_8_SHA256");
      break;
  }
  return result;
}

auto ToOpenSslString(server::tls::Tls12CipherSuites cipher) noexcept -> std::string {
  using Tls12CipherSuites = server::tls::Tls12CipherSuites;
  std::string result{};

  switch (cipher) {
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
      result.append("ECDHE-ECDSA-AES128-GCM-SHA256");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384:
      result.append("ECDHE-ECDSA-AES256-GCM-SHA384");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256:
      result.append("ECDHE-ECDSA-CHACHA20-POLY1305");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
      result.append("ECDHE-ECDSA-AES128-SHA256");
      break;
    case Tls12CipherSuites::TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384:
      result.append("ECDHE-ECDSA-AES256-SHA384");
      break;
  }

  return result;
}

auto ToOpenSslString(server::tls::Tls13CipherSuites cipher) noexcept -> std::string {
  using Tls13CipherSuites = server::tls::Tls13CipherSuites;
  std::string result{};

  switch (cipher) {
    case Tls13CipherSuites::TLS_AES_128_GCM_SHA256:
      result.append("TLS_AES_128_GCM_SHA256");
      break;
    case Tls13CipherSuites::TLS_AES_256_GCM_SHA384:
      result.append("TLS_AES_256_GCM_SHA384");
      break;
    case Tls13CipherSuites::TLS_CHACHA20_POLY1305_SHA256:
      result.append("TLS_CHACHA20_POLY1305_SHA256");
      break;
    case Tls13CipherSuites::TLS_AES_128_CCM_SHA256:
      result.append("TLS_AES_128_CCM_SHA256");
      break;
    case Tls13CipherSuites::TLS_AES_128_CCM_8_SHA256:
      result.append("TLS_AES_128_CCM_8_SHA256");
      break;
  }
  return result;
}

template<typename CipherType>
auto ConvertCipherListToString(std::initializer_list<CipherType> ciphers) noexcept -> std::string {
  return std::accumulate(
      ciphers.begin(), ciphers.end(), std::string{},
      [](std::string const& result, CipherType const& cipher) { return result + ':' + ToOpenSslString(cipher); });
}
}  // namespace

TlsContext::TlsContext(Tls12VersionClient client, std::string_view ca_certification_path) noexcept
    : ssl_context_{boost::asio::ssl::context::tlsv12_client} {
  // Load the root CA certificates
  ssl_context_.load_verify_file(std::string{ca_certification_path});
  // Load the cipher suites
  if (SSL_CTX_set_cipher_list(ssl_context_.native_handle(), ConvertCipherListToString(client.cipher_suites).c_str()) ==
      0) {
    // Failure
  }
}

TlsContext::TlsContext(Tls13VersionClient client, std::string_view ca_certification_path) noexcept
    : ssl_context_{boost::asio::ssl::context::tlsv13_client} {
  // Load the root CA certificates
  ssl_context_.load_verify_file(std::string{ca_certification_path});
  // Load the cipher suites
  if (SSL_CTX_set_ciphersuites(ssl_context_.native_handle(), ConvertCipherListToString(client.cipher_suites).c_str()) ==
      0) {
    // Failure
  }
}

TlsContext::TlsContext(Tls12VersionServer server, std::string_view certificate_path,
                       std::string_view private_key_path) noexcept
    : ssl_context_{boost::asio::ssl::context::tlsv12_server} {
  // Load certificate and private key from provided locations
  ssl_context_.use_certificate_chain_file(std::string{certificate_path});
  ssl_context_.use_private_key_file(std::string{private_key_path}, boost::asio::ssl::context::pem);
  // Load the cipher suites
  if (SSL_CTX_set_ciphersuites(ssl_context_.native_handle(), ConvertCipherListToString(server.cipher_suites).c_str()) ==
      0) {
    // Failure
  }
}

TlsContext::TlsContext(Tls13VersionServer server, std::string_view certificate_path,
                       std::string_view private_key_path) noexcept
    : ssl_context_{boost::asio::ssl::context::tlsv13_server} {
  // Load certificate and private key from provided locations
  ssl_context_.use_certificate_chain_file(std::string{certificate_path});
  ssl_context_.use_private_key_file(std::string{private_key_path}, boost::asio::ssl::context::pem);
  // Load the cipher suites
  if (SSL_CTX_set_ciphersuites(ssl_context_.native_handle(), ConvertCipherListToString(server.cipher_suites).c_str()) ==
      0) {
    // Failure
  }
}
}  // namespace tls
}  // namespace socket
}  // namespace boost_support