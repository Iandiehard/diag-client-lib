/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
//includes
#include "doip_transport_protocol_handler.h"

#include "common/common_doip_types.h"
#include "common/logger.h"
#include "connection/connection_manager.h"

namespace doip_client {
namespace transportProtocolHandler {
/*
 @ Class Name        : doipTransportProtocolHandler
 @ Class Description : This class must be instantiated by user for using the DoIP functionalities.  
                       This will inherit uds transport protocol handler                              
 */
//ctor
DoipTransportProtocolHandler::DoipTransportProtocolHandler(
    const uds_transport::UdsTransportProtocolHandlerID handler_id,
    uds_transport::UdsTransportProtocolMgr &transport_protocol_mgr)
    : uds_transport::UdsTransportProtocolHandler(handler_id, transport_protocol_mgr),
      handle_id_e{handler_id},
      transport_protocol_mgr_{transport_protocol_mgr},
      doip_connection_mgr_ptr{std::make_unique<connection::DoipConnectionManager>()} {}

//dtor
DoipTransportProtocolHandler::~DoipTransportProtocolHandler() = default;

// Return the UdsTransportProtocolHandlerID, which was given to the implementation during
// construction (ctor call)
uds_transport::UdsTransportProtocolHandlerID DoipTransportProtocolHandler::GetHandlerID() const { return handle_id_e; }

// Initializes handler
uds_transport::UdsTransportProtocolHandler::InitializationResult DoipTransportProtocolHandler::Initialize() {
  return (uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk);
}

// Start processing the implemented Uds Transport Protocol & Tcp Transport Protocol
void DoipTransportProtocolHandler::Start() {}

// Method to indicate that this UdsTransportProtocolHandler & TcpTransportProtocolHandler should terminate
void DoipTransportProtocolHandler::Stop() {}

// Get or Create connection
std::shared_ptr<uds_transport::Connection> DoipTransportProtocolHandler::FindOrCreateTcpConnection(
    const std::shared_ptr<uds_transport::ConversionHandler> &conversation, std::string_view tcp_ip_address,
    uint16_t port_num) {
  logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [tcp_ip_address](std::stringstream &msg) {
        msg << "Doip Tcp protocol requested with local endpoint : "
            << "<Tcp: " << tcp_ip_address << ">";
      });
  return (doip_connection_mgr_ptr->FindOrCreateTcpConnection(conversation, tcp_ip_address, port_num));
}

std::shared_ptr<uds_transport::Connection> DoipTransportProtocolHandler::FindOrCreateUdpConnection(
    const std::shared_ptr<uds_transport::ConversionHandler> &conversation, std::string_view udp_ip_address,
    uint16_t port_num) {
  logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [udp_ip_address](std::stringstream &msg) {
        msg << "Doip Udp protocol requested with local endpoint : "
            << "<Udp: " << udp_ip_address << ">";
      });
  return (doip_connection_mgr_ptr->FindOrCreateUdpConnection(conversation, udp_ip_address, port_num));
}
}  // namespace transportProtocolHandler
}  // namespace doip_client
