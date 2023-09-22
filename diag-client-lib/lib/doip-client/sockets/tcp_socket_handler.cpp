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

TcpSocketHandler::TcpSocketHandler(std::string_view local_ip_address, TcpChannel &channel)
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
  if (state_.load() != SocketHandlerState::kSocketOffline) { DisconnectFromHost(); }
  tcp_socket_.reset();
}

core_type::Result<void> TcpSocketHandler::ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
  core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError)};
  if (state_.load() != SocketHandlerState::kSocketConnected) {
    tcp_socket_->Open()
        .AndThen([this]() noexcept { state_.store(SocketHandlerState::kSocketOnline); })
        .AndThen([this, &result, host_ip_address, host_port_num]() {
          return tcp_socket_->ConnectToHost(host_ip_address, host_port_num).AndThen([this, &result]() {
            state_.store(SocketHandlerState::kSocketConnected);
            result.EmplaceValue();
          });
        });
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
    tcp_socket_->DisconnectFromHost()
        .AndThen([this]() noexcept { state_.store(SocketHandlerState::kSocketDisconnected); })
        .AndThen([this, &result]() noexcept {
          return tcp_socket_->Destroy().AndThen([this, &result]() {
            state_.store(SocketHandlerState::kSocketOffline);
            result.EmplaceValue();
          });
        });
  } else {
    // not connected
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "Tcp socket already in not connected state"; });
  }
  return result;
}

core_type::Result<void> TcpSocketHandler::Transmit(TcpMessageConstPtr tcp_message) {
  core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError)};
  if (state_.load() == SocketHandlerState::kSocketConnected) {
    if (tcp_socket_->Transmit(std::move(tcp_message)).HasValue()) { result.EmplaceValue(); }
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
