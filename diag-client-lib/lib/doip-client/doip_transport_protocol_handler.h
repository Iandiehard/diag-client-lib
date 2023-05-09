/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LIBDOIP_DOIP_TRANSPORT_PROTOCOL_HANDLER_H
#define LIB_LIBDOIP_DOIP_TRANSPORT_PROTOCOL_HANDLER_H

#include <string_view>

#include "common/common_doip_header.h"

namespace doip_client {
//forward declaration
namespace connection {
class DoipConnectionManager;
}

namespace transportProtocolHandler {
/*
 @ Class Name        : DoipTransportProtocolHandler
 @ Class Description : This class must be instantiated by user for using the DoIP functionalities.  
                       This will inherit uds transport protocol handler                              
 */
class DoipTransportProtocolHandler final : public uds_transport::UdsTransportProtocolHandler {
public:
  //ctor
  DoipTransportProtocolHandler(const uds_transport::UdsTransportProtocolHandlerID handler_id,
                               uds_transport::UdsTransportProtocolMgr &transport_protocol_mgr);

  //dtor
  ~DoipTransportProtocolHandler();

  // Return the UdsTransportProtocolHandlerID, which was given to the implementation during construction (ctor call).
  uds_transport::UdsTransportProtocolHandlerID GetHandlerID() const override;

  // Initializes handler
  uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize() override;

  // Start processing the implemented Uds Transport Protocol
  void Start() override;

  // Method to indicate that this UdsTransportProtocolHandler should terminate
  void Stop() override;

  // Get or Create Tcp connection
  std::shared_ptr<uds_transport::Connection> FindOrCreateTcpConnection(
      const std::shared_ptr<uds_transport::ConversionHandler> &conversation, std::string_view tcp_ip_address,
      uint16_t port_num) override;

  // Get or Create Udp connection
  std::shared_ptr<uds_transport::Connection> FindOrCreateUdpConnection(
      const std::shared_ptr<uds_transport::ConversionHandler> &conversation, std::string_view udp_ip_address,
      uint16_t port_num) override;

private:
  // store handle id
  uds_transport::UdsTransportProtocolHandlerID handle_id_e;
  // store the transport protocol manager
  uds_transport::UdsTransportProtocolMgr &transport_protocol_mgr_;
  // Create Doip Connection Manager
  std::unique_ptr<connection::DoipConnectionManager> doip_connection_mgr_ptr;
};

}  // namespace transportProtocolHandler
}  // namespace doip_client

#endif  // LIB_LIBDOIP_DOIP_TRANSPORT_PROTOCOL_HANDLER_H
