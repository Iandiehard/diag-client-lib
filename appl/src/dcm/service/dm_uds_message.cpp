/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dcm/service/dm_uds_message.h"

namespace diag{
namespace client{
namespace uds_message{

DmUdsMessage::DmUdsMessage(Address sa
                        , Address ta
                        , IpAddress hostipaddress
                        , ara::diag::uds_transport::ByteVector& payload) 
                        : ara::diag::uds_transport::UdsMessage()
                        , source_address(sa)
                        , target_address(ta)
                        , host_ip_address(hostipaddress)
                        , uds_payload(payload) {
}

DmUdsMessage::~DmUdsMessage () {
}

DmUdsResponse::DmUdsResponse(ByteVector& payload)
                            : uds_payload(payload) {
}

DmUdsResponse::~DmUdsResponse () {    
}

} // uds_message
} // client
} // diag