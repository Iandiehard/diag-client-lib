/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "doip_handler/tcp_socket_handler.h"

namespace ara {
namespace diag {
namespace doip {
namespace tcpSocket {

DoipTcpSocketHandler::TcpConnectionHandler::TcpConnectionHandler(
    std::unique_ptr<TcpConnection> tcp_connection)
  : tcp_connection_{std::move(tcp_connection)},
    exit_request_{false},
    running_{false} {
  // Start thread to receive messages
  thread_ = std::thread([&]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_) {
      if (!running_) {
        cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
      }
      if (!exit_request_.load()) {
        if (running_) { tcp_connection_->ReceivedMessage(); }
      }
    }
  });
}

DoipTcpSocketHandler::TcpConnectionHandler::~TcpConnectionHandler() {
  exit_request_ = true;
  running_ = false;
  cond_var_.notify_all();
  thread_.join();
}

void DoipTcpSocketHandler::TcpConnectionHandler::Initialize() {
  // start reading
  running_ = true;
  cond_var_.notify_all();
}

void DoipTcpSocketHandler::TcpConnectionHandler::DeInitialize() {
  tcp_connection_->Shutdown();
  // stop reading
  running_ = false;
}

bool DoipTcpSocketHandler::TcpConnectionHandler::Transmit(TcpMessageConstPtr tcp_tx_message) {
  return tcp_connection_->Transmit(std::move(tcp_tx_message));
}

DoipTcpSocketHandler::DoipTcpSocketHandler(std::string_view local_ip_address, uint16_t port_num)
  :local_ip_address_{local_ip_address}, port_num_{port_num} {
  tcp_socket_ = std::make_unique<TcpSocket>(local_ip_address_, port_num_);
}

std::unique_ptr<DoipTcpSocketHandler::TcpConnectionHandler> DoipTcpSocketHandler::CreateTcpConnection(
    DoipTcpSocketHandler::TcpHandlerRead &&tcp_handler_read) {
  return std::make_unique<DoipTcpSocketHandler::TcpConnectionHandler>(
      std::move(std::make_unique<TcpConnection>(
          tcp_socket_->GetTcpServerConnection(std::move(tcp_handler_read)))));
}

}  // namespace tcpSocket
}  // namespace doip
}  // namespace diag
}  // namespace ara