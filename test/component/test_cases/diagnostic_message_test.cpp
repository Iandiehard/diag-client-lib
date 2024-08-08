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
constexpr std::uint16_t kDiagTcpPortNum{13400U};
// Diag Test Server logical address
const std::uint16_t kDiagClientLogicalAddress{0x0001U};
// Diag Test Server logical address
const std::uint16_t kDiagServerLogicalAddress{0xFA25U};
// Path to json file
constexpr std::string_view kDiagClientConfigPath{"diag_client_config.json"};
// Default routing activation type
constexpr std::uint8_t kDoipRoutingActivationReqActTypeDefault{0x00U};
// Successful routing activation response code
constexpr std::uint8_t kDoipRoutingActivationResCodeRoutingSuccessful{0x10U};
// Diagnostic Message positive acknowledgement code
constexpr std::uint8_t kDoipDiagnosticMessagePosAckCodeConfirm{0x00U};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeInvalidSa{0x02};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeUnknownTa{0x03};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeMessageTooLarge{0x04};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeOutOfMemory{0x05};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeTargetUnreachable{0x06};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeUnknownNetwork{0x07};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeTpError{0x08};

// Uds message implementation
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

// Helper class for auto connection and disconnection to server
class DiagConnectedConversation final {
 public:
  DiagConnectedConversation(diag::client::DiagClient& diag_client,
                            std::string_view conversation_name) noexcept
      : conversation_{diag_client.GetDiagnosticClientConversation(conversation_name)} {
    conversation_.Startup();
    // Start connecting to server
    EXPECT_EQ(conversation_.ConnectToDiagServer(kDiagServerLogicalAddress, kDiagTcpIpAddress),
              diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);
  }

  auto GetConversation() noexcept -> diag::client::conversation::DiagClientConversation& {
    return conversation_;
  }

  ~DiagConnectedConversation() noexcept {
    EXPECT_EQ(
        conversation_.DisconnectFromDiagServer(),
        diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);
    conversation_.Shutdown();
  }

 private:
  diag::client::conversation::DiagClientConversation conversation_;
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

class DiagMessageFixtureValueParameter : public DiagMessageFixture,
                                         public ::testing::WithParamInterface<std::uint8_t> {};

INSTANTIATE_TEST_SUITE_P(DiagMessage, DiagMessageFixtureValueParameter,
                         testing::Values(kDoipDiagnosticMessageNegAckCodeInvalidSa,
                                         kDoipDiagnosticMessageNegAckCodeUnknownTa,
                                         kDoipDiagnosticMessageNegAckCodeMessageTooLarge,
                                         kDoipDiagnosticMessageNegAckCodeOutOfMemory,
                                         kDoipDiagnosticMessageNegAckCodeTargetUnreachable,
                                         kDoipDiagnosticMessageNegAckCodeUnknownNetwork,
                                         kDoipDiagnosticMessageNegAckCodeTpError));

/**
 * @brief  Verify that sending of diagnostic request works correctly and response is received.
 */
TEST_F(DiagMessageFixture, VerifyDiagPositiveResponse) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};
  UdsMessage::ByteVector kDiagResponse{0x50, 0x01, 0x00, 0x32, 0x01, 0xF4};

  std::future<bool> is_server_created{
      CreateServerWithExpectation([this, &kDiagRequest, &kDiagResponse]() {
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

        EXPECT_CALL(*doip_tcp_handler_,
                    ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
            .WillOnce(::testing::Invoke([this, &kDiagRequest, &kDiagResponse](
                                            std::uint16_t client_source_address,
                                            std::uint16_t server_target_address,
                                            core_type::Span<std::uint8_t const> diag_request) {
              EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
              EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
              EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
              // Send Diagnostic Positive Acknowledgement message
              doip_tcp_handler_->SendTcpMessage(
                  common::handler::ComposeDiagnosticPositiveAcknowledgementMessage(
                      kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                      kDoipDiagnosticMessagePosAckCodeConfirm));
              // Send Diagnostic response message
              doip_tcp_handler_->SendTcpMessage(common::handler::ComposeDiagnosticResponseMessage(
                  kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                  core_type::Span<std::uint8_t const>{kDiagResponse}));
            }));
      })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_TRUE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Value()->GetPayload(), testing::ElementsAreArray(kDiagResponse));
}

