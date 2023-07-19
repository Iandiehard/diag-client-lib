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
TcpSocketHandler::TcpSocketHandler(std::string_view localIpaddress, tcpChannel::tcpChannel &channel)
    : local_ip_address_{localIpaddress},
      local_port_num_{0U},
      channel_{channel} {
  //create socket
  tcpSocket_ = std::make_unique<TcpSocket>(local_ip_address_, local_port_num_, [&](TcpMessagePtr tcp_message) {
    channel_.HandleMessage(std::move(tcp_message));
  });
}

void TcpSocketHandler::Start() {}

void TcpSocketHandler::Stop() {}

// Connect to host
bool TcpSocketHandler::ConnectToHost(std::string_view host_ip_address, uint16_t host_port_num) {
  bool ret_val{false};
  if (tcpSocket_->Open()) { ret_val = tcpSocket_->ConnectToHost(host_ip_address, host_port_num); }
  return ret_val;
}

// Disconnect from host
bool TcpSocketHandler::DisconnectFromHost() {
  bool ret_val = false;
  if (tcpSocket_->DisconnectFromHost()) { ret_val = tcpSocket_->Destroy(); }
  return ret_val;
}

// Function to trigger transmission
bool TcpSocketHandler::Transmit(TcpMessageConstPtr tcp_message) {
  return (tcpSocket_->Transmit(std::move(tcp_message)));
}
}  // namespace tcpSocket
}  // namespace doip_client
