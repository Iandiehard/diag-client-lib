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
#include <string>
#include <vector>

class DoipMessage {
public:
  enum class rx_socket_type : std::uint8_t { kBroadcast, kUnicast };
  // ip address type
  using IpAddressType = std::string;

public:
  // ctor
  DoipMessage() = default;

  DoipMessage(const DoipMessage &other) = default;

  DoipMessage(DoipMessage &&other) noexcept = default;

  DoipMessage &operator=(const DoipMessage &other) = default;

  DoipMessage &operator=(DoipMessage &&other) noexcept = default;

  // rx type -> broadcast, unicast
  rx_socket_type rx_socket{rx_socket_type::kUnicast};
  // remote ip address;
  IpAddressType host_ip_address;
  // remote port number
  std::uint16_t  host_port_number;
  // doip protocol version
  std::uint8_t protocol_version{};
  // doip protocol inverse version
  std::uint8_t protocol_version_inv{};
  // doip payload type
  std::uint16_t payload_type{};
  // doip payload length
  std::uint32_t payload_length{};
  // doip payload
  std::vector<std::uint8_t> payload;
};

#endif  //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_DOIP_PAYLOAD_TYPE_H
