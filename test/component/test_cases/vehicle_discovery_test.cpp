/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <gtest/gtest.h>

#include <future>
#include <string_view>
#include <thread>

#include "common/handler/doip_udp_handler.h"
#include "component_test.h"
#include "diag-client/create_diagnostic_client.h"
#include "diag-client/diagnostic_client.h"
#include "diag-client/diagnostic_client_vehicle_info_message_type.h"

namespace test {
namespace component {
namespace test_cases {
// Diag Test Server Unicast Udp Ip Address
constexpr std::string_view kDiagUdpUnicastIpAddress{"172.16.25.128"};
// Diag Test Server Broadcast Udp Ip Address
constexpr std::string_view kDiagUdpBroadCastIpAddress{"172.16.255.255"};
// Diag Test Server Unicast Udp Ip Address
constexpr std::string_view kDiagUdpAnotherUnicastIpAddress{"172.16.25.129"};
// Port number
constexpr std::uint16_t kDiagUdpPortNum{13400u};
// Path to json file
constexpr std::string_view kDiagClientConfigPath{"./etc/diag_client_config.json"};

// Fixture to test Vehicle discovery functionality
class VehicleDiscoveryFixture : public component::ComponentTest {
 protected:
  VehicleDiscoveryFixture()
      : doip_udp_handler_{kDiagUdpBroadCastIpAddress, kDiagUdpUnicastIpAddress, kDiagUdpPortNum},
        diag_client_{diag::client::CreateDiagnosticClient(kDiagClientConfigPath)} {}

