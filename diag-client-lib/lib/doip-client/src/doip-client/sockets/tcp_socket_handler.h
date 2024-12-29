/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
#define DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_

#include "boost-support/client/tcp/tcp_client.h"
#include "doip-client/sockets/socket_handler.h"

namespace doip_client {
namespace sockets {

/**
 * @brief  Type alias of Tcp client socket handler
 */
using TcpSocketHandler = SocketHandler<boost_support::client::tcp::TcpClient>;

}  // namespace sockets
}  // namespace doip_client
#endif  // DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H_
