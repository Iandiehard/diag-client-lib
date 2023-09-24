/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "doip_transport_protocol_handler.h"

#include "common/logger.h"
#include "connection/connection_manager.h"

namespace doip_client {
namespace transport_protocol_handler {

DoipTransportProtocolHandler::DoipTransportProtocolHandler(
    UdsTransportProtocolHandlerId const handler_id,
    uds_transport::UdsTransportProtocolMgr const &transport_protocol_mgr)
    : uds_transport::UdsTransportProtocolHandler(handler_id, transport_protocol_mgr),
      doip_connection_mgr_{} {}

DoipTransportProtocolHandler::~DoipTransportProtocolHandler() = default;

DoipTransportProtocolHandler::InitializationResult DoipTransportProtocolHandler::Initialize() {
  return (InitializationResult::kInitializeOk);
}

void DoipTransportProtocolHandler::Start() {}

void DoipTransportProtocolHandler::Stop() {}

std::unique_ptr<uds_transport::Connection> DoipTransportProtocolHandler::CreateTcpConnection(
    uds_transport::ConversionHandler &conversation, std::string_view tcp_ip_address, std::uint16_t port_num) {
  logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [tcp_ip_address](std::stringstream &msg) {
        msg << "Doip Tcp protocol requested with local endpoint : "
            << "<Tcp: " << tcp_ip_address << ">";
      });
  return (connection::DoipConnectionManager::FindOrCreateTcpConnection(conversation, tcp_ip_address, port_num));
}

std::unique_ptr<uds_transport::Connection> DoipTransportProtocolHandler::CreateUdpConnection(
    uds_transport::ConversionHandler &conversation, std::string_view udp_ip_address, std::uint16_t port_num) {
  logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [udp_ip_address](std::stringstream &msg) {
        msg << "Doip Udp protocol requested with local endpoint : "
            << "<Udp: " << udp_ip_address << ">";
      });
  return (connection::DoipConnectionManager::FindOrCreateUdpConnection(conversation, udp_ip_address, port_num));
}
}  // namespace transport_protocol_handler
}  // namespace doip_client
