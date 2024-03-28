/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_SOCKET_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_SOCKET_H_

#include <boost/asio.hpp>
#include <vector>

#include "boost-support/message/udp/udp_message.h"
#include "core/include/result.h"

namespace boost_support {
namespace socket {
namespace udp {

/**
 * @brief       Class used to create a udp socket for handling transmission and reception of udp message from driver
 */
class UdpSocket final {
 public:
  /**
   * @brief   Socket error code
   */
  enum class SocketError : std::uint8_t { kOpenFailed, kBindingFailed, kGenericError };

  /**
   * @brief   Type alias for Tcp message
   */
  using UdpMessage = boost_support::message::udp::UdpMessage;

  /**
   * @brief  Type alias for Udp message pointer
   */
  using UdpMessagePtr = boost_support::message::udp::UdpMessagePtr;

  /**
   * @brief  Type alias for Udp message const pointer
   */
  using UdpMessageConstPtr = boost_support::message::udp::UdpMessageConstPtr;

  /**
   * @brief  Type alias for Udp protocol
   */
  using Udp = boost::asio::ip::udp;

  /**
   * @brief  Type alias for Udp socket
   */
  using Socket = Udp::socket;

  /**
   * @brief         Udp function template used for reception
   */
  using UdpHandlerRead = std::function<void(UdpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TcpSocket
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     io_context
   *                The I/O context required to create socket
   */
  UdpSocket(std::string_view local_ip_address, std::uint16_t local_port_num,
            boost::asio::io_context &io_context) noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  UdpSocket(const UdpSocket &other) noexcept = delete;
  UdpSocket &operator=(const UdpSocket &other) noexcept = delete;

  /**
   * @brief  Move assignment and Move constructor
   */
  UdpSocket(UdpSocket &&other) noexcept = default;
  UdpSocket &operator=(UdpSocket &&other) noexcept = default;

  /**
   * @brief         Destruct an instance of TcpSocket
   */
  ~UdpSocket() noexcept;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(UdpHandlerRead read_handler);

  /**
   * @brief         Function to open and bind the socket to provided ip address & port
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Open() noexcept;

  /**
   * @brief         Function to trigger transmission
   * @param[in]     udp_message
   *                The udp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Transmit(UdpMessageConstPtr udp_message) noexcept;

  /**
   * @brief         Function to destroy the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Close() noexcept;

 private:
  /**
   * @brief  Type alias for udp error codes
   */
  using UdpErrorCodeType = boost::system::error_code;

  /**
   * @brief  Store the underlying udp socket
   */
  Socket udp_socket_;

  /**
   * @brief  Store the local endpoints
   */
  Udp::endpoint local_endpoint_;

  /**
   * @brief  Store the remote endpoints
   */
  Udp::endpoint remote_endpoint_;

  /**
   * @brief  Reception buffer needed for async reception of udp data
   */
  std::vector<std::uint8_t> rx_buffer_;

  /**
   * @brief  Store the handler
   */
  UdpHandlerRead udp_handler_read_;

 private:
  /**
   * @brief         Function to handle the reception of tcp message
   * @param[in]     bytes_received
   *                The number of bytes to be read
   * @return        Udp Message created from received data on success, otherwise error
   */
  core_type::Result<UdpMessagePtr> Read(std::size_t bytes_received);

  /**
   * @brief  Function to start reception of Udp dataframe
   */
  void StartReceivingMessage();
};

}  // namespace udp
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_SOCKET_H_
