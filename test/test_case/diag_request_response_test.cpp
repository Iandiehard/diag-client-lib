/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include "doip_handler/common_doip_types.h"
#include "doip_handler/doip_tcp_handler.h"
#include "doip_handler/doip_udp_handler.h"
#include "doip_handler/logger.h"
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_uds_message_type.h"

namespace doip_client {
namespace {

using doip_handler::DoipTcpHandler;
using doip_handler::DoipUdpHandler;

// Diag Test Server Udp Ip Address
const std::string DiagUdpIpAddress{"172.16.25.128"};

// Diag Test Server Tcp Ip Address
const std::string DiagTcpIpAddress{"172.16.25.128"};

// Diag Test Server logical address
const std::uint16_t DiagServerLogicalAddress{0xFA25U};

// Diag Second Test Server logical address
const std::uint16_t DiagSecondServerLogicalAddress{0xFA26U};

// Port number
constexpr std::uint16_t DiagUdpPortNum{13400u};
constexpr std::uint16_t DiagTcpPortNum{13400u};

// Path to json file
const std::string DiagClientJsonPath{"../../diag-client-lib/appl/etc/diag_client_config.json"};

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

class DiagReqResFixture : public ::testing::Test {
protected:
  DiagReqResFixture()
      : diag_client_{diag::client::CreateDiagnosticClient(DiagClientJsonPath)},
        doip_udp_handler_{DiagUdpIpAddress, DiagUdpPortNum},
        doip_tcp_handler_{DiagTcpIpAddress, DiagTcpPortNum} {
    // Initialize logger
    doip_handler::logger::LibGtestLogger::GetLibGtestLogger();
    // Initialize doip test handler
    doip_udp_handler_.Initialize();
    // Initialize diag client library
    diag_client_->Initialize();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  ~DiagReqResFixture() override {
    // De-initialize diag client library
    diag_client_->DeInitialize();
    // De-initialize doip test handler
    doip_udp_handler_.DeInitialize();
  }

  void SetUp() override {}

  void TearDown() override {}

  // Function to get Diag client library reference
  auto GetDiagClientRef() noexcept -> diag::client::DiagClient& { return *diag_client_; }

  // Function to get Doip Test Handler reference
  auto GetDoipTestUdpHandlerRef() noexcept -> DoipUdpHandler& { return doip_udp_handler_; }

  // Function to get Doip Tcp Test Handler reference
  auto GetDoipTestTcpHandlerRef() noexcept -> DoipTcpHandler& { return doip_tcp_handler_; }

private:
  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;

  // doip udp test handler
  DoipUdpHandler doip_udp_handler_;

  // doip tcp test handler
  DoipTcpHandler doip_tcp_handler_;
};

}  // namespace

TEST_F(DiagReqResFixture, VerifyRoutingActivationSuccessful) {
  // Get the doip channel and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel{GetDoipTestTcpHandlerRef().CreateDoipChannel(DiagServerLogicalAddress)};
  doip_channel.Initialize();

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(DiagServerLogicalAddress, DiagTcpIpAddress)};

  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

TEST_F(DiagReqResFixture, VerifyRoutingActivationFailure) {
  // Get the doip channel and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel{GetDoipTestTcpHandlerRef().CreateDoipChannel(0xFA25U)};
  doip_channel.Initialize();

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Set expectation code - RoutingSuccess
  doip_channel.SetExpectedRoutingActivationResponseToBeSent(kDoip_RoutingActivation_ResCode_UnknownSA);

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(DiagServerLogicalAddress, DiagTcpIpAddress)};

  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectFailed);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

TEST_F(DiagReqResFixture, VerifyDiagPositiveResponse) {
  // Get the doip channel and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel{GetDoipTestTcpHandlerRef().CreateDoipChannel(0xFA25U)};
  doip_channel.Initialize();

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(DiagTcpIpAddress, UdsMessage::ByteVector{0x10, 0x01})};
  // Create expected uds response
  doip_channel.SetExpectedDiagnosticMessageUdsMessageToBeSend(UdsMessage::ByteVector{0x50, 0x01});

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(DiagServerLogicalAddress, uds_message->GetHostIpAddress())};

  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  // Send Diagnostic message
  auto diag_result{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

  // Verify positive response
  EXPECT_TRUE(diag_result.HasValue());
  EXPECT_EQ(diag_result.Value()->GetPayload()[0], 0x50);
  EXPECT_EQ(diag_result.Value()->GetPayload()[1], 0x01);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

TEST_F(DiagReqResFixture, VerifyDiagPendingResponse) {
  // Get the doip channel and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel{GetDoipTestTcpHandlerRef().CreateDoipChannel(0xFA25U)};
  doip_channel.Initialize();

  // Create expected uds pending response
  doip_channel.SetExpectedDiagnosticMessageWithPendingUdsMessageToBeSend(UdsMessage::ByteVector{0x7F, 0x10, 0x78}, 10u);

  // Create expected uds positive response
  doip_channel.SetExpectedDiagnosticMessageUdsMessageToBeSend(UdsMessage::ByteVector{0x50, 0x01});

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(DiagTcpIpAddress, UdsMessage::ByteVector{0x10, 0x01})};

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(DiagServerLogicalAddress, uds_message->GetHostIpAddress())};

  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  // Send Diagnostic message
  auto diag_result{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

  // Verify positive response
  EXPECT_TRUE(diag_result.HasValue());
  EXPECT_EQ(diag_result.Value()->GetPayload()[0], 0x50);
  EXPECT_EQ(diag_result.Value()->GetPayload()[1], 0x01);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

TEST_F(DiagReqResFixture, VerifyDiagNegAcknowledgement) {
  // Get the doip channel and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel{GetDoipTestTcpHandlerRef().CreateDoipChannel(0xFA25U)};
  doip_channel.Initialize();

  // Create uds message
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(DiagTcpIpAddress, UdsMessage::ByteVector{0x10, 0x01})};
  // Create expected Diag NACK
  doip_channel.SetExpectedDiagnosticMessageAckResponseToBeSend(kDoip_DiagnosticMessage_NegAckCode_InvalidSA);

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(DiagServerLogicalAddress, uds_message->GetHostIpAddress())};

  EXPECT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  // Send Diagnostic message
  auto diag_result{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

  // Verify Negative Acknowledgement
  EXPECT_FALSE(diag_result.HasValue());
  EXPECT_EQ(diag_result.Error(), diag::client::conversation::DiagClientConversation::DiagError::kDiagNegAckReceived);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

TEST_F(DiagReqResFixture, VerifyDiagReqResponseWithVehicleDiscovery) {
  // Test preparation
  // ========================================================
  doip_handler::DoipUdpHandler::VehicleAddrInfo vehicle_addr_response{DiagServerLogicalAddress, "ABCDEFGH123456789",
                                                                      "00:02:36:31:00:1c", "0a:0b:0c:0d:0e:0f"};
  // Get the doip channel and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel{GetDoipTestTcpHandlerRef().CreateDoipChannel(DiagServerLogicalAddress)};
  doip_channel.Initialize();

  // Create an expected vehicle identification response
  GetDoipTestUdpHandlerRef().SetExpectedVehicleIdentificationResponseToBeSent(vehicle_addr_response);
  // Create expected uds response
  doip_channel.SetExpectedDiagnosticMessageUdsMessageToBeSend(UdsMessage::ByteVector{0x50, 0x01});

  // Test execution
  // ========================================================
  // Send Vehicle Identification request with VIN and expect response
  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{1U, "ABCDEFGH123456789"};
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseErrorCode>
      response_result{GetDiagClientRef().SendVehicleIdentificationRequest(vehicle_info_request)};

  // Verify Vehicle identification responses
  EXPECT_TRUE(response_result.HasValue());
  EXPECT_TRUE(response_result.Value());

  // Get the list of all vehicle available
  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType response_collection{
      response_result.Value()->GetVehicleList()};

  // Create uds message using response collection
  diag::client::uds_message::UdsRequestMessagePtr uds_message{
      std::make_unique<UdsMessage>(response_collection[0].ip_address, UdsMessage::ByteVector{0x10, 0x01})};

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
      diag_client_conversation.ConnectToDiagServer(response_collection[0].logical_address,
                                                   uds_message->GetHostIpAddress())};

  ASSERT_EQ(connect_result, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  // Send Diagnostic message
  auto diag_result{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

  // Verify positive response
  EXPECT_TRUE(diag_result.HasValue());
  EXPECT_EQ(diag_result.Value()->GetPayload()[0], 0x50);
  EXPECT_EQ(diag_result.Value()->GetPayload()[1], 0x01);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result{
      diag_client_conversation.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation.Shutdown();
  doip_channel.DeInitialize();
}

TEST_F(DiagReqResFixture, VerifyTwoDiagClientConnection) {
  // Get the doip channels and Initialize it
  DoipTcpHandler::DoipChannel& doip_channel_1{GetDoipTestTcpHandlerRef().CreateDoipChannel(DiagServerLogicalAddress)};
  DoipTcpHandler::DoipChannel& doip_channel_2{
      GetDoipTestTcpHandlerRef().CreateDoipChannel(DiagSecondServerLogicalAddress)};
  doip_channel_1.Initialize();
  doip_channel_2.Initialize();

  // Get conversation for tester one and start up the conversation
  diag::client::conversation::DiagClientConversation diag_client_conversation_1{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  diag_client_conversation_1.Startup();

  diag::client::conversation::DiagClientConversation diag_client_conversation_2{
      GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterTwo")};
  diag_client_conversation_2.Startup();

  // Connect Tester One to remote ip address 172.16.25.128
  diag::client::conversation::DiagClientConversation::ConnectResult connect_result_1{
      diag_client_conversation_1.ConnectToDiagServer(DiagServerLogicalAddress, DiagTcpIpAddress)};

  diag::client::conversation::DiagClientConversation::ConnectResult connect_result_2{
      diag_client_conversation_2.ConnectToDiagServer(DiagSecondServerLogicalAddress, DiagTcpIpAddress)};

  EXPECT_EQ(connect_result_1, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  EXPECT_EQ(connect_result_2, diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess);

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result_1{
      diag_client_conversation_1.DisconnectFromDiagServer()};

  diag::client::conversation::DiagClientConversation::DisconnectResult disconnect_result_2{
      diag_client_conversation_2.DisconnectFromDiagServer()};

  EXPECT_EQ(disconnect_result_1,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  EXPECT_EQ(disconnect_result_2,
            diag::client::conversation::DiagClientConversation::DisconnectResult::kDisconnectSuccess);

  diag_client_conversation_1.Shutdown();
  diag_client_conversation_2.Shutdown();
  doip_channel_1.DeInitialize();
  doip_channel_2.DeInitialize();
}

}  // namespace doip_client