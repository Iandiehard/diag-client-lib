/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef TEST_COMPONENT_COMMON_MESSAGE_DOIP_MESSAGE_H_
#define TEST_COMPONENT_COMMON_MESSAGE_DOIP_MESSAGE_H_

#include <string_view>

#include "core/include/span.h"

namespace test {
namespace component {
namespace common {
namespace message {

/**
 * @brief    Immutable class to store received doip message
 */
class DoipMessage final {
 public:
  /**
   * @brief         Constructs an instance of DoipMessage
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_number
   *                The host port number
   * @param[in]     payload
   *                The view over received data payload
   */
  DoipMessage(std::string_view host_ip_address, std::uint16_t host_port_number,
              core_type::Span<std::uint8_t const> payload);

  /**
   * @brief         Default copy assignment, copy constructor, move assignment and move constructor
   */
  DoipMessage(const DoipMessage &other) = default;
  DoipMessage(DoipMessage &&other) noexcept = default;
  DoipMessage &operator=(const DoipMessage &other) = default;
  DoipMessage &operator=(DoipMessage &&other) noexcept = default;

  /**
   * @brief         Destructs an instance of DoipMessage
   */
  ~DoipMessage() noexcept = default;

  /**
   * @brief       Get the host ip address
   * @return      The IP address
   */
  std::string_view GetHostIpAddress() const { return host_ip_address_; }

  /**
   * @brief       Get the host port number
   * @return      The port number
   */
  std::uint16_t GetHostPortNumber() const { return host_port_number_; }

  /**
   * @brief       Get the protocol version
   * @return      The protocol version
   */
  std::uint8_t GetProtocolVersion() const { return protocol_version_; }

  /**
   * @brief       Get the inverse protocol version
   * @return      The inverse protocol version
   */
  std::uint8_t GetInverseProtocolVersion() const { return protocol_version_inv_; }

  /**
   * @brief       Get the payload type
   * @return      The payload type
   */
  std::uint16_t GetPayloadType() const { return payload_type_; }

  /**
   * @brief       Get the payload
   * @return      The payload
   */
  core_type::Span<std::uint8_t const> GetPayload() const { return payload_; }

 private:
  /**
   * @brief    Store remote ip address
   */
  std::string host_ip_address_;

  /**
   * @brief    Store remote port number
   */
  std::uint16_t host_port_number_;

  /**
   * @brief    Store protocol version
   */
  std::uint8_t protocol_version_;

  /**
   * @brief    Store protocol inverse version
   */
  std::uint8_t protocol_version_inv_;

  /**
   * @brief    Store payload type
   */
  std::uint16_t payload_type_;

  /**
   * @brief    Store payload length
   */
  std::uint32_t payload_length_;

  /**
   * @brief    Store payload
   */
  core_type::Span<std::uint8_t const> payload_;
};

}  // namespace message
}  // namespace common
}  // namespace component
}  // namespace test
#endif  // TEST_COMPONENT_COMMON_MESSAGE_DOIP_MESSAGE_H_
