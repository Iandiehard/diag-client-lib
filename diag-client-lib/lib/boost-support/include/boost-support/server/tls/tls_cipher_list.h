/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TLS_TLS_CIPHER_LIST_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TLS_TLS_CIPHER_LIST_H_

#include <cstdint>

namespace boost_support {
namespace server {
namespace tls {

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

/**
 * @brief         The TLS 1.3 cipher suites supported
 */
enum class Tls13CipherSuites : std::uint16_t {
  TLS_AES_128_GCM_SHA256 = 4865,
  TLS_AES_256_GCM_SHA384 = 4866,
  TLS_CHACHA20_POLY1305_SHA256 = 4867,
  TLS_AES_128_CCM_SHA256 = 4868,
  TLS_AES_128_CCM_8_SHA256 = 4869
};

}  // namespace tls
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TLS_TLS_CIPHER_LIST_H_
