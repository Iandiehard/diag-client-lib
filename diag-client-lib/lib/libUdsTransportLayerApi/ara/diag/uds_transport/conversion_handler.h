/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _CONVERSION_H
#define _CONVERSION_H
/* includes */
#include "connection.h"
#include "protocol_types.h"

namespace ara {
namespace diag {
namespace conversion {
/*
 @ Class Name        : Conversion
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class ConversionHandler {
public:
  // ctor
  explicit ConversionHandler(
    ara::diag::conversion_manager::ConversionHandlerID handler_id)
    : handler_id_(handler_id) {}
  
  // dtor
  ~ConversionHandler() = default;
  
  // Indicate message Diagnostic message reception over TCP/UDP to user
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
  
  // Hands over a valid message to conversion
  virtual void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) = 0;

protected:
  ara::diag::conversion_manager::ConversionHandlerID handler_id_;
};
}  // namespace conversion
}  // namespace diag
}  // namespace ara
#endif  // _CONVERSION_H
