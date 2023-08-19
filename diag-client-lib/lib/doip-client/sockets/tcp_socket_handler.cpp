/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sockets/tcp_socket_handler.h"

#include "channel/tcp_channel.h"
#include "common/logger.h"

namespace doip_client {
namespace tcpSocket {
/*
 @ Class Name        : TcpSocketHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */
//ctor
TcpSocketHandler::TcpSocketHandler(std::string_view local_ip_address, tcpChannel::TcpChannel &channel)
    : local_ip_address_{local_ip_address},
      local_port_num_{0U},
      channel_{channel},
      tcp_socket_{} {}

void TcpSocketHandler::Start() {
  tcp_socket_.emplace(local_ip_address_, local_port_num_, [this](TcpMessagePtr tcp_message) {
    channel_.HandleMessage(std::move(tcp_message));
  });
}

void TcpSocketHandler::Stop() {
  tcp_socket_.reset();
}

core_type::Result<void> TcpSocketHandler::ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
  core_type::Result<void> result{core_type::Result<void>::FromError()};
  if (tcp_socket_->Open()) { ret_val = tcp_socket_->ConnectToHost(host_ip_address, host_port_num); }
  return ret_val;
}

core_type::Result<void> TcpSocketHandler::DisconnectFromHost() {
  bool ret_val{false};
  if (tcp_socket_.DisconnectFromHost()) { ret_val = tcp_socket_.Destroy(); }
  return ret_val;
}

core_type::Result<void> TcpSocketHandler::Transmit(TcpMessageConstPtr tcp_message) {
  return (tcp_socket_.Transmit(std::move(tcp_message)));
}
}  // namespace tcpSocket
}  // namespace doip_client
