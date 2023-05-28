/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_HANDLER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_HANDLER_H

#include <string_view>

#include "protocol_mgr.h"
#include "protocol_types.h"

namespace uds_transport {
// forward declaration
class Connection;
class ConversionHandler;

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

  // Get or Create a Tcp Connection
  virtual std::shared_ptr<Connection> FindOrCreateTcpConnection(ConversionHandler &conversion_handler,
                                                                std::string_view tcpIpaddress, uint16_t portNum) = 0;

  // Get or Create an Udp Connection
  virtual std::shared_ptr<Connection> FindOrCreateUdpConnection(ConversionHandler &conversion_handler,
                                                                std::string_view udpIpaddress, uint16_t portNum) = 0;

protected:
  UdsTransportProtocolHandlerID handler_id_;

private:
  UdsTransportProtocolMgr &transport_protocol_mgr_;
};
}  // namespace uds_transport
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_HANDLER_H
