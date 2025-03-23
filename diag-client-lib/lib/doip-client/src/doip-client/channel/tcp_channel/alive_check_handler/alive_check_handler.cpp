/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/channel/tcp_channel/alive_check_handler/alive_check_handler.h"

#include "utility-support/thread_pool/thread_pool.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace alive_check_handler {

AliveCheckHandler::AliveCheckHandler(sockets::TcpSocketHandler& tcp_socket_handler,
                                     uds_transport::Connection& connection)
    : tcp_socket_handler_{tcp_socket_handler},
      connection_{connection} {}

void AliveCheckHandler::ProcessAliveCheckRequest(DoipMessage const& doip_payload) const noexcept {
  // ask connection handler about the connection status
  if (connection_.IsConversationAlive()) {
    // send the alive check response
  }
}

}  // namespace alive_check_handler
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
