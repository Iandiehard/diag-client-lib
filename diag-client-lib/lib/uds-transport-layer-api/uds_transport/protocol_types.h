/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_TYPES_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_TYPES_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace uds_transport {

// This is the type of ByteVector
using ByteVector = std::vector<std::uint8_t>;
// This is the type of Channel ID
using ChannelID = uint32_t;
// This is the type of Priority
using Priority = std::uint8_t;
// This is the type of Protocol Kind
using ProtocolKind = std::string_view;

namespace conversion_manager {
// Conversion identification needed by user
using ConversionHandlerID = std::uint8_t;
}  // namespace conversion_manager

}  // namespace uds_transport

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_TYPES_H