/**
 * @brief  Verify that sending of diagnostic request works correctly when negative diagnostic response is received.
 */
TEST_F(DiagMessageFixture, VerifyDiagNegativeResponse) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};
  UdsMessage::ByteVector kDiagResponse{0x7F, 0x10, 0x10};

  std::future<bool> is_server_created{
      CreateServerWithExpectation([this, &kDiagRequest, &kDiagResponse]() {
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

        EXPECT_CALL(*doip_tcp_handler_,
                    ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
            .WillOnce(::testing::Invoke([this, &kDiagRequest, &kDiagResponse](
                                            std::uint16_t client_source_address,
                                            std::uint16_t server_target_address,
                                            core_type::Span<std::uint8_t const> diag_request) {
              EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
              EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
              EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
              // Send Diagnostic Positive Acknowledgement message
              doip_tcp_handler_->SendTcpMessage(
                  common::handler::ComposeDiagnosticPositiveAcknowledgementMessage(
                      kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                      kDoipDiagnosticMessagePosAckCodeConfirm));
              // Send Diagnostic response message
              doip_tcp_handler_->SendTcpMessage(common::handler::ComposeDiagnosticResponseMessage(
                  kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                  core_type::Span<std::uint8_t const>{kDiagResponse}));
            }));
      })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_TRUE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Value()->GetPayload(), testing::ElementsAreArray(kDiagResponse));
}

/**
 * @brief  Verify that sending of diagnostic request works correctly when pending diagnostic responses are received.
 */
TEST_F(DiagMessageFixture, VerifyDiagPendingResponse) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};
  UdsMessage::ByteVector kDiagPendingResponse{0x7F, 0x10, 0x78};
  UdsMessage::ByteVector kDiagFinalResponse{0x50, 0x01, 0x00, 0x32, 0x01, 0xF4};
  constexpr std::uint8_t kNumOfPending{10u};

  std::future<bool> is_server_created{CreateServerWithExpectation(
      [this, &kDiagRequest, &kDiagPendingResponse, &kDiagFinalResponse]() {
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

        EXPECT_CALL(*doip_tcp_handler_,
                    ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
            .WillOnce(::testing::Invoke([this, &kDiagRequest, &kDiagPendingResponse,
                                         &kDiagFinalResponse](
                                            std::uint16_t client_source_address,
                                            std::uint16_t server_target_address,
                                            core_type::Span<std::uint8_t const> diag_request) {
              EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
              EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
              EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
              // Send Diagnostic Positive Acknowledgement message
              doip_tcp_handler_->SendTcpMessage(
                  common::handler::ComposeDiagnosticPositiveAcknowledgementMessage(
                      kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                      kDoipDiagnosticMessagePosAckCodeConfirm));
              // Send Diagnostic pending response message
              for (std::uint8_t pending_count{0}; pending_count < kNumOfPending; pending_count++) {
                doip_tcp_handler_->SendTcpMessage(common::handler::ComposeDiagnosticResponseMessage(
                    kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                    core_type::Span<std::uint8_t const>{kDiagPendingResponse}));
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
              }
              // Send final diagnostic response message
              doip_tcp_handler_->SendTcpMessage(common::handler::ComposeDiagnosticResponseMessage(
                  kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                  core_type::Span<std::uint8_t const>{kDiagFinalResponse}));
            }));
      })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_TRUE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Value()->GetPayload(), testing::ElementsAreArray(kDiagFinalResponse));
}

/**
 * @brief  Verify that correct error is propagated in case of no diagnostic response received and timeout happens.
 */
