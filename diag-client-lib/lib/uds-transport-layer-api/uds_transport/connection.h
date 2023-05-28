/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONNECTION_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONNECTION_H
/* includes */
#include "protocol_handler.h"
#include "protocol_mgr.h"
#include "protocol_types.h"

namespace uds_transport {

/*
 @ Class Name        : Connection
 @ Class Description : Class to creating connection                           
 */
using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

class Connection {
public:
  // type alias for connection id
  using ConnectionId = uint8_t;

  // ctor
  Connection(ConnectionId connection_id, uds_transport::ConversionHandler &conversation)
      : conversation_{conversation},
        connection_id_{connection_id} {}

  // dtor
  virtual ~Connection() = default;

  // Initialize
  virtual InitializationResult Initialize() = 0;

  // Start the connection
  virtual void Start() = 0;

  // Stop the connection
  virtual void Stop() = 0;

  // Check if already connected to host
  virtual bool IsConnectToHost() = 0;

  // Connect to Host Server
  virtual UdsTransportProtocolMgr::ConnectionResult ConnectToHost(UdsMessageConstPtr message) = 0;

  // Disconnect from Host Server
  virtual UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost() = 0;

  // Indicate message Diagnostic message reception over TCP to user
  virtual std::pair<UdsTransportProtocolMgr::IndicationResult, UdsMessagePtr> IndicateMessage(
      UdsMessage::Address source_addr, UdsMessage::Address target_addr, UdsMessage::TargetAddressType type,
      ChannelID channel_id, std::size_t size, Priority priority, ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo) = 0;

  // Transmit tcp/udp data
  virtual UdsTransportProtocolMgr::TransmissionResult Transmit(UdsMessageConstPtr message) = 0;

  // Hands over a valid message to conversion
  virtual void HandleMessage(UdsMessagePtr message) = 0;

protected:
  // Store the conversion
  uds_transport::ConversionHandler &conversation_;

private:
  // store the connection id
  ConnectionId connection_id_;
};

}  // namespace uds_transport

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONNECTION_H