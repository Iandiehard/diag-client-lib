/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_VERSION_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_VERSION_H_

#include <cstdint>
#include <initializer_list>

#include "boost-support/server/tls/tls_cipher_list.h"

namespace boost_support {
namespace server {
namespace tls {
namespace detail {
/**
 * @brief    Template type for Tls version
 * @tparam   CipherSuite
 *           The supported cipher suites in corresponding tls version
 */
template<typename CipherSuite>
struct TlsVersion {
  std::initializer_list<CipherSuite> cipher_suites{};
};
}  // namespace detail

/**
 * @brief  Strong type for TLS version 1.2
 */
using TlsVersion12 = detail::TlsVersion<Tls12CipherSuites>;

/**
 * @brief  Strong type for TLS version 1.3
 */
using TlsVersion13 = detail::TlsVersion<Tls13CipherSuites>;

}  // namespace tls
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_TLS_TLS_VERSION_H_
