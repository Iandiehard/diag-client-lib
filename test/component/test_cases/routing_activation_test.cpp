/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <gtest/gtest.h>
#include <future>
#include <string_view>
#include <thread>
#include <optional>
#include "boost-support/server/tcp/tcp_acceptor.h"
#include "boost-support/server/tcp/tcp_server.h"
#include "common/handler/doip_tcp_handler.h"
#include "component_test.h"
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"

namespace test {
namespace component {
namespace test_cases {
// Diag Test Server Tcp Ip Address
constexpr std::string_view kDiagTcpIpAddress{"172.16.25.128"};
// Diag Test Server port number
constexpr std::uint16_t kDiagTcpPortNum{13400u};
// Diag Test Server logical address
const std::uint16_t DiagServerLogicalAddress{0xFA25U};
// Path to json file
constexpr std::string_view kDiagClientConfigPath{"diag_client_config.json"};

// Fixture to test Routing activation functionality
class RoutingActivationFixture : public component::ComponentTest {
 public:
  using TcpAcceptor = boost_support::server::tcp::TcpAcceptor;
  
  using TcpServer = boost_support::server::tcp::TcpServer;
 protected:
  RoutingActivationFixture()
      : tcp_acceptor_{kDiagTcpIpAddress, kDiagTcpPortNum, 1u},
        doip_tcp_handler_{},
        diag_client_{diag::client::CreateDiagnosticClient(kDiagClientConfigPath)} {}

  void SetUp() override {
    // doip_udp_handler_.Initialize();
    ASSERT_TRUE(diag_client_->Initialize().HasValue());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void TearDown() override {
    diag_client_->DeInitialize();
    if(doip_tcp_handler_) {
      doip_tcp_handler_->DeInitialize();
    }
  }
  
 protected:
  // tcp acceptor
  TcpAcceptor tcp_acceptor_;
  
  // doip udp handler
  std::optional<testing::StrictMock<common::handler::DoipTcpHandler>> doip_tcp_handler_;

  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;
};

/**
 * @brief  Verify that sending of routing activation request works correctly.
 */
TEST_F(RoutingActivationFixture, VerifyRoutingActivationSuccessful) {
  std::future<bool> is_server_created{std::async(std::launch::async, [this](){
    std::optional<TcpServer> server{tcp_acceptor_.GetTcpServer()};
    if(server.has_value()) {
      doip_tcp_handler_.emplace(std::move(server).value());
      doip_tcp_handler_->Initialize();
    }
    return doip_tcp_handler_.has_value();
  })};
  
  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      diag_client_->GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(DiagServerLogicalAddress, kDiagTcpIpAddress)};

  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
