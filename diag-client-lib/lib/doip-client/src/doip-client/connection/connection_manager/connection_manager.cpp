/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/connection/connection_manager/connection_manager.h"

#include "doip-client/connection/tcp_connection/tcp_connection.h"
#include "doip-client/connection/udp_connection/udp_connection.h"
#include "doip-client/logger/logger.h"

namespace doip_client {
namespace connection {
namespace connection_manager {
ConnectionManager::ConnectionManager() noexcept : io_context_{} {}

std::unique_ptr<uds_transport::Connection> ConnectionManager::CreateTcpConnection(
    uds_transport::ConversionHandler const &conversation, std::string_view tcp_ip_address,
    std::uint16_t port_num) {
  logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      FILE_NAME, __LINE__, __func__, [tcp_ip_address](std::stringstream &msg) {
        msg << "Created DoIP Tcp connection with local endpoint : "
            << "<Tcp: " << tcp_ip_address << ">";
      });
  return std::make_unique<tcp_connection::DoipTcpConnection>(conversation, tcp_ip_address,
                                                             port_num);
}

std::unique_ptr<uds_transport::Connection> ConnectionManager::CreateUdpConnection(
    uds_transport::ConversionHandler const &conversation, std::string_view udp_ip_address,
    std::uint16_t port_num) {
  logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      FILE_NAME, __LINE__, __func__, [udp_ip_address](std::stringstream &msg) {
        msg << "Created DoIP Udp connection with local endpoint : "
            << "<Udp: " << udp_ip_address << ">";
      });
  return std::make_unique<udp_connection::DoipUdpConnection>(conversation, udp_ip_address,
                                                             port_num);
}

}  // namespace connection_manager
}  // namespace connection
}  // namespace doip_client
