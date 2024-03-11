/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef TEST_COMPONENT_COMMON_HANDLER_DOIP_TCP_HANDLER_H_
#define TEST_COMPONENT_COMMON_HANDLER_DOIP_TCP_HANDLER_H_

#include <gmock/gmock.h>

#include <string_view>

#include "boost-support/server/tcp/tcp_server.h"

namespace test {
namespace component {
namespace common {
namespace handler {

class DoipTcpHandler {
public:
 using TcpServer = boost_support::server::tcp::TcpServer;

 DoipTcpHandler(TcpServer tcp_server);

 void Initialize();

 void DeInitialize();

 ~DoipTcpHandler() = default;

 /*!
  * @brief           Function that gets invoked on reception of Routing activation request message
  */
 MOCK_METHOD(void, ProcessRoutingActivationRequestMessage,
             (std::uint16_t client_source_address, std::uint8_t activation_type),
             (noexcept));

 auto ComposeRoutingActivationResponse(std::uint16_t client_logical_address, std::uint16_t server_logical_address,
                                       std::uint8_t activation_response_code, std::optional<std::uint32_t> vm_specific)
     noexcept -> TcpServer::MessagePtr;

 void SendTcpMessage(TcpServer::MessageConstPtr udp_message) noexcept;

private:
 void ProcessReceivedTcpMessage(TcpServer::MessagePtr udp_message);

private:
 TcpServer tcp_server_;
};

}  // namespace handler
}  // namespace common
}  // namespace component
}  // namespace test
#endif  // TEST_COMPONENT_COMMON_HANDLER_DOIP_TCP_HANDLER_H_
