/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H

#include <string>
#include <string_view>

#include "common/common_doip_types.h"
#include "socket/tcp/tcp_client.h"

namespace doip_client {
// forward declaration
namespace tcpChannel {
class tcpChannel;
}

namespace tcpSocket {
// typedefs
using TcpSocket = boost_support::socket::tcp::CreateTcpClientSocket;
using TcpMessage = boost_support::socket::tcp::TcpMessage;
using TcpMessagePtr = boost_support::socket::tcp::TcpMessagePtr;
using TcpMessageConstPtr = boost_support::socket::tcp::TcpMessageConstPtr;

/*
 @ Class Name        : tcp_SocketHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */
class TcpSocketHandler final {
public:
  // ctor
  TcpSocketHandler(std::string_view localIpaddress, tcpChannel::tcpChannel &channel);

  // dtor
  ~TcpSocketHandler() = default;

  // Start
  void Start();

  // Stop
  void Stop();

  // Connect to host
  bool ConnectToHost(std::string_view host_ip_address, uint16_t host_port_num);

  // Disconnect from host
  bool DisconnectFromHost();

  // Transmit function
  bool Transmit(TcpMessageConstPtr tcp_message);

private:
  // local Ip address
  std::string local_ip_address_;
  // local port number
  std::uint16_t local_port_num_;
  // tcp socket
  std::unique_ptr<TcpSocket> tcp_socket_;
  // store tcp channel reference
  tcpChannel::tcpChannel &channel_;
};
}  // namespace tcpSocket
}  // namespace doip_client
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_TCP_SOCKET_HANDLER_H
