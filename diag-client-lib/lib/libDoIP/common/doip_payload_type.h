/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_DOIP_PAYLOAD_TYPE_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_DOIP_PAYLOAD_TYPE_H

#include <cstdint>
#include <vector>

class DoipMessage {
public:
    // ctor
    DoipMessage() = default;

    DoipMessage (const DoipMessage &other)=default;
    DoipMessage (DoipMessage &&other) noexcept=default;
    DoipMessage& operator= (const DoipMessage &other) = default;
    DoipMessage& operator= (DoipMessage &&other) noexcept = default;

    // doip protocol version
    uint8_t protocol_version{};

    // doip protocol inverse version
    uint8_t protocol_version_inv{};

    // doip payload type
    uint16_t payload_type{};

    // doip payload length
    uint32_t payload_length{};

    // doip payload
    std::vector<uint8_t> payload;
};


#endif //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_DOIP_PAYLOAD_TYPE_H
