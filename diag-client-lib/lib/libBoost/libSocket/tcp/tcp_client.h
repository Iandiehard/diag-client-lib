/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBSOCKET_TCP_TCP_CLIENT_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBSOCKET_TCP_TCP_CLIENT_H
// includes
#include <string>
#include <string_view>

#include "tcp_types.h"

namespace libBoost {
namespace libSocket {
namespace tcp {

/*
 @ Class Name        : Create Tcp Socket
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver
*/
class CreateTcpClientSocket {
public:
  // Type alias for tcp protocol
  using Tcp = boost::asio::ip::tcp;
  // Type alias for tcp socket
  using TcpSocket = Tcp::socket;
  // Type alias for tcp ip address
  using TcpIpAddress = boost::asio::ip::address;
  // Type alias for tcp error codes
  using TcpErrorCodeType = boost::system::error_code;
  // Tcp function template used for reception
  using TcpHandlerRead = std::function<void(TcpMessagePtr)>;

public:
  //ctor
  CreateTcpClientSocket(std::string_view local_ip_address, uint16_t local_port_num, TcpHandlerRead&& tcp_handler_read);

  //dtor
  virtual ~CreateTcpClientSocket();

  // Function to Open the socket
  bool Open();

  // Function to Connect to host
  bool ConnectToHost(std::string_view host_ip_address, uint16_t host_port_num);

  // Function to Disconnect from host
  bool DisconnectFromHost();

  // Function to trigger transmission
  bool Transmit(TcpMessageConstPtr tcpMessage);

  // Function to destroy the socket
  bool Destroy();

private:
  // local Ip address
  std::string local_ip_address_;
  // local port number
  uint16_t local_port_num_;
  // tcp socket
  std::unique_ptr<TcpSocket> tcp_socket_;
  // boost io context
  boost::asio::io_context io_context_;
  // flag to terminate the thread
  std::atomic_bool exit_request_;
  // flag th start the thread
  std::atomic_bool running_;
  // conditional variable to block the thread
  std::condition_variable cond_var_;
  // threading var
  std::thread thread_;
  // locking critical section
  std::mutex mutex_;
  // Handler invoked during read operation
  TcpHandlerRead tcp_handler_read_;

private:
  // function to handle read
  void HandleMessage();
};
}  // namespace tcp
}  // namespace libSocket
}  // namespace libBoost
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBSOCKET_TCP_TCP_CLIENT_H
