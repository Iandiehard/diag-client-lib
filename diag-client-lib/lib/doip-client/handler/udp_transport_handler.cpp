/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "handler/udp_transport_handler.h"

#include "connection/connection_manager.h"

namespace doip_client {
namespace udp_transport {
// ctor
UdpTransportHandler::UdpTransportHandler(std::string_view localIpaddress, uint16_t portNum,
                                         uds_transport::Connection &connection)
    : connection_{connection},
      udp_channel{std::make_unique<udpChannel::UdpChannel>(localIpaddress, portNum, *this)} {}

// dtor
UdpTransportHandler::~UdpTransportHandler() = default;

//Initialize the Udp Transport Handler
uds_transport::UdsTransportProtocolHandler::InitializationResult UdpTransportHandler::Initialize() {
  return (udp_channel->Initialize());
}

// start handler
void UdpTransportHandler::Start() { udp_channel->Start(); }

// stop handler
void UdpTransportHandler::Stop() { udp_channel->Stop(); }

// Transmit
uds_transport::UdsTransportProtocolMgr::TransmissionResult UdpTransportHandler::Transmit(
    uds_transport::UdsMessageConstPtr message, uds_transport::ChannelID channel_id) {
  return (udp_channel->Transmit(std::move(message)));
}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
UdpTransportHandler::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                     uds_transport::UdsMessage::Address target_addr,
                                     uds_transport::UdsMessage::TargetAddressType type,
                                     uds_transport::ChannelID channel_id, std::size_t size,
                                     uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                                     core_type::Span<std::uint8_t> payloadInfo) {
  return (connection_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                      payloadInfo));
}

void UdpTransportHandler::HandleMessage(uds_transport::UdsMessagePtr message) {
  connection_.HandleMessage(std::move(message));
}
}  // namespace udp_transport
}  // namespace doip_client
