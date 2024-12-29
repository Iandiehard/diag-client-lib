/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/doip_client.h"

namespace doip_client {

DoipClient::DoipClient() noexcept : connection_mgr_{} {}

std::unique_ptr<uds_transport::Connection> DoipClient::CreateTcpConnection(
    uds_transport::ConversionHandler const &conversation, std::string_view tcp_ip_address,
    std::uint16_t port_num) {
  return connection_mgr_.CreateTcpConnection(conversation, tcp_ip_address, port_num);
}

std::unique_ptr<uds_transport::Connection> DoipClient::CreateUdpConnection(
    uds_transport::ConversionHandler const &conversation, std::string_view udp_ip_address,
    std::uint16_t port_num) {
  return connection_mgr_.CreateUdpConnection(conversation, udp_ip_address, port_num);
}

}  // namespace doip_client
