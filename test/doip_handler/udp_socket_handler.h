/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAG_CLIENT_UDP_SOCKET_HANDLER_H
#define DIAG_CLIENT_UDP_SOCKET_HANDLER_H

//includes
#include <functional>
#include <string>

#include "socket/udp/udp_client.h"

namespace doip_handler {
namespace udpSocket {

// typedefs
using UdpSocket = boost_support::socket::udp::createUdpClientSocket;
using UdpMessage = boost_support::socket::udp::UdpMessageType;
using UdpMessagePtr = boost_support::socket::udp::UdpMessagePtr;
using UdpMessageConstPtr = boost_support::socket::udp::UdpMessageConstPtr;
using kDoip_String = std::string;
using UdpMessageFunctor = std::function<void(UdpMessagePtr)>;

/*
 @ Class Name        : DoipUdpSocketHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver
 */
class DoipUdpSocketHandler {
 public:
  // Port Type
  using PortType = boost_support::socket::udp::createUdpClientSocket::PortType;

 public:
  //ctor
  DoipUdpSocketHandler(kDoip_String &local_ip_address, uint16_t port_num, PortType port_type,
                       UdpMessageFunctor udp_handler);

  //dtor
  ~DoipUdpSocketHandler() = default;

  //start
  void Start();

  //stop
  void Stop();

  // function to trigger transmission
  bool Transmit(UdpMessageConstPtr udp_tx_message);

 private:
  // local Ip address
  kDoip_String local_ip_address_;

  // Host Ip address
  kDoip_String host_ip_address_;

  // Host port number
  uint16_t port_num_;

  // Port type
  UdpSocket::PortType port_type_;

  // udp socket
  std::unique_ptr<UdpSocket> udp_socket_;
};

}  // namespace udpSocket
}  // namespace doip_handler

#endif  // DIAG_CLIENT_UDP_SOCKET_HANDLER_H
