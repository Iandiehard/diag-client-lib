/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <gtest/gtest.h>

#include <chrono>

#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"
#include "main.h"

namespace doip_client {

TEST_F(DoipClientFixture, SendVehicleIdentification) {

  ara::diag::doip::DoipUdpHandler::VehicleAddrInfo vehicle_addr_response{0x0001U,
                                                                         "ABCDEFGH123456789",
                                                                         "010203040506",
                                                                         "0A0B0C0D0E0F"};
  // Create an expected vehicle identification response
  GetDoipTestUdpHandlerRef().SetExpectedVehicleIdentificationResponseToBeSent(vehicle_addr_response);

  diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request{0u, ""};
  std::pair<diag::client::DiagClient::VehicleResponseResult, diag::client::vehicle_info::VehicleInfoMessageResponsePtr>
      response_result{GetDiagClientRef().SendVehicleIdentificationRequest(vehicle_info_request)};

  ASSERT_EQ(response_result.first, diag::client::DiagClient::VehicleResponseResult::kStatusOk);
  ASSERT_TRUE(response_result.second);

  diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType response_collection{
      response_result.second->GetVehicleList()};

  EXPECT_EQ(response_collection.size(), 1U);
  EXPECT_EQ(response_collection[0].logical_address, vehicle_addr_response.logical_address);
  EXPECT_EQ(response_collection[0].vin, vehicle_addr_response.vin);
  EXPECT_EQ(response_collection[0].eid, vehicle_addr_response.eid);
  EXPECT_EQ(response_collection[0].gid, vehicle_addr_response.gid);
}

}  // namespace doip_client