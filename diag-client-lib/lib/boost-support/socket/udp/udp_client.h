/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_CLIENT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_CLIENT_H_
// includes
#include <boost/asio.hpp>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "core/include/result.h"
#include "socket/udp/udp_message.h"

namespace boost_support {
namespace socket {
namespace udp {

/**
 * @brief       Class used to create a udp socket for handling transmission and reception of udp message from driver
 */
class UdpClientSocket final {
 public:
  /**
   * @brief         Udp error code
   */
  enum class UdpErrorCode : std::uint8_t { kOpenFailed, kBindingFailed, kGenericError };

  /**
   * @brief         Type of udp port to be used underneath
   */
  enum class PortType : std::uint8_t { kUdp_Broadcast = 0, kUdp_Unicast };

  /**
   * @brief         Udp function template used for reception
   */
  using UdpHandlerRead = std::function<void(UdpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of UdpClientSocket
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     port_type
   *                The type of socket port
   * @param[in]     UdpHandlerRead
   *                The handler to send received data to user
   */
  UdpClientSocket(std::string_view local_ip_address, std::uint16_t local_port_num, PortType port_type,
                  UdpHandlerRead udp_handler_read);

  /**
   * @brief         Destruct an instance of UdpClientSocket
   */
  virtual ~UdpClientSocket();

  /**
   * @brief         Function to Open the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, UdpErrorCode> Open();

  /**
   * @brief         Function to trigger transmission
   * @param[in]     udp_message
   *                The udp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, UdpErrorCode> Transmit(UdpMessageConstPtr udp_message);

  /**
   * @brief         Function to destroy the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, UdpErrorCode> Destroy();

 private:
  /**
   * @brief  Type alias for udp protocol
   */
  using Udp = boost::asio::ip::udp;

  /**
   * @brief  Type alias for udp socket
   */
  using UdpSocket = Udp::socket;

  /**
   * @brief  Type alias for udp ip address
   */
  using UdpIpAddress = boost::asio::ip::address;

  /**
   * @brief  Type alias for udp error codes
   */
  using UdpErrorCodeType = boost::system::error_code;

  /**
   * @brief  Store local ip address
   */
  std::string local_ip_address_;

  /**
   * @brief  Store local port number
   */
  std::uint16_t local_port_num_;

  /**
   * @brief  boost io context
   */
  boost::asio::io_context io_context_;

  /**
   * @brief  Store tcp socket
   */
  UdpSocket udp_socket_;

  /**
   * @brief  Flag to terminate the thread
   */
  std::atomic_bool exit_request_;

  /**
   * @brief  Flag to start the thread
   */
  std::atomic_bool running_;

  /**
   * @brief  Conditional variable to block the thread
   */
  std::condition_variable cond_var_;

  /**
   * @brief  The thread itself
   */
  std::thread thread_;

  /**
   * @brief  mutex to lock critical section
   */
  std::mutex mutex_;

  /**
   * @brief  Store the remote endpoint
   */
  Udp::endpoint remote_endpoint_;

  /**
   * @brief  Store the port type - broadcast / unicast
   */
  PortType port_type_;

  /**
   * @brief  Store the handler
   */
  UdpHandlerRead udp_handler_read_;

  /**
   * @brief  Reception buffer needed for async reception of udp data
   */
  std::array<std::uint8_t, kDoipUdpResSize> rx_buffer_;

 private:
  /**
   * @brief  Function to handle the reception of tcp message
   */
  void HandleMessage(const UdpErrorCodeType &error, std::size_t bytes_received);
};
}  // namespace udp
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_UDP_UDP_CLIENT_H_
