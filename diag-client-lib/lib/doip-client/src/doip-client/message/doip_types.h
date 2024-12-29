/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOIP_CLIENT_MESSAGE_DOIP_TYPES_H_
#define DOIP_CLIENT_MESSAGE_DOIP_TYPES_H_

#include <cstdint>

namespace doip_client {
namespace message {

/* DoIP Protocol versions */
constexpr std::uint8_t kDoipProtocolVersion2012{2u};                    // ISO 13400-2012
constexpr std::uint8_t kDoipProtocolVersion2019{3u};                    // ISO 13400-2019
constexpr std::uint8_t kDoipProtocolVersionDefault{0xFFu};              // Default version
constexpr std::uint8_t kDoipProtocolVersion{kDoipProtocolVersion2019};  // Selected version

/* DoIP header size */
constexpr std::uint8_t kDoipHeaderSize{8u};

/* DoIP max payload size */
constexpr std::uint32_t kDoipProtocolMaxPayload{0xFFFFFFFF};  // 4294967295 bytes

/* Tcp Channel Length */
constexpr std::uint32_t kTcpChannelLength{4096u};

/* Udp Channel Length */
constexpr std::uint32_t kUdpChannelLength{41u};

/* DoIP timing and communication parameter (in millisecond) */
/* Description: This timeout specifies the maximum time that the
                client waits for response to a previously sent UDP message.
                This includes max time to wait and collect multiple responses
                to previous broadcast(UDP only)
 */
constexpr std::uint32_t kDoIPCtrl{2000u};  // 2 sec

}  // namespace message
}  // namespace doip_client

#endif  // DOIP_CLIENT_MESSAGE_DOIP_TYPES_H_
