/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TCP_TCP_CLIENT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TCP_TCP_CLIENT_H_

#include "boost-support/connection/tcp/tcp_connection.h"
#include "boost-support/socket/tcp/tcp_socket.h"
#include "boost-support/socket/tls/tls_socket.h"

namespace boost_support {
namespace client {
namespace tcp {

using TcpClientUnsecure =
    connection::tcp::TcpConnection<connection::tcp::ConnectionType::kClient, socket::tcp::TcpSocket>;

using TcpClientSecured =
    connection::tcp::TcpConnection<connection::tcp::ConnectionType::kServer, socket::tcp::TcpSocket>;

}  // namespace tcp
}  // namespace client
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TCP_TCP_CLIENT_H_
