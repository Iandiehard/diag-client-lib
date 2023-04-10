/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <string_view>
#include <sstream>

#include "src/dcm/service/vd_message.h"

namespace diag {
namespace client {
namespace vd_message {

auto SerializeVehicleInfoList(
    vehicle_info::VehicleInfoListRequestType& vehicle_info_request)
    noexcept -> ara::diag::uds_transport::ByteVector {
  ara::diag::uds_transport::ByteVector payload{0U, vehicle_info_request.preselection_mode};
  std::stringstream preselection_value{vehicle_info_request.preselection_value};
  
  for (std::uint8_t char_count{0U};
       char_count < static_cast<std::uint8_t>(vehicle_info_request.preselection_value.length()); char_count++) {
    payload.emplace_back(static_cast<std::uint8_t>(preselection_value.get()));
  }
  return payload;
}

VdMessage::VdMessage(vehicle_info::VehicleInfoListRequestType vehicle_info_request, std::string_view host_ip_address)
    : ara::diag::uds_transport::UdsMessage(),
      source_address_{0U},
      target_address_{0U},
      target_address_type{TargetAddressType::kPhysical},
      host_ip_address_{host_ip_address},
      vehicle_info_payload_{SerializeVehicleInfoList(vehicle_info_request)} {}

VdMessage::VdMessage() noexcept
    : ara::diag::uds_transport::UdsMessage(),
      source_address_{0U},
      target_address_{0U},
      target_address_type{TargetAddressType::kPhysical} {}

}  // namespace vd_message
}  // namespace client
}  // namespace diag
