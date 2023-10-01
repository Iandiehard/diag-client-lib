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
namespace socket {
namespace udp {

/**
   * @brief    Doip maximum response size
   */
constexpr std::uint8_t kDoipUdpResSize{40u};

/**
 * @brief    Immutable class to store received udp message
 */
class UdpMessage final {
 public:
  /**
   * @brief    Type alias for underlying buffer
   */
  using BufferType = std::vector<uint8_t>;

  /**
   * @brief    Type alias of IP address type
   */
  using IpAddressType = std::string_view;

 public:
  /**
   * @brief         Default constructor of UdpMessage
   */
  UdpMessage() : rx_buffer_{}, tx_buffer_{}, host_ip_address_{}, host_port_number_{} {}

  /**
   * @brief         Constructs an instance of UdpMessage
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_number
   *                The host port number
   * @param[in]     payload
   *                The received data payload
   */
  UdpMessage(IpAddressType host_ip_address, std::uint16_t host_port_number, BufferType payload)
      : rx_buffer_{std::move(payload)},
        tx_buffer_{},
        host_ip_address_{host_ip_address},
        host_port_number_{host_port_number} {}

  UdpMessage(UdpMessage &&other) noexcept = default;
  UdpMessage &operator=(UdpMessage &&other) noexcept = default;

  UdpMessage(const UdpMessage &other) = delete;
  UdpMessage &operator=(const UdpMessage &other) = delete;

  /**
   * @brief         Destructs an instance of UdpMessage
   */
  virtual ~UdpMessage() = default;

  /**
   * @brief       Get the host ip address
   * @return      The IP address
   */
  IpAddressType GetHostIpAddress() const { return host_ip_address_; }

  /**
   * @brief       Get the host port number
   * @return      The port number
   */
  std::uint16_t GetHostPortNumber() const { return host_port_number_; }

  /**
   * @brief       Get the view to the rx buffer
   * @return      The rx buffer
   */
  core_type::Span<std::uint8_t> GetRxBuffer() { return core_type::Span<std::uint8_t>{rx_buffer_}; }

  /**
   * @brief       Get the reference to tx buffer
   * @return      The reference to buffer
   */
  BufferType &GetTxBuffer() { return tx_buffer_; }

  /**
   * @brief       Get the reference to tx buffer
   * @return      The reference to buffer
   */
  BufferType const &GetTxBuffer() const { return tx_buffer_; }

 private:
  /**
   * @brief         The reception buffer
   */
  BufferType rx_buffer_;

  /**
   * @brief         The transmission buffer
   */
  BufferType tx_buffer_;

  /**
   * @brief    Store remote ip address
   */
  IpAddressType host_ip_address_;

  /**
   * @brief    Store remote port number
   */
  std::uint16_t host_port_number_;
};

/**
 * @brief    The unique pointer to const UdpMessage
 */
using UdpMessageConstPtr = std::unique_ptr<const UdpMessage>;

/**
 * @brief    The unique pointer to UdpMessage
 */
using UdpMessagePtr = std::unique_ptr<UdpMessage>;

}  // namespace udp
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_MESSAGE_H_
