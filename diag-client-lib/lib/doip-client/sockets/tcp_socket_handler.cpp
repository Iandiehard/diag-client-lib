/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sockets/tcp_socket_handler.h"

#include <utility>

#include "channel/tcp_channel.h"
#include "common/logger.h"
#include "error_domain/doip_error_domain.h"

namespace doip_client {
namespace tcpSocket {

TcpSocketHandler::TcpSocketHandler(std::string_view local_ip_address, tcpChannel::TcpChannel &channel)
    : local_ip_address_{local_ip_address},
      local_port_num_{0U},
      tcp_socket_{},
      channel_{channel},
      state_{SocketHandlerState::kSocketOffline} {}

void TcpSocketHandler::Start() {
  tcp_socket_.emplace(local_ip_address_, local_port_num_, [this](TcpMessagePtr tcp_message) {
    channel_.ProcessReceivedTcpMessage(std::move(tcp_message));
  });
}

void TcpSocketHandler::Stop() {
  if (state_ != SocketHandlerState::kSocketOffline) { DisconnectFromHost(); }
  tcp_socket_.reset();
}

core_type::Result<void> TcpSocketHandler::ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
  using TcpErrorCode = boost_support::socket::tcp::TcpClientSocket::TcpErrorCode;
  return tcp_socket_->Open()
      .AndThen([this, host_ip_address, host_port_num]() noexcept {
        state_ = SocketHandlerState::kSocketOnline;
        return tcp_socket_->ConnectToHost(host_ip_address, host_port_num).AndThen([this]() {
          state_ = SocketHandlerState::kSocketConnected;
        });
      })
      .MapError([](TcpErrorCode const &) noexcept {
        return error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError);
      });
}

core_type::Result<void> TcpSocketHandler::DisconnectFromHost() {
  using TcpErrorCode = boost_support::socket::tcp::TcpClientSocket::TcpErrorCode;
  return tcp_socket_->DisconnectFromHost()
      .AndThen([this]() noexcept {
        state_ = SocketHandlerState::kSocketDisconnected;
        return tcp_socket_->Destroy().AndThen([this]() { state_ = SocketHandlerState::kSocketOffline; });
      })
      .MapError([](TcpErrorCode const &) noexcept {
        return error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError);
      });
}

core_type::Result<void> TcpSocketHandler::Transmit(TcpMessageConstPtr tcp_message) {
  using TcpErrorCode = boost_support::socket::tcp::TcpClientSocket::TcpErrorCode;
  return tcp_socket_->Transmit(std::move(tcp_message)).MapError([](TcpErrorCode const &) noexcept {
    return error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError);
  });
}

TcpSocketHandler::SocketHandlerState TcpSocketHandler::GetSocketHandlerState() const { return state_.load(); }

}  // namespace tcpSocket
}  // namespace doip_client
