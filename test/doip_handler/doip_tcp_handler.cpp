/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "doip_handler/doip_tcp_handler.h"

namespace ara {
namespace diag {
namespace doip {

DoipTcpHandler::DoipTcpHandler(std::string_view local_tcp_address, std::uint16_t tcp_port_num)
  : tcp_socket_handler_{
      std::make_unique<tcpSocket::DoipTcpSocketHandler>(local_tcp_address, tcp_port_num)} {
}

DoipTcpHandler::~DoipTcpHandler() = default;

DoipTcpHandler::DoipChannel& DoipTcpHandler::CreateDoipChannel(std::uint16_t logical_address) {
  // create new doip channel
  doip_channel_list_.emplace(logical_address,
                             std::make_unique<DoipTcpHandler::DoipChannel>(logical_address, *tcp_socket_handler_));
  return *doip_channel_list_[logical_address];
}

DoipTcpHandler::DoipChannel::DoipChannel(std::uint16_t logical_address,
                                         tcpSocket::DoipTcpSocketHandler &tcp_socket_handler)
  : logical_address_{logical_address},
    tcp_socket_handler_{tcp_socket_handler},
    tcp_connection_{},
    exit_request_{false},
    running_{false} {
  // Start thread to receive messages
  thread_ = std::thread([this]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_) {
      if (!running_) {
        cond_var_.wait(lck, [this]() { return exit_request_.load(); });
      }
      if (!exit_request_.load()) {
        if (running_) { this->StartAcceptingConnection(); }
      }
    }
    StopAcceptingConnection();
  });
}

DoipTcpHandler::DoipChannel::~DoipChannel() {
  exit_request_ = true;
  running_ = false;
  cond_var_.notify_all();
  thread_.join();
}

void DoipTcpHandler::DoipChannel::Initialize() {
  running_ = true;
  cond_var_.notify_all();
}

void DoipTcpHandler::DoipChannel::DeInitialize() {
  running_ = false;
  cond_var_.notify_all();
}

void DoipTcpHandler::DoipChannel::StartAcceptingConnection() {
  // Get the tcp connection - this will return after client is connected
  tcp_connection_ = std::move(
      tcp_socket_handler_.CreateTcpConnection(
          [this](TcpMessagePtr tcp_rx_message) {
            // handle message
            this->HandleMessage(std::move(tcp_rx_message));
          }
          ));
  tcp_connection_->StartReception();
}

void DoipTcpHandler::DoipChannel::StopAcceptingConnection() {
  tcp_connection_->StopReception();
}

void DoipTcpHandler::DoipChannel::HandleMessage(TcpMessagePtr tcp_rx_message) {

}

}  // namespace doip
}  // namespace diag
}  // namespace ara
