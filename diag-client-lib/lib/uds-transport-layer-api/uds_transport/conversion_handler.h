/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONVERSATION_H
/* includes */
#include "connection.h"
#include "protocol_types.h"

namespace uds_transport {
/*
 @ Class Name        : Conversion
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class ConversionHandler {
public:
  // ctor
  explicit ConversionHandler(conversion_manager::ConversionHandlerID handler_id) : handler_id_{handler_id} {}

  // dtor
  virtual ~ConversionHandler() = default;

  // Indicate message Diagnostic message reception over TCP/UDP to user
  virtual std::pair<UdsTransportProtocolMgr::IndicationResult, UdsMessagePtr> IndicateMessage(
      UdsMessage::Address source_addr, UdsMessage::Address target_addr, UdsMessage::TargetAddressType type,
      ChannelID channel_id, std::size_t size, Priority priority, ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo) = 0;

  // Hands over a valid message to conversion
  virtual void HandleMessage(UdsMessagePtr message) = 0;

protected:
  conversion_manager::ConversionHandlerID handler_id_;
};
}  // namespace uds_transport
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_CONVERSATION_H
