/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/channel/udp_channel/doip_udp_channel.h"

namespace doip_client {
namespace channel {
namespace udp_channel {

DoipUdpChannel::DoipUdpChannel(UdpSocketHandler udp_socket_handler_broadcast,
                               UdpSocketHandler udp_socket_handler_unicast,
                               uds_transport::Connection &connection)
    : udp_socket_handler_broadcast_{std::move(udp_socket_handler_broadcast)},
      udp_socket_handler_unicast_{std::move(udp_socket_handler_unicast)},
      udp_channel_handler_{udp_socket_handler_broadcast_, udp_socket_handler_unicast_, *this},
      connection_{connection} {}

void DoipUdpChannel::Start() {
  udp_socket_handler_broadcast_.SetReadHandler([this](UdpMessagePtr udp_message) noexcept {
    ProcessReceivedUdpBroadcast(std::move(udp_message));
  });
  udp_socket_handler_unicast_.SetReadHandler([this](UdpMessagePtr udp_message) noexcept {
    ProcessReceivedUdpUnicast(std::move(udp_message));
  });
  udp_socket_handler_broadcast_.Initialize();
  udp_socket_handler_unicast_.Initialize();
}

void DoipUdpChannel::Stop() {
  udp_socket_handler_broadcast_.DeInitialize();
  udp_socket_handler_unicast_.DeInitialize();
}

void DoipUdpChannel::ProcessReceivedUdpBroadcast(DoipUdpChannel::UdpMessagePtr udp_rx_message) {
  udp_channel_handler_.HandleMessageBroadcast(std::move(udp_rx_message));
}

void DoipUdpChannel::ProcessReceivedUdpUnicast(DoipUdpChannel::UdpMessagePtr udp_rx_message) {
  udp_channel_handler_.HandleMessageUnicast(std::move(udp_rx_message));
}

uds_transport::UdsTransportProtocolMgr::TransmissionResult DoipUdpChannel::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  return udp_channel_handler_.SendVehicleIdentificationRequest(std::move(message));
}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DoipUdpChannel::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                uds_transport::UdsMessage::Address target_addr,
                                uds_transport::UdsMessage::TargetAddressType type,
                                uds_transport::ChannelID channel_id, std::size_t size,
                                uds_transport::Priority priority,
                                uds_transport::ProtocolKind protocol_kind,
                                core_type::Span<std::uint8_t const> payload_info) {
  return connection_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority,
                                     protocol_kind, payload_info);
}

void DoipUdpChannel::HandleMessage(uds_transport::UdsMessagePtr message) {
  connection_.HandleMessage(std::move(message));
}

}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client
