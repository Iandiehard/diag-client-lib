/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/connection/udp_connection/udp_connection.h"

#include <string_view>

#include "uds_transport-layer-api/conversation_handler.h"

namespace doip_client {
namespace connection {
namespace udp_connection {
namespace {
using namespace std::literals;
/**
 * @brief   Udp connection name
 */
constexpr std::string_view kDoipUdpConnectionName{"DUdpCntn_"sv};
}  // namespace

DoipUdpConnection::DoipUdpConnection(uds_transport::ConversionHandler const &conversation_handler,
                                     std::string_view udp_ip_address, std::uint16_t port_num)
    : uds_transport::Connection{kDoipUdpConnectionName, 1, conversation_handler},
      doip_udp_channel_{sockets::UdpSocketHandler{UdpClient{udp_ip_address, port_num}},
                        sockets::UdpSocketHandler{UdpClient{udp_ip_address, port_num}}, *this} {}

DoipUdpConnection::InitializationResult DoipUdpConnection::Initialize() {
  return InitializationResult::kInitializeOk;
}

void DoipUdpConnection::Start() { doip_udp_channel_.Start(); }

void DoipUdpConnection::Stop() { doip_udp_channel_.Stop(); }

bool DoipUdpConnection::IsConnectToHost() { return false; }

uds_transport::UdsTransportProtocolMgr::ConnectionResult DoipUdpConnection::ConnectToHost(
    uds_transport::UdsMessageConstPtr) {
  return uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed;
}

uds_transport::UdsTransportProtocolMgr::DisconnectionResult
DoipUdpConnection::DisconnectFromHost() {
  return uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed;
}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DoipUdpConnection::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                   uds_transport::UdsMessage::Address target_addr,
                                   uds_transport::UdsMessage::TargetAddressType type,
                                   uds_transport::ChannelID channel_id, std::size_t size,
                                   uds_transport::Priority priority,
                                   uds_transport::ProtocolKind protocol_kind,
                                   core_type::Span<std::uint8_t const> payload_info) {
  // Send Indication to conversion
  return conversation_handler_.IndicateMessage(source_addr, target_addr, type, channel_id, size,
                                               priority, protocol_kind, payload_info);
}

uds_transport::UdsTransportProtocolMgr::TransmissionResult DoipUdpConnection::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  return doip_udp_channel_.Transmit(std::move(message));
}

void DoipUdpConnection::HandleMessage(uds_transport::UdsMessagePtr message) {
  // send full message to conversion
  conversation_handler_.HandleMessage(std::move(message));
}

}  // namespace udp_connection
}  // namespace connection
}  // namespace doip_client