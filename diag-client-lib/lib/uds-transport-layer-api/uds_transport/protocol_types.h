/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_TYPES_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_TYPES_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace uds_transport {

// This is the type of ByteVector
using ByteVector = std::vector<std::uint8_t>;
// This is the type of Channel Id
using ChannelID = uint32_t;
// This is the type of Priority
using Priority = std::uint8_t;
// This is the type of Protocol Kind
using ProtocolKind = std::string;
// UdsTransportProtocolHandler are flexible "plugins", which need an identification
using UdsTransportProtocolHandlerID = std::uint8_t;

namespace conversion_manager {
// Conversion identification needed by user
using ConversionHandlerID = std::uint8_t;

// Conversion identifier Type can be used by user
struct ConversionIdentifierType {
  // Reception buffer
  uint32_t rx_buffer_size{};
  // p2 client time
  uint16_t p2_client_max{};
  // p2 star Client time
  uint16_t p2_star_client_max{};
  // source address of client
  uint16_t source_address{};
  // self tcp address
  std::string tcp_address{};
  // self udp address
  std::string udp_address{};
  // Vehicle broadcast address
  std::string udp_broadcast_address{};
  // Port Number
  uint16_t port_num{};
  // conversion handler ID
  ConversionHandlerID handler_id{};
};
}  // namespace conversion_manager

}  // namespace uds_transport

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_TYPES_H
