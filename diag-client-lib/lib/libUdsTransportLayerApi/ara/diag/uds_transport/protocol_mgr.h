/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PROTOCOL_MANAGER_H_
#define _PROTOCOL_MANAGER_H_

#include "uds_message.h"

namespace ara {
namespace diag {
namespace uds_transport {
class UdsTransportProtocolMgr {
public:
  // Type of tuple to pack UdsTransportProtocolHandlerID and ChannelID together, to form a global
  // unique (among all used UdsTransportProtocolHandlers within DM) identifier of a UdsTransport
  // Protocol channel.
  using GlobalChannelIdentifier = std::tuple<UdsTransportProtocolHandlerID, ChannelID>;
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
  enum class ConnectionResult : std::uint8_t { kConnectionOk = 0U, kConnectionFailed, kConnectionTimeout };
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
}  // namespace diag
}  // namespace ara
#endif  // _PROTOCOL_MANAGER_H_
