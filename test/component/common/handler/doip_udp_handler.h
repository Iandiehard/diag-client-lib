/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef TEST_COMPONENT_COMMON_HANDLER_DOIP_UDP_HANDLER_H_
#define TEST_COMPONENT_COMMON_HANDLER_DOIP_UDP_HANDLER_H_

#include <gmock/gmock.h>

#include <string_view>

#include "boost-support/server/udp/udp_server.h"

namespace test {
namespace component {
namespace common {
namespace handler {

class DoipUdpHandler {
 public:
  using UdpServer = boost_support::server::udp::UdpServer;

  DoipUdpHandler(std::string_view broadcast_ip_address, std::string_view unicast_ip_address,
                 std::uint16_t local_port_number);

  void Initialize();

  void DeInitialize();

  ~DoipUdpHandler() = default;

  /*!
   * @brief           Function that gets invoked on reception of Vehicle identification request message
   */
  MOCK_METHOD(void, ProcessVehicleIdentificationRequestMessage,
              (std::string_view client_ip_address, std::uint16_t client_port_number, std::string_view eid,
               std::string_view vin),
              (noexcept));

  auto ComposeVehicleIdentificationResponse(std::string_view remote_ip_address, std::uint16_t remote_port_number,
                                            std::string_view vin, std::uint16_t logical_address, std::string_view eid,
                                            std::string_view gid, std::uint8_t action_byte,
                                            std::optional<std::uint8_t> sync_status) noexcept -> UdpServer::MessagePtr;

  void SendUdpMessage(UdpServer::MessageConstPtr udp_message) noexcept;

 private:
  void ProcessReceivedUdpMessage(UdpServer::MessagePtr udp_message);

 private:
  UdpServer udp_broadcast_server_;

  UdpServer udp_unicast_server_;
};

}  // namespace handler
}  // namespace common
}  // namespace component
}  // namespace test
#endif  // TEST_COMPONENT_COMMON_HANDLER_DOIP_UDP_HANDLER_H_