  void SetUp() override {
    doip_udp_handler_.Initialize();
    ASSERT_TRUE(diag_client_->Initialize().HasValue());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void TearDown() override {
    diag_client_->DeInitialize();
    doip_udp_handler_.DeInitialize();
  }

 protected:
  // doip udp handler
  testing::StrictMock<common::handler::DoipUdpHandler> doip_udp_handler_;

  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;
};

/**
 * @brief  Verify that sending of vehicle identification request without VIN/EID works correctly.
 */
TEST_F(VehicleDiscoveryFixture, VerifyPreselectionModeEmpty) {
  constexpr std::string_view kVin{"ABCDEFGH123456789"};
  constexpr std::string_view kEid{"00:02:36:31:00:1c"};
  constexpr std::string_view kGid{"0a:0b:0c:0d:0e:0f"};
  std::uint16_t const kLogicalAddress{0xFA25u};

  // Create an expectation of vehicle identification response
  EXPECT_CALL(doip_udp_handler_, ProcessVehicleIdentificationRequestMessage(testing::_, testing::_,
                                                                            testing::_, testing::_))
      .WillOnce(
          ::testing::Invoke([this, kVin, kEid, kGid](std::string_view client_ip_address,
                                                     std::uint16_t client_port_number,
                                                     std::string_view eid, std::string_view vin) {
            EXPECT_TRUE(eid.empty());
            EXPECT_TRUE(vin.empty());
            // Send Vehicle Identification response
            doip_udp_handler_.SendUdpMessage(doip_udp_handler_.ComposeVehicleIdentificationResponse(
                client_ip_address, client_port_number, kVin, kLogicalAddress, kEid, kGid, 0,
                std::nullopt));
          }));

  // Send Vehicle Identification request and expect response
  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{0u, ""};
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseError>
      response{diag_client_->SendVehicleIdentificationRequest(std::move(vehicle_info_request))};
  // Verify Vehicle identification responses received successfully
  ASSERT_TRUE(response.HasValue());

  // Get the list of all vehicle available
  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType const
      response_collection{response.Value()->GetVehicleList()};

  // Expect only one vehicle available
  EXPECT_EQ(response_collection.size(), 1U);
  EXPECT_EQ(response_collection[0].ip_address, kDiagUdpUnicastIpAddress);
  EXPECT_EQ(response_collection[0].logical_address, kLogicalAddress);
  EXPECT_EQ(response_collection[0].vin, kVin);
  EXPECT_EQ(response_collection[0].eid, kEid);
  EXPECT_EQ(response_collection[0].gid, kGid);
}

/**
 * @brief  Verify that sending of vehicle identification request with VIN works correctly.
 */
TEST_F(VehicleDiscoveryFixture, VerifyPreselectionModeVin) {
  constexpr std::string_view kVin{"ABCDEFGH123456789"};
  constexpr std::string_view kEid{"00:02:36:31:00:1c"};
  constexpr std::string_view kGid{"0a:0b:0c:0d:0e:0f"};
  std::uint16_t const kLogicalAddress{0xFA25u};

  // Create an expectation of vehicle identification response
  EXPECT_CALL(doip_udp_handler_, ProcessVehicleIdentificationRequestMessage(testing::_, testing::_,
                                                                            testing::_, testing::_))
      .WillOnce(
          ::testing::Invoke([this, kVin, kEid, kGid](std::string_view client_ip_address,
                                                     std::uint16_t client_port_number,
                                                     std::string_view eid, std::string_view vin) {
            EXPECT_TRUE(eid.empty());
            EXPECT_EQ(vin, kVin);
            // Send Vehicle Identification response
            doip_udp_handler_.SendUdpMessage(doip_udp_handler_.ComposeVehicleIdentificationResponse(
                client_ip_address, client_port_number, kVin, kLogicalAddress, kEid, kGid, 0,
                std::nullopt));
          }));

  // Send Vehicle Identification request and expect response
  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{1u,
                                                                              std::string{kVin}};
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseError>
      response{diag_client_->SendVehicleIdentificationRequest(std::move(vehicle_info_request))};
  // Verify Vehicle identification responses received successfully
  ASSERT_TRUE(response.HasValue());

  // Get the list of all vehicle available
  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType const
      response_collection{response.Value()->GetVehicleList()};

  // Expect only one vehicle available
  EXPECT_EQ(response_collection.size(), 1U);
  EXPECT_EQ(response_collection[0].ip_address, kDiagUdpUnicastIpAddress);
  EXPECT_EQ(response_collection[0].logical_address, kLogicalAddress);
  EXPECT_EQ(response_collection[0].vin, kVin);
  EXPECT_EQ(response_collection[0].eid, kEid);
  EXPECT_EQ(response_collection[0].gid, kGid);
}

/**
 * @brief  Verify that sending of vehicle identification request with EID works correctly.
 */
TEST_F(VehicleDiscoveryFixture, VerifyPreselectionModeEid) {
  constexpr std::string_view kVin{"ABCDEFGH123456789"};
  constexpr std::string_view kEid{"00:02:36:31:00:1c"};
  constexpr std::string_view kGid{"0a:0b:0c:0d:0e:0f"};
  std::uint16_t const kLogicalAddress{0xFA25u};

  // Create an expectation of vehicle identification response
  EXPECT_CALL(doip_udp_handler_, ProcessVehicleIdentificationRequestMessage(testing::_, testing::_,
                                                                            testing::_, testing::_))
      .WillOnce(
          ::testing::Invoke([this, kVin, kEid, kGid](std::string_view client_ip_address,
                                                     std::uint16_t client_port_number,
                                                     std::string_view eid, std::string_view vin) {
            EXPECT_TRUE(vin.empty());
            EXPECT_EQ(eid, kEid);
            // Send Vehicle Identification response
            doip_udp_handler_.SendUdpMessage(doip_udp_handler_.ComposeVehicleIdentificationResponse(
                client_ip_address, client_port_number, kVin, kLogicalAddress, kEid, kGid, 0,
                std::nullopt));
          }));

  // Send Vehicle Identification request and expect response
  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{2u,
                                                                              std::string{kEid}};
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseError>
      response{diag_client_->SendVehicleIdentificationRequest(std::move(vehicle_info_request))};
  // Verify Vehicle identification responses received successfully
  ASSERT_TRUE(response.HasValue());

  // Get the list of all vehicle available
  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType const
      response_collection{response.Value()->GetVehicleList()};

  // Expect only one vehicle available
  EXPECT_EQ(response_collection.size(), 1U);
  EXPECT_EQ(response_collection[0].ip_address, kDiagUdpUnicastIpAddress);
  EXPECT_EQ(response_collection[0].logical_address, kLogicalAddress);
  EXPECT_EQ(response_collection[0].vin, kVin);
  EXPECT_EQ(response_collection[0].eid, kEid);
  EXPECT_EQ(response_collection[0].gid, kGid);
}

// Fixture to test Vehicle discovery functionality
class MultipleVehicleDiscoveryFixture : public component::ComponentTest {
 protected:
  MultipleVehicleDiscoveryFixture()
      : first_doip_udp_handler_{kDiagUdpBroadCastIpAddress, kDiagUdpUnicastIpAddress,
                                kDiagUdpPortNum},
        second_doip_udp_handler_{kDiagUdpBroadCastIpAddress, kDiagUdpAnotherUnicastIpAddress,
                                 kDiagUdpPortNum},
        diag_client_{diag::client::CreateDiagnosticClient(kDiagClientConfigPath)} {}

