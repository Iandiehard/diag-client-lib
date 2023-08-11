/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "common/doip_message.h"

#include "common/common_doip_types.h"

namespace doip_client {
namespace {

auto GetDoIPPayloadType(core_type::Span<std::uint8_t> payload) noexcept -> std::uint16_t {
  return ((std::uint16_t)(((payload[2u] & 0xFF) << 8) | (payload[3u] & 0xFF)));
}

auto GetDoIPPayloadLength(core_type::Span<std::uint8_t> payload) noexcept -> std::uint32_t {
  return ((std::uint32_t)((payload[4u] << 24) & 0xFF000000) | (std::uint32_t)((payload[5u] << 16) & 0x00FF0000) |
          (std::uint32_t)((payload[6u] << 8) & 0x0000FF00) | (std::uint32_t)((payload[7u] & 0x000000FF)));
}

auto GetServerAddr(core_type::Span<std::uint8_t> payload) noexcept -> std::uint16_t {
  return (static_cast<std::uint16_t>(((payload[0u] & 0xFF) << 8) | (payload[1u] & 0xFF)));
}

auto GetClientAddr(core_type::Span<std::uint8_t> payload) noexcept -> std::uint16_t {
  return (static_cast<std::uint16_t>(((payload[2u] & 0xFF) << 8) | (payload[3u] & 0xFF)));
}

}  // namespace

DoipMessage::DoipMessage(MessageType message_type, DoipMessage::IpAddressType host_ip_address,
                         std::uint16_t host_port_number, core_type::Span<std::uint8_t> payload)
    : message_type_{message_type},
      host_ip_address_{host_ip_address},
      host_port_number_{host_port_number},
      protocol_version_{payload[0u]},
      protocol_version_inv_{payload[1u]},
      server_address_{0u},
      client_address_{0u},
      payload_type_{GetDoIPPayloadType(payload)},
      payload_length_{GetDoIPPayloadLength(payload)} {
  payload_ = (message_type_ == MessageType::kTcp)
                 ?  // header + server address(2 byte) + client address(2 byte)
                 core_type::Span<std::uint8_t>{&payload[kDoipheadrSize + 4u], payload.size() - (kDoipheadrSize + 4u)}
                 : core_type::Span<std::uint8_t>{&payload[kDoipheadrSize], payload.size() - kDoipheadrSize};
  if (message_type == MessageType::kTcp) {
    server_address_ = GetServerAddr(payload);
    client_address_ = GetClientAddr(payload);
  }  // no client
}

}  // namespace doip_client
