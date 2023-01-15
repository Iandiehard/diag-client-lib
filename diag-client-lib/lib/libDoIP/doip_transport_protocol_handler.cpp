/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
//includes
#include "doip_transport_protocol_handler.h"

#include "common/common_doip_types.h"
#include "connection/connection_manager.h"

namespace ara {
namespace diag {
namespace doip {
namespace transportProtocolHandler {
/*
 @ Class Name        : doipTransportProtocolHandler
 @ Class Description : This class must be instantiated by user for using the DoIP functionalities.  
                       This will inherit uds transport protocol handler                              
 */
//ctor
DoipTransportProtocolHandler::DoipTransportProtocolHandler(
  const ara::diag::uds_transport::UdsTransportProtocolHandlerID handler_id,
  ara::diag::uds_transport::UdsTransportProtocolMgr &transport_protocol_mgr)
  : ara::diag::uds_transport::UdsTransportProtocolHandler(handler_id, transport_protocol_mgr),
    handle_id_e{handler_id},
    transport_protocol_mgr_{transport_protocol_mgr},
    doip_connection_mgr_ptr{std::make_unique<connection::DoipConnectionManager>()} {
  DLT_REGISTER_CONTEXT(doipclient_main, "dphl", "DoipClient Tp Handler Context");
}

//dtor
DoipTransportProtocolHandler::~DoipTransportProtocolHandler() {
  DLT_UNREGISTER_CONTEXT(doipclient_main);
}

// Return the UdsTransportProtocolHandlerID, which was given to the implementation during
// construction (ctor call)
ara::diag::uds_transport::UdsTransportProtocolHandlerID DoipTransportProtocolHandler::GetHandlerID() const {
  return handle_id_e;
}

// Initializes handler
ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult
DoipTransportProtocolHandler::Initialize() {
  return (ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk);
}

// Start processing the implemented Uds Transport Protocol & Tcp Transport Protocol
void DoipTransportProtocolHandler::Start() {
}

// Method to indicate that this UdsTransportProtocolHandler & TcpTransportProtocolHandler should terminate
void DoipTransportProtocolHandler::Stop() {
}

// Get or Create connection
std::shared_ptr<ara::diag::connection::Connection>
DoipTransportProtocolHandler::FindOrCreateTcpConnection(
  const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversation,
  kDoip_String &tcp_ip_address,
  uint16_t port_num) {
  DLT_LOG(doipclient_main, DLT_LOG_INFO,
          DLT_CSTRING("Doip tcp protocol requested with local endpoints :"),
          DLT_CSTRING("<tcp:"),
          DLT_STRING(tcp_ip_address.c_str()),
          DLT_CSTRING(">"));
  return (doip_connection_mgr_ptr->FindOrCreateTcpConnection(
    conversation,
    tcp_ip_address,
    port_num));
}

std::shared_ptr<ara::diag::connection::Connection>
DoipTransportProtocolHandler::FindOrCreateUdpConnection(
  const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversation,
  kDoip_String &udp_ip_address,
  uint16_t port_num) {
  DLT_LOG(doipclient_main, DLT_LOG_INFO,
          DLT_CSTRING("Doip udp protocol requested with local endpoints :"),
          DLT_CSTRING("<udp:"),
          DLT_STRING(udp_ip_address.c_str()),
          DLT_CSTRING(">"));
  return (doip_connection_mgr_ptr->FindOrCreateUdpConnection(
    conversation,
    udp_ip_address,
    port_num));
}
}  // namespace transportProtocolHandler
}  // namespace doip
}  // namespace diag
}  // namespace ara
