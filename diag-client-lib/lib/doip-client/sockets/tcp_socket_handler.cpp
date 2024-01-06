/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "sockets/tcp_socket_handler.h"

#include <utility>

#include "channel/tcp_channel/doip_tcp_channel.h"
#include "common/logger.h"
#include "error_domain/doip_error_domain.h"

namespace doip_client {
namespace sockets {

TcpSocketHandler::TcpSocketHandler(TcpSocket socket)
    : tcp_client_{std::make_unique<TcpClient>(std::move(socket))},
      state_{SocketHandlerState::kSocketDisconnected},
      handler_read_{} {}

TcpSocketHandler::TcpSocketHandler(TcpSocketHandler &&other) noexcept
    : tcp_client_{std::move(other.tcp_client_)},
      state_{other.state_.load()},
      handler_read_{std::move(other.handler_read_)} {}

TcpSocketHandler &TcpSocketHandler::operator=(TcpSocketHandler &&other) noexcept {
  if (this != &other) {
    tcp_client_ = std::move(other.tcp_client_);
    state_.store(other.state_.load());
    handler_read_ = std::move(other.handler_read_);
  }
  return *this;
}

void TcpSocketHandler::Start() {
  tcp_client_->SetReadHandler([this](TcpMessagePtr tcp_message) {
    if (handler_read_) { handler_read_(std::move(tcp_message)); }
  });
  tcp_client_->Initialize();
}

void TcpSocketHandler::Stop() {
  if (state_.load() != SocketHandlerState::kSocketDisconnected) { DisconnectFromHost(); }
  tcp_client_->DeInitialize();
}

void TcpSocketHandler::SetReadHandler(TcpSocketHandler::HandlerRead read_handler) {
  handler_read_ = std::move(read_handler);
}

core_type::Result<void> TcpSocketHandler::ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
  core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError)};
  if (state_.load() != SocketHandlerState::kSocketConnected) {
    if (tcp_client_->ConnectToHost(host_ip_address, host_port_num)) {
      state_.store(SocketHandlerState::kSocketConnected);
      result.EmplaceValue();
    }  // else, connect failed
  } else {
    // already connected
    result.EmplaceValue();
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Tcp socket socket already connected"; });
  }
  return result;
}

core_type::Result<void> TcpSocketHandler::DisconnectFromHost() {
  core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError)};
  if (state_.load() == SocketHandlerState::kSocketConnected) {
    tcp_client_->DisconnectFromHost();
    state_.store(SocketHandlerState::kSocketDisconnected);
    result.EmplaceValue();
  } else {
    // Not connected
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "Tcp socket already is in disconnected state"; });
  }
  return result;
}

core_type::Result<void> TcpSocketHandler::Transmit(TcpMessageConstPtr tcp_message) {
  core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError)};
  if (state_.load() == SocketHandlerState::kSocketConnected) {
    if (tcp_client_->Transmit(std::move(tcp_message))) { result.EmplaceValue(); }
  } else {
    // not connected
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "Tcp socket Offline, please connect to server first"; });
  }
  return result;
}

TcpSocketHandler::SocketHandlerState TcpSocketHandler::GetSocketHandlerState() const { return state_.load(); }

}  // namespace sockets
}  // namespace doip_client