TEST_F(DiagMessageFixture, VerifyDiagResponseTimeout) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};

  std::future<bool> is_server_created{CreateServerWithExpectation([this, &kDiagRequest]() {
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

    EXPECT_CALL(*doip_tcp_handler_,
                ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(::testing::Invoke(
            [this, &kDiagRequest](std::uint16_t client_source_address,
                                  std::uint16_t server_target_address,
                                  core_type::Span<std::uint8_t const> diag_request) {
              EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
              EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
              EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
              // Send Diagnostic Positive Acknowledgement message
              doip_tcp_handler_->SendTcpMessage(
                  common::handler::ComposeDiagnosticPositiveAcknowledgementMessage(
                      kDiagServerLogicalAddress, kDiagClientLogicalAddress,
                      kDoipDiagnosticMessagePosAckCodeConfirm));
            }));
  })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_FALSE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Error(),
              diag::client::conversation::DiagClientConversation::DiagError::kDiagResponseTimeout);
}

/**
 * @brief  Verify that correct error is propagated in case invalid parameter provided when sending diagnostic request.
 */
TEST_F(DiagMessageFixture, VerifyInvalidParameterInRequest) {
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

    // Expect no diagnostic request received
    EXPECT_CALL(*doip_tcp_handler_,
                ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
        .Times(0u);
  })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{nullptr};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_FALSE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Error(),
              diag::client::conversation::DiagClientConversation::DiagError::kDiagInvalidParameter);
}

/**
 * @brief  Verify that correct error is propagated in case of negative acknowledgement received.
 */
TEST_P(DiagMessageFixtureValueParameter, VerifyDiagNegativeAcknowledgement) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};

  std::future<bool> is_server_created{CreateServerWithExpectation([this, &kDiagRequest]() {
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

    EXPECT_CALL(*doip_tcp_handler_,
                ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(::testing::Invoke(
            [this, &kDiagRequest](std::uint16_t client_source_address,
                                  std::uint16_t server_target_address,
                                  core_type::Span<std::uint8_t const> diag_request) {
              EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
              EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
              EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
              // Send Diagnostic negative Acknowledgement message
              doip_tcp_handler_->SendTcpMessage(
                  common::handler::ComposeDiagnosticNegativeAcknowledgementMessage(
                      kDiagServerLogicalAddress, kDiagClientLogicalAddress, GetParam()));
            }));
  })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_FALSE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Error(),
              diag::client::conversation::DiagClientConversation::DiagError::kDiagNegAckReceived);
}

/**
 * @brief  Verify that correct error is propagated in case of no diagnostic acknowledgement received and timeout happen.
 */
TEST_F(DiagMessageFixture, VerifyDiagAcknowledgementTimeout) {
  UdsMessage::ByteVector kDiagRequest{0x10, 0x01};

  std::future<bool> is_server_created{CreateServerWithExpectation([this, &kDiagRequest]() {
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

    EXPECT_CALL(*doip_tcp_handler_,
                ProcessDiagnosticRequestMessage(testing::_, testing::_, testing::_))
        .WillOnce(
            ::testing::Invoke([&kDiagRequest](std::uint16_t client_source_address,
                                              std::uint16_t server_target_address,
                                              core_type::Span<std::uint8_t const> diag_request) {
              EXPECT_EQ(client_source_address, kDiagClientLogicalAddress);
              EXPECT_EQ(server_target_address, kDiagServerLogicalAddress);
              EXPECT_THAT(diag_request, testing::ElementsAreArray(kDiagRequest));
            }));
  })};

  DiagConnectedConversation diag_client_conversation{*diag_client_, "DiagTesterOne"};

  ASSERT_TRUE(is_server_created.get());

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(kDiagTcpIpAddress, kDiagRequest)};

  diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                       diag::client::conversation::DiagClientConversation::DiagError>
      diag_result{
          diag_client_conversation.GetConversation().SendDiagnosticRequest(std::move(uds_message))};

  ASSERT_FALSE(diag_result.HasValue());
  EXPECT_THAT(diag_result.Error(),
              diag::client::conversation::DiagClientConversation::DiagError::kDiagAckTimeout);
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
