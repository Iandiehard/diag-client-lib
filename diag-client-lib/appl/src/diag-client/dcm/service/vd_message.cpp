/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "diag-client/dcm/service/vd_message.h"

#include <sstream>
#include <string_view>

namespace diag {
namespace client {
namespace vd_message {

auto SerializeVehicleInfoList(std::uint8_t preselection_mode,
                              uds_transport::ByteVector& preselection_value) noexcept
    -> uds_transport::ByteVector {
  constexpr std::uint8_t VehicleIdentificationHandler{0U};

  uds_transport::ByteVector payload{VehicleIdentificationHandler, preselection_mode};
  payload.insert(payload.begin() + 2U, preselection_value.begin(), preselection_value.end());
  return payload;
}

VdMessage::VdMessage(std::uint8_t preselection_mode, uds_transport::ByteVector& preselection_value,
                     std::string_view host_ip_address)
    : uds_transport::UdsMessage(),
      source_address_{0U},
      target_address_{0U},
      target_address_type{TargetAddressType::kPhysical},
      host_ip_address_{host_ip_address},
      vehicle_info_payload_{SerializeVehicleInfoList(preselection_mode, preselection_value)} {}

VdMessage::VdMessage() noexcept
    : uds_transport::UdsMessage(),
      source_address_{0U},
      target_address_{0U},
      target_address_type{TargetAddressType::kPhysical} {}

}  // namespace vd_message
}  // namespace client
}  // namespace diag
