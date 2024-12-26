/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_MESSAGE_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_MESSAGE_H_

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "core/include/span.h"

namespace boost_support {
namespace message {
namespace tcp {

/**
 * @brief    Immutable class to store received tcp message
 */
class TcpMessage final {
 public:
  /**
   * @brief    Definition of different socket state
   */
  enum class SocketState : std::uint8_t {
    kIdle = 0x00,
    kSocketOnline,
    kSocketOffline,
    kSocketRxMessageReceived,
    kSocketTxMessageSend,
    kSocketTxMessageConf,
    kSocketError
  };

  /**
   * @brief    Definition of different socket error that could occur
   */
  enum class SocketError : std::uint8_t { kNone = 0x00 };

  /**
   * @brief    Type alias for underlying buffer
   */
  using BufferType = std::vector<std::uint8_t>;

  /**
   * @brief    Type alias of IP address type
   */
  using IpAddressType = std::string_view;

 public:
  /**
   * @brief         Constructs an instance of TcpMessage
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_number
   *                The host port number
   * @param[in]     payload
   *                The received data payload
   */
  TcpMessage(IpAddressType host_ip_address, std::uint16_t host_port_number, BufferType payload)
      : socket_state_{SocketState::kIdle},
        socket_error_{SocketError::kNone},
        payload_{std::move(payload)},
        host_ip_address_{host_ip_address},
        host_port_number_{host_port_number} {}

  TcpMessage(TcpMessage &&other) noexcept = default;
  TcpMessage &operator=(TcpMessage &&other) noexcept = default;

  TcpMessage(const TcpMessage &other) = default;
  TcpMessage &operator=(const TcpMessage &other) = default;

  /**
   * @brief         Destructs an instance of TcpMessage
   */
  ~TcpMessage() = default;

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
   * @brief       Get the readable view on received payload
   * @return      The view on payload
   */
  core_type::Span<std::uint8_t const> GetPayload() const {
    return core_type::Span<std::uint8_t const>{payload_};
  }

  /**
   * @brief       Get the state of underlying socket
   * @return      The socket state
   */
  SocketState GetSocketState() const { return socket_state_; }

  /**
   * @brief       Get the error of underlying socket
   * @return      The socket error
   */
  SocketError GetSocketError() const { return socket_error_; }

 private:
  /**
   * @brief         Store the socket state
   */
  SocketState socket_state_;

  /**
   * @brief         Store the socket error
   */
  SocketError socket_error_;

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
 * @brief    The unique pointer to const TcpMessage
 */
using TcpMessageConstPtr = std::unique_ptr<TcpMessage const>;

/**
 * @brief    The unique pointer to TcpMessage
 */
using TcpMessagePtr = std::unique_ptr<TcpMessage>;

/**
 * @brief    Doip HeaderSize
 */
constexpr std::uint8_t kDoipheadrSize = 8U;

}  // namespace tcp
}  // namespace message
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_MESSAGE_H_
