/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "handler/udp_transport_handler.h"

#include "connection/connection_manager.h"

namespace ara {
namespace diag {
namespace doip {
namespace udpTransport {
// ctor
UdpTransportHandler::UdpTransportHandler(
  kDoip_String &localIpaddress,
  uint16_t portNum,
  connection::DoipUdpConnection &doip_connection)
    : doip_connection_{doip_connection},
      udp_channel{
        std::make_unique<ara::diag::doip::udpChannel::UdpChannel>(localIpaddress, portNum, *this)} {
}

// dtor
UdpTransportHandler::~UdpTransportHandler() {
}

//Initialize the Udp Transport Handler
uds_transport::UdsTransportProtocolHandler::InitializationResult
UdpTransportHandler::Initialize() {
  return (udp_channel->Initialize());
}

// start handler
void UdpTransportHandler::Start() {
  udp_channel->Start();
}

// stop handler
void UdpTransportHandler::Stop() {
  udp_channel->Stop();
}

// Transmit
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
UdpTransportHandler::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message,
                              ara::diag::uds_transport::ChannelID channel_id) {
  return (udp_channel->Transmit(std::move(message)));
}

std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
          ara::diag::uds_transport::UdsMessagePtr>
UdpTransportHandler::IndicateMessage(
  ara::diag::uds_transport::UdsMessage::Address source_addr,
  ara::diag::uds_transport::UdsMessage::Address target_addr,
  ara::diag::uds_transport::UdsMessage::TargetAddressType type,
  ara::diag::uds_transport::ChannelID channel_id, std::size_t size,
  ara::diag::uds_transport::Priority priority,
  ara::diag::uds_transport::ProtocolKind protocol_kind,
  std::vector<uint8_t> payloadInfo) {
  return (doip_connection_.IndicateMessage(source_addr,
                                           target_addr,
                                           type,
                                           channel_id,
                                           size,
                                           priority,
                                           protocol_kind,
                                           payloadInfo));
}

void UdpTransportHandler::HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) {
  doip_connection_.HandleMessage(std::move(message));
}
}  // namespace udpTransport
}  // namespace doip
}  // namespace diag
}  // namespace ara
