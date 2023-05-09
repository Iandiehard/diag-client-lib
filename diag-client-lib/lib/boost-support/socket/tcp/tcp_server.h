/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_SERVER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_SERVER_H

// includes
#include <boost/asio.hpp>
#include <string_view>
#include <vector>

#include "tcp_types.h"

namespace boost_support {
namespace socket {
namespace tcp {
// type alias for tcp socket
using Tcp = boost::asio::ip::tcp;
using TcpSocket = Tcp::socket;

class CreateTcpServerSocket {
public:
  // Tcp function template used for reception
  using TcpHandlerRead = std::function<void(TcpMessagePtr)>;

  // Tcp Server connection class to create connection with client
  class TcpServerConnection {
  public:
    // ctor
    TcpServerConnection(boost::asio::io_context &io_context, TcpHandlerRead &&tcp_handler_read);

    // dtor
    ~TcpServerConnection() = default;

    // move ctor
    TcpServerConnection(TcpServerConnection &&) = default;

    // move assignment
    TcpServerConnection &operator=(TcpServerConnection &&) = default;

    // copy ctor & assignment deleted
    TcpServerConnection(TcpServerConnection &) = delete;
    TcpServerConnection &operator=(TcpServerConnection &) = delete;

    // Get reference to underlying socket
    TcpSocket &GetSocket();

    // function to transmit tcp message
    bool Transmit(TcpMessageConstPtr udp_tx_message);

    // function to handle read
    bool ReceivedMessage();

    // function to close the socket
    bool Shutdown();

  private:
    // tcp socket
    TcpSocket tcp_socket_;

    // handler read
    TcpHandlerRead tcp_handler_read_;
  };

public:
  // type alias for tcp accepter
  using TcpAccepter = boost::asio::ip::tcp::acceptor;

  // ctor
  CreateTcpServerSocket(std::string_view local_ip_address, uint16_t local_port_num);

  // dtor
  ~CreateTcpServerSocket() = default;

  // Blocking function get a tcp connection
  TcpServerConnection GetTcpServerConnection(TcpHandlerRead &&tcp_handler_read);

private:
  // local Ip address
  std::string local_ip_address_;
  // local port number
  uint16_t local_port_num_;
  // tcp socket accepter
  std::unique_ptr<TcpAccepter> tcp_accepter_;
  // boost io context
  boost::asio::io_context io_context_;
};

}  // namespace tcp
}  // namespace socket
}  // namespace boost_support

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_SERVER_H
