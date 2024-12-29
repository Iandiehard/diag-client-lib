/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/connection/tcp_connection/tcp_connection.h"

#include <string_view>

#include "uds_transport-layer-api/conversation_handler.h"

namespace doip_client {
namespace connection {
namespace tcp_connection {
namespace {
using namespace std::literals;

/**
 * @brief   Tcp connection name
 */
constexpr std::string_view kDoipTcpConnectionName{"DTcpCntn_"sv};
}  // namespace

DoipTcpConnection::DoipTcpConnection(uds_transport::ConversionHandler const &conversation_handler,
                                     std::string_view tcp_ip_address,
                                     std::uint16_t port_num) noexcept
    : uds_transport::Connection{kDoipTcpConnectionName, 1u, conversation_handler},
      doip_tcp_channel_{
          sockets::TcpSocketHandler{TcpClient{GetConnectionName(), tcp_ip_address, port_num}},
          *this} {}

DoipTcpConnection::InitializationResult DoipTcpConnection::Initialize() {
  return InitializationResult::kInitializeOk;
}

void DoipTcpConnection::Start() { doip_tcp_channel_.Start(); }

void DoipTcpConnection::Stop() { doip_tcp_channel_.Stop(); }

bool DoipTcpConnection::IsConnectToHost() { return doip_tcp_channel_.IsConnectedToHost(); }

uds_transport::UdsTransportProtocolMgr::ConnectionResult DoipTcpConnection::ConnectToHost(
    uds_transport::UdsMessageConstPtr message) {
  return doip_tcp_channel_.ConnectToHost(std::move(message));
}

uds_transport::UdsTransportProtocolMgr::DisconnectionResult
DoipTcpConnection::DisconnectFromHost() {
  return doip_tcp_channel_.DisconnectFromHost();
}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DoipTcpConnection::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                   uds_transport::UdsMessage::Address target_addr,
                                   uds_transport::UdsMessage::TargetAddressType type,
                                   uds_transport::ChannelID channel_id, std::size_t size,
                                   uds_transport::Priority priority,
                                   uds_transport::ProtocolKind protocol_kind,
                                   core_type::Span<std::uint8_t const> payload_info) {
  return conversation_handler_.IndicateMessage(source_addr, target_addr, type, channel_id, size,
                                               priority, protocol_kind, payload_info);
}

uds_transport::UdsTransportProtocolMgr::TransmissionResult DoipTcpConnection::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  return doip_tcp_channel_.Transmit(std::move(message));
}

void DoipTcpConnection::HandleMessage(uds_transport::UdsMessagePtr message) {
  conversation_handler_.HandleMessage(std::move(message));
}

}  // namespace tcp_connection
}  // namespace connection
}  // namespace doip_client