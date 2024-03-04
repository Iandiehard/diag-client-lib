/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <gtest/gtest.h>

#include <string_view>
#include <thread>

#include "common/handler/doip_udp_handler.h"
#include "component_test.h"
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"

namespace test {
namespace component {
namespace test_cases {
// Diag Test Server Unicast Udp Ip Address
constexpr std::string_view kDiagUdpUnicastIpAddress{"172.16.25.128"};
// Diag Test Server Broadcast Udp Ip Address
constexpr std::string_view kDiagUdpBroadCastIpAddress{"172.16.255.255"};
// Port number
constexpr std::uint16_t kDiagUdpPortNum{13400u};
// Path to json file
constexpr std::string_view kDiagClientConfigPath{"diag_client_config.json"};

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
  EXPECT_CALL(doip_udp_handler_,
              ProcessVehicleIdentificationRequestMessage(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(::testing::Invoke([this, kVin, kEid, kGid](std::string_view client_ip_address,
                                                           std::uint16_t client_port_number, std::string_view eid,
                                                           std::string_view vin) {
        EXPECT_TRUE(eid.empty());
        EXPECT_TRUE(vin.empty());
        // Send Vehicle Identification response
        doip_udp_handler_.SendUdpMessage(doip_udp_handler_.ComposeVehileIdentificationResponse(
            client_ip_address, client_port_number, kVin, kLogicalAddress, kEid, kGid, 0, std::nullopt));
      }));

  // Send Vehicle Identification request and expect response
  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{0u, ""};
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseError>
      response{diag_client_->SendVehicleIdentificationRequest(std::move(vehicle_info_request))};
  // Verify Vehicle identification responses received successfully
  ASSERT_TRUE(response.HasValue());

  // Get the list of all vehicle available
  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType const response_collection{
      response.Value()->GetVehicleList()};

  // Expect only one vehicle available
  EXPECT_EQ(response_collection.size(), 1U);
  EXPECT_EQ(response_collection[0].ip_address, kDiagUdpUnicastIpAddress);
  EXPECT_EQ(response_collection[0].logical_address, kLogicalAddress);
  EXPECT_EQ(response_collection[0].vin, kVin);
  EXPECT_EQ(response_collection[0].eid, kEid);
  EXPECT_EQ(response_collection[0].gid, kGid);
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
