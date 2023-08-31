/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_CLIENT_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_CLIENT_H
// includes
#include <boost/asio.hpp>
#include <string>
#include <string_view>
#include <thread>

#include "core/include/result.h"
#include "tcp_message.h"

namespace boost_support {
namespace socket {
namespace tcp {

/**
 * @brief       Class used to create a tcp socket for handling transmission and reception of tcp message from driver
 */
class TcpClientSocket final {
 public:
  /**
   * @brief         Tcp error code
   */
  enum class TcpErrorCode : std::uint8_t { kOpenFailed, kBindingFailed, kGenericError };

  /**
   * @brief         Tcp function template used for reception
   */
  using TcpHandlerRead = std::function<void(TcpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TcpClientSocket
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     tcp_handler_read
   *                The handler to send received data to user
   */
  TcpClientSocket(std::string_view local_ip_address, std::uint16_t local_port_num, TcpHandlerRead tcp_handler_read);

  /**
   * @brief         Destruct an instance of TcpClientSocket
   */
  ~TcpClientSocket();

  /**
   * @brief         Function to Open the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> Open();

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num);

  /**
   * @brief         Function to Disconnect from host
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> DisconnectFromHost();

  /**
   * @brief         Function to trigger transmission
   * @param[in]     tcp_message
   *                The tcp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> Transmit(TcpMessageConstPtr tcp_message);

  /**
   * @brief         Function to destroy the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> Destroy();

 private:
  /**
   * @brief  Type alias for tcp protocol
   */
  using Tcp = boost::asio::ip::tcp;

  /**
   * @brief  Type alias for tcp socket
   */
  using TcpSocket = Tcp::socket;

  /**
   * @brief  Type alias for tcp ip address
   */
  using TcpIpAddress = boost::asio::ip::address;

  /**
   * @brief  Type alias for tcp error codes
   */
  using TcpErrorCodeType = boost::system::error_code;

  /**
   * @brief  Store local ip address
   */
  std::string local_ip_address_;

  /**
   * @brief  Store local port number
   */
  uint16_t local_port_num_;

  /**
   * @brief  boost io context
   */
  boost::asio::io_context io_context_;

  /**
   * @brief  Store tcp socket
   */
  TcpSocket tcp_socket_;

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
   * @brief  Store the handler
   */
  TcpHandlerRead tcp_handler_read_;

 private:
  /**
   * @brief  Function to handle the reception of tcp message
   */
  void HandleMessage();
};
}  // namespace tcp
}  // namespace socket
}  // namespace boost_support
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TCP_CLIENT_H
