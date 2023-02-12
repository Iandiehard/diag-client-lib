/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <sstream>
#include <string>
#include "src/dcm/service/vd_message.h"

namespace diag {
namespace client {
namespace vd_message {
VdMessage::VdMessage(vehicle_info::VehicleInfoListRequestType vehicle_info_request,
                     IpAddress host_ip_address)
  : ara::diag::uds_transport::UdsMessage(),
    source_address_{0U},
    target_address_{0U},
    target_address_type{TargetAddressType::kPhysical},
    host_ip_address_{host_ip_address},
    vehicle_info_payload_{SerializeVehicleInfoList(vehicle_info_request)} {
}

VdMessage::VdMessage() noexcept
  : ara::diag::uds_transport::UdsMessage(),
    source_address_{0U},
    target_address_{0U},
    target_address_type{TargetAddressType::kPhysical} {
}

ara::diag::uds_transport::ByteVector
VdMessage::SerializeVehicleInfoList(vehicle_info::VehicleInfoListRequestType vehicle_info_request) {
  ara::diag::uds_transport::ByteVector payload{0U, vehicle_info_request.preselection_mode};
  std::stringstream preselection_value{vehicle_info_request.preselection_value};
  
  for (auto count = 0U; count < vehicle_info_request.preselection_value.length(); count++) {
    std::string each_byte{static_cast<char>(preselection_value.get())};
    payload.emplace_back(std::stoi(each_byte));
  }
  return payload;
}

}  // namespace vd_message
}  // namespace client
}  // namespace diag