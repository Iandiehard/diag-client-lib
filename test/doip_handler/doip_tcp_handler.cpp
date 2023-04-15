/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "doip_handler/doip_tcp_handler.h"

namespace ara {
namespace diag {
namespace doip {

DoipTcpHandler::DoipTcpHandler(std::string_view local_udp_address, std::uint16_t udp_port_num) {

}

DoipTcpHandler::~DoipTcpHandler() = default;

DoipTcpHandler::DoipChannel DoipTcpHandler::CreateDoipChannel(
    std::uint16_t logical_address, DoipTcpHandler::DoipChannelReadCallback &&doip_channel_read_cbk) {
  // create new doip channel
  return DoipTcpHandler::DoipChannel{logical_address, *tcp_socket_handler_};
}

DoipTcpHandler::DoipChannel::DoipChannel(std::uint16_t logical_address,
                                         tcpSocket::DoipTcpSocketHandler &tcp_socket_handler)
  : logical_address_{logical_address},
    tcp_socket_handler_{tcp_socket_handler},
    tcp_connection_{std::move(
              tcp_socket_handler_.CreateTcpConnection(
              [](TcpMessagePtr tcp_rx_message) {
                // handle message
              }
              ))} {}

}  // namespace doip
}  // namespace diag
}  // namespace ara
