/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LIBDOIP_HANDLER_TCP_TRANSPORT_HANDLER_H
#define LIB_LIBDOIP_HANDLER_TCP_TRANSPORT_HANDLER_H
//includes
#include <string_view>

#include "channel/tcp_channel.h"
#include "common/common_doip_types.h"

namespace ara {
namespace diag {
namespace doip {
//forward declaration
namespace connection {
class DoipTcpConnection;
}

namespace tcpTransport {
/*
 @ Class Name        : tcp_TransportHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */
class TcpTransportHandler {
public:
  // ctor
  TcpTransportHandler(std::string_view local_ip_address, uint16_t port_num, uint8_t total_tcp_channel_req,
                      connection::DoipTcpConnection &doip_connection);

  // dtor
  ~TcpTransportHandler();

  // Initialize
  uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();

  // Start
  void Start();

  // Stop
  void Stop();

  // Check if already connected to host
  bool IsConnectToHost();

  // Connect to remote Host
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(uds_transport::UdsMessageConstPtr message);

  // Disconnect from remote Host
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost();

  // Transmit
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(uds_transport::UdsMessageConstPtr message,
                                                                      uds_transport::ChannelID channel_id);

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind, std::vector<uint8_t> payloadInfo);

  // Hands over a valid received Uds message (currently this is only a request type) from transport
  // layer to session layer
  void HandleMessage(uds_transport::UdsMessagePtr message);

private:
  // reference to doip connection
  connection::DoipTcpConnection &doip_connection_;
  // Tcp channel responsible for transmitting and reception of TCP messages
  std::unique_ptr<ara::diag::doip::tcpChannel::tcpChannel> tcp_channel_;
};
}  // namespace tcpTransport
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // LIB_LIBDOIP_HANDLER_TCP_TRANSPORT_HANDLER_H
