/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef CONNECTION_H
#define CONNECTION_H
/* includes */
#include "protocol_handler.h"
#include "protocol_mgr.h"
#include "protocol_types.h"

namespace ara {
namespace diag {
namespace connection {
/*
 @ Class Name        : Connection
 @ Class Description : Class to creating connection                           
 */
using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

class Connection {
public:
  // ctor
  Connection(
    ConnectionId connection_id,
    const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversation)
    : conversation_{conversation},
      connection_id_{connection_id} {
  }
  
  // dtor
  virtual ~Connection() = default;
  
  // Initialize
  virtual InitializationResult Initialize() = 0;
  
  // Start the connection
  virtual void Start() = 0;
  
  // Stop the connection
  virtual void Stop() = 0;
  
  // Connect to Host Server
  virtual ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
  ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) = 0;
  
  // Disconnect from Host Server
  virtual ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
  DisconnectFromHost() = 0;
  
  // Indicate message Diagnostic message reception over TCP to user
  virtual std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
    ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(
    ara::diag::uds_transport::UdsMessage::Address source_addr,
    ara::diag::uds_transport::UdsMessage::Address target_addr,
    ara::diag::uds_transport::UdsMessage::TargetAddressType type,
    ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size,
    ara::diag::uds_transport::Priority priority,
    ara::diag::uds_transport::ProtocolKind protocol_kind,
    std::vector<uint8_t> payloadInfo) = 0;
  
  // Transmit tcp/udp data
  virtual ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
  Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) = 0;
  
  // Hands over a valid message to conversion
  virtual void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) = 0;

protected:
  // Store the conversion
  const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversation_;

private:
  // store the connection id
  ConnectionId connection_id_;
};
}  // namespace connection
}  // namespace diag
}  // namespace ara
#endif  // CONNECTION_H