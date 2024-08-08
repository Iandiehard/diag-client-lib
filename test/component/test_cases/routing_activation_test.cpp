/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <gtest/gtest.h>

#include <future>
#include <optional>
#include <string_view>
#include <thread>

#include "boost-support/server/tcp/tcp_acceptor.h"
#include "boost-support/server/tcp/tcp_server.h"
#include "common/handler/doip_tcp_handler.h"
#include "component_test.h"
#include "diag-client/create_diagnostic_client.h"
#include "diag-client/diagnostic_client.h"

namespace test {
namespace component {
namespace test_cases {
// Diag Test Server Tcp Ip Address
constexpr std::string_view kDiagTcpIpAddress{"172.16.25.128"};
// Diag Test Server port number
constexpr std::uint16_t kDiagTcpPortNum{13400u};
// Diag Test Server logical address
const std::uint16_t kDiagClientLogicalAddress{0x0001U};
// Diag Test Server logical address
const std::uint16_t kDiagServerLogicalAddress{0xFA25U};
// Path to json file
constexpr std::string_view kDiagClientConfigPath{"diag_client_config.json"};
// Default routing activation type
constexpr std::uint8_t kDoipRoutingActivationReqActTypeDefault{0x00};
constexpr std::uint8_t kDoipRoutingActivationResCodeUnknownSa{0x00};
constexpr std::uint8_t kDoipRoutingActivationResCodeAllSocketActive{0x01};
constexpr std::uint8_t kDoipRoutingActivationResCodeDifferentSa{0x02};
constexpr std::uint8_t kDoipRoutingActivationResCodeActiveSa{0x03};
constexpr std::uint8_t kDoipRoutingActivationResCodeAuthentnMissng{0x04};
constexpr std::uint8_t kDoipRoutingActivationResCodeConfirmtnRejectd{0x05};
constexpr std::uint8_t kDoipRoutingActivationResCodeUnsupportdActType{0x06};
constexpr std::uint8_t kDoipRoutingActivationResCodeTlsRequired{0x07};
constexpr std::uint8_t kDoipRoutingActivationResCodeRoutingSuccessful{0x10};
constexpr std::uint8_t kDoipRoutingActivationResCodeConfirmtnRequired{0x11};

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
    ASSERT_TRUE(diag_client_->Initialize().HasValue());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void TearDown() override {
    diag_client_->DeInitialize();
    if (doip_tcp_handler_) { doip_tcp_handler_->DeInitialize(); }
  }

  // Function to create a Tcp server with test expectation
  template<typename Functor>
  auto CreateServerWithExpectation(Functor expectation_functor) noexcept -> std::future<bool> {
    return std::async(std::launch::async,
                      [this, expectation_functor = std::move(expectation_functor)]() {
                        std::optional<TcpServer> server{tcp_acceptor_.GetTcpServer()};
                        if (server.has_value()) {
                          doip_tcp_handler_.emplace(std::move(server).value());
                          doip_tcp_handler_->Initialize();
                          // Set Expectation
                          expectation_functor();
                        }
                        return doip_tcp_handler_.has_value();
                      });
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
  std::future<bool> is_server_created{CreateServerWithExpectation([this]() {
    // Create an expectation of routing activation response
    EXPECT_CALL(*doip_tcp_handler_,
                ProcessRoutingActivationRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(::testing::Invoke([this](std::uint16_t client_source_address,
                                           std::uint8_t activation_type,
                                           std::optional<std::uint8_t> vm_specific) {
          EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
          EXPECT_EQ(activation_type, kDoipRoutingActivationReqActTypeDefault);
          EXPECT_FALSE(vm_specific.has_value());
          // Send Routing activation response
          doip_tcp_handler_->SendTcpMessage(common::handler::ComposeRoutingActivationResponse(
              client_source_address, kDiagServerLogicalAddress,
              kDoipRoutingActivationResCodeRoutingSuccessful, std::nullopt));
        }));
  })};

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      diag_client_->GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(kDiagServerLogicalAddress, kDiagTcpIpAddress)};

  ASSERT_TRUE(is_server_created.get());
  EXPECT_EQ(connect_result,
            diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(
      disconnect_result,
      diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
}

class RoutingActivationFixtureValueParameter : public RoutingActivationFixture,
                                               public ::testing::WithParamInterface<std::uint8_t> {
};

INSTANTIATE_TEST_SUITE_P(RoutingActivation, RoutingActivationFixtureValueParameter,
                         testing::Values(kDoipRoutingActivationResCodeUnknownSa,
                                         kDoipRoutingActivationResCodeAllSocketActive,
                                         kDoipRoutingActivationResCodeDifferentSa,
                                         kDoipRoutingActivationResCodeActiveSa,
                                         kDoipRoutingActivationResCodeAuthentnMissng,
                                         kDoipRoutingActivationResCodeConfirmtnRejectd,
                                         kDoipRoutingActivationResCodeUnsupportdActType,
                                         kDoipRoutingActivationResCodeTlsRequired,
                                         kDoipRoutingActivationResCodeConfirmtnRequired));

/**
 * @brief  Verify that sending of routing activation request fails on reception of negative activation code .
 */
TEST_P(RoutingActivationFixtureValueParameter, VerifyRoutingActivationFailure) {
  std::future<bool> is_server_created{CreateServerWithExpectation([this]() {
    // Create an expectation of routing activation response
    EXPECT_CALL(*doip_tcp_handler_,
                ProcessRoutingActivationRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(::testing::Invoke([this](std::uint16_t client_source_address,
                                           std::uint8_t activation_type,
                                           std::optional<std::uint8_t> vm_specific) {
          EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
          EXPECT_EQ(activation_type, kDoipRoutingActivationReqActTypeDefault);
          EXPECT_FALSE(vm_specific.has_value());
          // Send Routing activation response
          doip_tcp_handler_->SendTcpMessage(common::handler::ComposeRoutingActivationResponse(
              client_source_address, kDiagServerLogicalAddress, GetParam(), std::nullopt));
        }));
  })};

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      diag_client_->GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(kDiagServerLogicalAddress, kDiagTcpIpAddress)};

  ASSERT_TRUE(is_server_created.get());
  EXPECT_EQ(connect_result,
            diag::client::conversation::DiagClientConversation::ConnectResult::kConnectFailed);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(
      disconnect_result,
      diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
