/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "src/dcm/service/dm_uds_message.h"

namespace diag {
namespace client {
namespace uds_message {
DmUdsMessage::DmUdsMessage(Address sa, Address ta, IpAddress host_ip_address,
                           ara::diag::uds_transport::ByteVector &payload)
  : ara::diag::uds_transport::UdsMessage(),
    source_address(sa),
    target_address(ta),
    host_ip_address(host_ip_address),
    uds_payload(payload) {
}

DmUdsResponse::DmUdsResponse(ByteVector &payload)
  : uds_payload(payload) {
}

DmUdsResponse::~DmUdsResponse() {
}
}  // namespace uds_message
}  // namespace client
}  // namespace diag