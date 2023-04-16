/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef DIAG_CLIENT_DOIP_TCP_HANDLER_H
#define DIAG_CLIENT_DOIP_TCP_HANDLER_H

#include <string_view>
#include <memory>
#include <map>
#include "doip_handler/tcp_socket_handler.h"

namespace ara {
namespace diag {
namespace doip {

using TcpMessagePtr = tcpSocket::TcpMessagePtr;
using TcpMessageConstPtr = tcpSocket::TcpMessageConstPtr;

class DoipTcpHandler {
public:
  using TcpConnectionHandler = tcpSocket::DoipTcpSocketHandler::TcpConnectionHandler;
  using DoipChannelReadCallback = tcpSocket::DoipTcpSocketHandler::TcpHandlerRead;
  // Class maintaining the doip channel
  class DoipChannel{
  public:
    DoipChannel(std::uint16_t logical_address,
                tcpSocket::DoipTcpSocketHandler &tcp_socket_handler);

    ~DoipChannel();

    // Initialize
    void Initialize();

    // De-Initialize
    void DeInitialize();

    // start accepting connection from client
    void StartAcceptingConnection();

    // stop accepting connection from client
    void StopAcceptingConnection();
  private:
    // Store the logical address
    std::uint16_t logical_address_;

    // store the tcp socket handler reference
    tcpSocket::DoipTcpSocketHandler &tcp_socket_handler_;

    // Tcp connection to handler tcp req and response
    std::unique_ptr<TcpConnectionHandler> tcp_connection_;

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
  private:
    void HandleMessage(TcpMessagePtr tcp_rx_message);
  };
public:
  // ctor
  DoipTcpHandler(std::string_view local_tcp_address, std::uint16_t tcp_port_num);

  // dtor
  ~DoipTcpHandler();

  // Function to create doip channel
  DoipChannel& CreateDoipChannel(std::uint16_t logical_address);
private:
  // tcp socket handler
  std::unique_ptr<tcpSocket::DoipTcpSocketHandler> tcp_socket_handler_;

  // list of doip channel
  std::map<std::uint16_t, std::unique_ptr<DoipChannel>> doip_channel_list_;
};

}  // namespace doip
}  // namespace diag
}  // namespace ara

#endif  //DIAG_CLIENT_DOIP_TCP_HANDLER_H