  void SetUp() override {
    first_doip_udp_handler_.Initialize();
    second_doip_udp_handler_.Initialize();
    ASSERT_TRUE(diag_client_->Initialize().HasValue());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void TearDown() override {
    diag_client_->DeInitialize();
    first_doip_udp_handler_.DeInitialize();
    second_doip_udp_handler_.DeInitialize();
  }

 protected:
  // doip udp handler
  testing::StrictMock<common::handler::DoipUdpHandler> first_doip_udp_handler_;

  // doip udp handler
  testing::StrictMock<common::handler::DoipUdpHandler> second_doip_udp_handler_;

  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;
};

/**
 * @brief  Verify that sending of vehicle identification request to multiple vehicle works correctly.
 */
TEST_F(MultipleVehicleDiscoveryFixture, VerifyPreselectionModeVin) {
  struct VehicleIdentificationResponse {
    std::string_view vin;
    std::string_view eid;
    std::string_view gid;
    std::string_view ip_address;
    std::uint16_t logical_address;
  };

  std::vector<VehicleIdentificationResponse> const kExpectedResponse{
      {"ABCDEFGH123456789", "00:02:36:31:00:1c", "0a:0b:0c:0d:0e:0f", kDiagUdpUnicastIpAddress,
       0xFA25u},
      {"IJKLMNOP123456789", "00:02:36:32:00:1d", "0a:0b:0c:0d:0e:0f",
       kDiagUdpAnotherUnicastIpAddress, 0xFA26u}};

  // This is needed so that first doip handler always sends the response first
  std::promise<bool> is_response_sent_promise{};
  std::future<bool> is_response_sent_future{is_response_sent_promise.get_future()};
  // Create an expectation of vehicle identification response
  EXPECT_CALL(first_doip_udp_handler_, ProcessVehicleIdentificationRequestMessage(
                                           testing::_, testing::_, testing::_, testing::_))
      .WillOnce(::testing::Invoke([this, &kExpectedResponse, &is_response_sent_promise](
                                      std::string_view client_ip_address,
                                      std::uint16_t client_port_number, std::string_view eid,
                                      std::string_view vin) {
        EXPECT_TRUE(eid.empty());
        EXPECT_TRUE(vin.empty());
        // Send Vehicle Identification response
        first_doip_udp_handler_.SendUdpMessage(
            first_doip_udp_handler_.ComposeVehicleIdentificationResponse(
                client_ip_address, client_port_number, kExpectedResponse.at(0u).vin,
                kExpectedResponse.at(0u).logical_address, kExpectedResponse.at(0u).eid,
                kExpectedResponse.at(0u).gid, 0, std::nullopt));
        is_response_sent_promise.set_value(true);
      }));

  EXPECT_CALL(second_doip_udp_handler_, ProcessVehicleIdentificationRequestMessage(
                                            testing::_, testing::_, testing::_, testing::_))
      .WillOnce(::testing::Invoke([this, &kExpectedResponse, &is_response_sent_future](
                                      std::string_view client_ip_address,
                                      std::uint16_t client_port_number, std::string_view eid,
                                      std::string_view vin) {
        EXPECT_TRUE(eid.empty());
        EXPECT_TRUE(vin.empty());
        if (is_response_sent_future.get()) {
          // Delay by 1 sec before sending second vehicle identification response, this is done to achieve consistency while
          // sending the response
          std::this_thread::sleep_for(std::chrono::seconds(1));
          // Send Vehicle Identification response
          second_doip_udp_handler_.SendUdpMessage(
              second_doip_udp_handler_.ComposeVehicleIdentificationResponse(
                  client_ip_address, client_port_number, kExpectedResponse.at(1u).vin,
                  kExpectedResponse.at(1u).logical_address, kExpectedResponse.at(1u).eid,
                  kExpectedResponse.at(1u).gid, 0, std::nullopt));
        }
      }));

  // Send Vehicle Identification request and expect response
  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{0u, ""};
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseError>
      response{diag_client_->SendVehicleIdentificationRequest(std::move(vehicle_info_request))};
  // Verify Vehicle identification responses received successfully
  ASSERT_TRUE(response.HasValue());

  // Get the list of all vehicle available
  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType const
      response_collection{response.Value()->GetVehicleList()};

  // Expect two vehicle available
  EXPECT_EQ(response_collection.size(), kExpectedResponse.size());
  // Verify the payload from two vehicle
  std::uint8_t vehicle_number{0u};
  for (VehicleIdentificationResponse const &expected_response: kExpectedResponse) {

    EXPECT_EQ(response_collection[vehicle_number].ip_address, expected_response.ip_address);
    EXPECT_EQ(response_collection[vehicle_number].logical_address,
              expected_response.logical_address);
    EXPECT_EQ(response_collection[vehicle_number].vin, expected_response.vin);
    EXPECT_EQ(response_collection[vehicle_number].eid, expected_response.eid);
    EXPECT_EQ(response_collection[vehicle_number].gid, expected_response.gid);
    vehicle_number++;
  }
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
