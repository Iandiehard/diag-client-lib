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
  //
  class DoipChannel{
  public:
    DoipChannel(std::uint16_t logical_address,
                tcpSocket::DoipTcpSocketHandler &tcp_socket_handler);

    //
    void SetExpectedUdsMessageResponseToBeSent();


    auto VerifyUdsMessageRequestWithExpectedMessage() noexcept -> bool;

    // start the reception
    void StartReception();

    // stop the reception
    void StopReception();
  private:
    // Store the logical address
    std::uint16_t logical_address_;

    tcpSocket::DoipTcpSocketHandler &tcp_socket_handler_;

    // Tcp connection to handler tcp req and response
    std::unique_ptr<TcpConnectionHandler> tcp_connection_;
  };
public:
  // ctor
  DoipTcpHandler(std::string_view local_udp_address, std::uint16_t udp_port_num);

  // dtor
  ~DoipTcpHandler();

  // Function to create doip channel
  DoipChannel CreateDoipChannel(std::uint16_t logical_address, DoipChannelReadCallback && doip_channel_read_cbk);

private:
  // tcp socket handler
  std::unique_ptr<tcpSocket::DoipTcpSocketHandler> tcp_socket_handler_;
};

}  // namespace doip
}  // namespace diag
}  // namespace ara

#endif  //DIAG_CLIENT_DOIP_TCP_HANDLER_H
