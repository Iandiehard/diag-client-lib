/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _UDP_TRANSPORT_HANDLER_H_
#define _UDP_TRANSPORT_HANDLER_H_
//includes
#include "channel/udp_channel.h"
#include "common/common_doip_header.h"

namespace ara {
namespace diag {
namespace doip {
//forward declaration
namespace connection {
class DoipUdpConnection;
}

namespace udpTransport {
/*
 @ Class Name        : UdpTransportHandler
 @ Class Description : Class used to create a udp transport handler to initiate transmission
                       and reception of udp message from/to user                            
*/
class UdpTransportHandler {
public:
  // ctor
  UdpTransportHandler(kDoip_String &localIpaddress, uint16_t portNum, connection::DoipUdpConnection &doipConnection);

  // dtor
  ~UdpTransportHandler();

  // Initialize
  ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();

  // Start
  void Start();

  // Stop
  void Stop();

  // Transmit
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(
      ara::diag::uds_transport::UdsMessageConstPtr message, ara::diag::uds_transport::ChannelID channel_id);

  // Indicate message Diagnostic message reception over UDP to user
  std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
            ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                  ara::diag::uds_transport::UdsMessage::Address target_addr,
                  ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                  ara::diag::uds_transport::ChannelID channel_id, std::size_t size,
                  ara::diag::uds_transport::Priority priority, ara::diag::uds_transport::ProtocolKind protocol_kind,
                  std::vector<uint8_t> payloadInfo);

  // Hands over a valid received UDP message (currently this is only a request type) from transport
  // layer to session layer
  void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message);

private:
  // reference to doip Connection
  connection::DoipUdpConnection &doip_connection_;
  // Udp channel responsible for transmitting and reception of UDP messages
  std::unique_ptr<ara::diag::doip::udpChannel::UdpChannel> udp_channel;
};
}  // namespace udpTransport
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // _UDP_TRANSPORT_HANDLER_H_
