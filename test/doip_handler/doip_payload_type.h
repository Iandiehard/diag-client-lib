/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_DOIP__PAYLOAD_TYPE_H
#define DIAG_CLIENT_DOIP__PAYLOAD_TYPE_H

#include <cstdint>
#include <string>
#include <vector>

/* Magic numbers */
constexpr uint8_t BYTE_POS_ZERO = 0x00;
constexpr uint8_t BYTE_POS_ONE = 0x01;
constexpr uint8_t BYTE_POS_TWO = 0x02;
constexpr uint8_t BYTE_POS_THREE = 0x03;
constexpr uint8_t BYTE_POS_FOUR = 0x04;
constexpr uint8_t BYTE_POS_FIVE = 0x05;
constexpr uint8_t BYTE_POS_SIX = 0x06;
constexpr uint8_t BYTE_POS_SEVEN = 0x07;

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

#endif  // DIAG_CLIENT_DOIP__PAYLOAD_TYPE_H
