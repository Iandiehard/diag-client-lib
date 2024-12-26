/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_MANAGER_H

#include <cstdint>

#include "uds_transport/uds_message.h"

namespace uds_transport {

class UdsTransportProtocolMgr {
 public:
  // Result for Indication of message received
  enum class IndicationResult : std::uint8_t {
    kIndicationOk = 0U,
    kIndicationOccupied,
    kIndicationOverflow,
    kIndicationUnknownTargetAddress,
    kIndicationPending,
    kIndicationNOk
  };
  // Result for transmission of message sent
  enum class TransmissionResult : std::uint8_t {
    kTransmitOk = 0U,
    kTransmitFailed,
    kNoTransmitAckReceived,
    kNegTransmitAckReceived,
    kBusyProcessing
  };
  // Result for connection to remote endpoint
  enum class ConnectionResult : std::uint8_t {
    kConnectionOk = 0U,
    kConnectionFailed,
    kConnectionTimeout
  };
  // Result for disconnection to remote endpoint
  enum class DisconnectionResult : std::uint8_t { kDisconnectionOk = 0U, kDisconnectionFailed };

  //ctor
  UdsTransportProtocolMgr() = default;

  //dtor
  virtual ~UdsTransportProtocolMgr() = default;

  // initialize all the transport protocol handler
  virtual void Startup() = 0;

  // start all the transport protocol handler
  virtual void Run() = 0;

  // terminate all the transport protocol handler
  virtual void Shutdown() = 0;
};
}  // namespace uds_transport
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_MANAGER_H
