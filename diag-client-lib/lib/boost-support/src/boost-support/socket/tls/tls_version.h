/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_VERSION_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_VERSION_H_

#include <cstdint>

namespace boost_support {
namespace socket {
namespace tls {

/**
 * @brief         The TLS version to be used by this socket
 */
enum class TlsVersionType : std::uint8_t {
  kTls12,  // Tls version 1.2
  kTls13   // Tls version 1.3
};

/**
 * @brief         The TLS 1.2 cipher suites supported
 */
enum class Tls12CipherSuites : std::uint16_t {
  TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA25 = 49195,
  TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 = 49196,
  TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 = 52393,
  TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 = 49187,
  TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 = 49188
};

}  // namespace tls
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_VERSION_H_
