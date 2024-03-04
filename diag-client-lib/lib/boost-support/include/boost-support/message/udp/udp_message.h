/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_MESSAGE_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_MESSAGE_H_

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "core/include/span.h"

namespace boost_support {
namespace message {
namespace udp {

/**
 * @brief    Maximum response size
 */
constexpr std::uint8_t kMaxUdpResSize{40u};

/**
 * @brief    Immutable class to store received udp message
 */
class UdpMessage final {
 public:
  /**
   * @brief    Type alias for underlying buffer
   */
  using BufferType = std::vector<uint8_t>;

 public:
  /**
   * @brief         Constructs an instance of UdpMessage
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_number
   *                The host port number
   * @param[in]     payload
   *                The received data payload
   */
  UdpMessage(std::string_view host_ip_address, std::uint16_t host_port_number, BufferType payload)
      : payload_{std::move(payload)},
        host_ip_address_{host_ip_address},
        host_port_number_{host_port_number} {}

  UdpMessage(UdpMessage &&other) noexcept = default;
  UdpMessage &operator=(UdpMessage &&other) noexcept = default;

  UdpMessage(const UdpMessage &other) = delete;
  UdpMessage &operator=(const UdpMessage &other) = delete;

  /**
   * @brief         Destructs an instance of UdpMessage
   */
  ~UdpMessage() = default;

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
   * @brief       Get the readable view on received paylosd
   * @return      The view on payload
   */
  core_type::Span<std::uint8_t const> const GetPayload() const { return core_type::Span<std::uint8_t const>{payload_}; }

 private:
  /**
   * @brief         The reception buffer
   */
  BufferType payload_;

  /**
   * @brief    Store remote ip address
   */
  std::string host_ip_address_;

  /**
   * @brief    Store remote port number
   */
  std::uint16_t host_port_number_;
};

/**
 * @brief    The unique pointer to const UdpMessage
 */
using UdpMessageConstPtr = std::unique_ptr<UdpMessage const>;

/**
 * @brief    The unique pointer to UdpMessage
 */
using UdpMessagePtr = std::unique_ptr<UdpMessage>;

}  // namespace udp
}  // namespace message
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_MESSAGE_H_
