/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "dm_uds_message.h"

namespace diag {
namespace client {
namespace uds_message {
DmUdsMessage::DmUdsMessage(Address sa, Address ta, IpAddress host_ip_address,
                           uds_transport::ByteVector &payload)
    : uds_transport::UdsMessage(),
      source_address_{sa},
      target_address_{ta},
      target_address_type_{TargetAddressType::kPhysical},
      host_ip_address_{host_ip_address},
      uds_payload_{payload} {}

DmUdsResponse::DmUdsResponse(ByteVector &payload) : uds_payload_{payload} {}

}  // namespace uds_message
}  // namespace client
}  // namespace diag