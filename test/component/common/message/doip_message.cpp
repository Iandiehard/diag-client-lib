/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "common/message/doip_message.h"

namespace test {
namespace component {
namespace common {
namespace message {
namespace {

auto GetDoIPPayloadType(core_type::Span<std::uint8_t const> payload) noexcept -> std::uint16_t {
  return static_cast<std::uint16_t>(((payload[2u] << 8) & 0xFF00) | payload[3u]);
}

auto GetDoIPPayloadLength(core_type::Span<std::uint8_t const> payload) noexcept -> std::uint32_t {
  return ((static_cast<std::uint32_t>(payload[4u] << 24) & 0xFF000000) |
          (static_cast<std::uint32_t>(payload[5u] << 16) & 0x00FF0000) |
          (static_cast<std::uint32_t>(payload[6u] << 8) & 0x0000FF00) |
          (static_cast<std::uint32_t>(payload[7u] & 0x000000FF)));
}
}  // namespace

DoipMessage::DoipMessage(std::string_view host_ip_address, std::uint16_t host_port_number,
                         core_type::Span<std::uint8_t const> payload)
    : host_ip_address_{host_ip_address},
      host_port_number_{host_port_number},
      protocol_version_{},
      protocol_version_inv_{},
      payload_type_{},
      payload_length_{} {
  constexpr std::size_t kDoipHeaderSize{8u};
  // Extract header
  protocol_version_ = payload[0u];
  protocol_version_inv_ = payload[1u];
  payload_type_ = GetDoIPPayloadType(payload);
  payload_length_ = GetDoIPPayloadLength(payload);
  // Extract Payload
  payload_ = core_type::Span<std::uint8_t const>{&payload[kDoipHeaderSize], payload.size() - kDoipHeaderSize};
}

}  // namespace message
}  // namespace common
}  // namespace component
}  // namespace test
