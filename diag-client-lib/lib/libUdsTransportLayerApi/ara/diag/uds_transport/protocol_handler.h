/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PROTOCOL_HANDLER_H_
#define _PROTOCOL_HANDLER_H_

#include <string_view>

#include "protocol_mgr.h"
#include "protocol_types.h"

namespace ara {
namespace diag {
namespace connection {
class Connection;
}

namespace conversion {
class ConversionHandler;
}

namespace uds_transport {
using kDoip_String = std::string;

class UdsTransportProtocolHandler {
public:
  enum class InitializationResult : std::uint8_t { kInitializeOk = 0, kInitializeFailed = 1 };

  //ctor
  inline UdsTransportProtocolHandler(const UdsTransportProtocolHandlerID handler_id,
                                     UdsTransportProtocolMgr &transport_protocol_mgr)
      : handler_id_(handler_id),
        transport_protocol_mgr_(transport_protocol_mgr) {}

  //dtor
  virtual ~UdsTransportProtocolHandler() = default;

  // Return the UdsTransportProtocolHandlerID
  inline virtual UdsTransportProtocolHandlerID GetHandlerID() const { return handler_id_; };

  // Initialize
  virtual InitializationResult Initialize() = 0;

  // Start processing the implemented Uds Transport Protocol
  virtual void Start() = 0;

  // Method to indicate that this UdsTransportProtocolHandler should terminate
  virtual void Stop() = 0;

  // Get or Create connection a Tcp Connection
  virtual std::shared_ptr<ara::diag::connection::Connection> FindOrCreateTcpConnection(
      const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion_handler,
      std::string_view tcpIpaddress, uint16_t portNum) = 0;

  // Get or Create connection a Udp Connection
  virtual std::shared_ptr<ara::diag::connection::Connection> FindOrCreateUdpConnection(
      const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion_handler,
      std::string_view udpIpaddress, uint16_t portNum) = 0;

protected:
  UdsTransportProtocolHandlerID handler_id_;

private:
  UdsTransportProtocolMgr &transport_protocol_mgr_;
};
}  // namespace uds_transport
}  // namespace diag
}  // namespace ara
#endif  // _PROTOCOL_HANDLER_H_
