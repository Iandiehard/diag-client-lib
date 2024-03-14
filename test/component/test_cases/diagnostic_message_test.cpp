/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
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
const std::uint16_t kDiagClientLogicalAddress{0x0001U};
// Diag Test Server logical address
const std::uint16_t kDiagServerLogicalAddress{0xFA25U};
// Path to json file
constexpr std::string_view kDiagClientConfigPath{"diag_client_config.json"};
// Default routing activation type
constexpr std::uint8_t kDoipRoutingActivationReqActTypeDefault{0x00};
// Successful routing activation response code
constexpr std::uint8_t kDoipRoutingActivationResCodeRoutingSuccessful{0x10};

class UdsMessage : public diag::client::uds_message::UdsMessage {
 public:
  // alias of ByteVector
  using ByteVector = diag::client::uds_message::UdsMessage::ByteVector;

 public:
  // ctor
  UdsMessage(std::string_view host_ip_address, ByteVector payload)
      : host_ip_address(host_ip_address),
        uds_payload{std::move(payload)} {}

  // dtor
  ~UdsMessage() override = default;

 private:
  // host ip address
  IpAddress host_ip_address;
  // store only UDS payload to be sent
  ByteVector uds_payload;

  const ByteVector& GetPayload() const override { return uds_payload; }

  // return the underlying buffer for write access
  ByteVector& GetPayload() override { return uds_payload; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address; };
};

// Fixture to test Routing activation functionality
class DiagMessageFixture : public component::ComponentTest {
 public:
  using TcpAcceptor = boost_support::server::tcp::TcpAcceptor;

  using TcpServer = boost_support::server::tcp::TcpServer;

 protected:
  DiagMessageFixture()
      : tcp_acceptor_{kDiagTcpIpAddress, kDiagTcpPortNum, 1u},
        doip_tcp_handler_{},
        diag_client_{diag::client::CreateDiagnosticClient(kDiagClientConfigPath)} {}

  void SetUp() override {
    ASSERT_TRUE(diag_client_->Initialize().HasValue());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void TearDown() override {
    if (doip_tcp_handler_) { doip_tcp_handler_->DeInitialize(); }
    diag_client_->DeInitialize();
  }

  template<typename Functor>
  auto CreateServerWithExpectation(Functor expectation_functor) noexcept -> std::future<bool> {
    return std::async(std::launch::async, [this, expectation_functor = std::move(expectation_functor)]() {
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
TEST_F(DiagMessageFixture, VerifyDiagPositiveResponse) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};

  std::future<bool> is_server_created{CreateServerWithExpectation([this, &kDiagRequest]() {
    // Create an expectation of routing activation response
    EXPECT_CALL(*doip_tcp_handler_, ProcessRoutingActivationRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(::testing::Invoke([this](std::uint16_t client_source_address, std::uint8_t activation_type,
                                           std::optional<std::uint8_t> vm_specific) {
          EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
          EXPECT_EQ(activation_type, kDoipRoutingActivationReqActTypeDefault);
          EXPECT_FALSE(vm_specific.has_value());
          // Send Routing activation response
          doip_tcp_handler_->SendTcpMessage(doip_tcp_handler_->ComposeRoutingActivationResponse(
              client_source_address, kDiagServerLogicalAddress, kDoipRoutingActivationResCodeRoutingSuccessful,
              std::nullopt));
        }));

    EXPECT_CALL(*doip_tcp_handler_, ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(::testing::Invoke([this, &kDiagRequest](std::uint16_t client_source_address, std::uint16_t server_target_address,
                                           core_type::Span<std::uint8_t const> diag_request) {
          EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
          EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
          EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
          // Send Diagnostic Positive Acknowledgement message
          doip_tcp_handler_->SendTcpMessage(doip_tcp_handler_->ComposeDiagnosticPositiveAcknowlegdementMessage(
              kDiagServerLogicalAddress, kDiagClientLogicalAddress, 0u));
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
  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_TRUE(diag_result.HasValue());


  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
