/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LIBDOIP_SOCKETS_UDP_SOCKETHANDLER_H
#define LIB_LIBDOIP_SOCKETS_UDP_SOCKETHANDLER_H
//includes
#include <string>
#include <string_view>

#include "common/common_doip_types.h"
#include "socket/udp/udp_client.h"

namespace ara {
namespace diag {
namespace doip {
// forward class declaration
namespace udpChannel {
class UdpChannel;
}

namespace udpSocket {
// typedefs
using UdpSocket = libBoost::libSocket::udp::createUdpClientSocket;
using UdpMessage = libBoost::libSocket::udp::UdpMessageType;
using UdpMessagePtr = libBoost::libSocket::udp::UdpMessagePtr;
using UdpMessageConstPtr = libBoost::libSocket::udp::UdpMessageConstPtr;

/*
 @ Class Name        : UdpSocketHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver
 */
class UdpSocketHandler {
public:
  // Port Type
  using PortType = libBoost::libSocket::udp::createUdpClientSocket::PortType;

public:
  //ctor
  UdpSocketHandler(std::string_view local_ip_address, uint16_t port_num, PortType port_type,
                   ara::diag::doip::udpChannel::UdpChannel &channel);

  //dtor
  ~UdpSocketHandler() = default;

  //start
  void Start();

  //stop
  void Stop();

  // function to trigger transmission
  bool Transmit(UdpMessageConstPtr udp_tx_message);

private:
  // local Ip address
  std::string local_ip_address_;
  // Host Ip address
  std::string host_ip_address_;
  // Host port number
  uint16_t port_num_;
  // Port type
  UdpSocket::PortType port_type_;
  // udp socket
  std::unique_ptr<UdpSocket> udp_socket_;
  // store tcp channel reference
  ara::diag::doip::udpChannel::UdpChannel &channel_;
};
}  // namespace udpSocket
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // LIB_LIBDOIP_SOCKETS_UDP_SOCKETHANDLER_H
