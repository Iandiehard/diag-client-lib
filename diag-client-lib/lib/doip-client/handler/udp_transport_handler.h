/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_HANDLER_UDP_TRANSPORT_HANDLER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_HANDLER_UDP_TRANSPORT_HANDLER_H
//includes
#include <string_view>

#include "channel/udp_channel.h"
#include "common/common_doip_header.h"
#include "core/include/span.h"

namespace doip_client {
//forward declaration
namespace connection {
class DoipUdpConnection;
}

namespace udpTransport {
/*
 @ Class Name        : UdpTransportHandler
 @ Class Description : Class used to create an udp transport handler to initiate transmission
                       and reception of udp message from/to user                            
*/
class UdpTransportHandler {
public:
  // ctor
  UdpTransportHandler(std::string_view localIpaddress, uint16_t portNum, connection::DoipUdpConnection &doipConnection);

  // dtor
  ~UdpTransportHandler();

  // Initialize
  uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();

  // Start
  void Start();

  // Stop
  void Stop();

  // Transmit
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(uds_transport::UdsMessageConstPtr message,
                                                                      uds_transport::ChannelID channel_id);

  // Indicate message Diagnostic message reception over UDP to user
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
      core_type::Span<std::uint8_t> payloadInfo);

  // Hands over a valid received UDP message (currently this is only a request type) from transport
  // layer to session layer
  void HandleMessage(uds_transport::UdsMessagePtr message);

private:
  // reference to doip Connection
  connection::DoipUdpConnection &doip_connection_;
  // Udp channel responsible for transmitting and reception of UDP messages
  std::unique_ptr<udpChannel::UdpChannel> udp_channel;
};
}  // namespace udpTransport
}  // namespace doip_client
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_HANDLER_UDP_TRANSPORT_HANDLER_H
