/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TLS_CLIENT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TLS_CLIENT_H_
// includes
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>
#include <string_view>
#include <thread>

#include "boost-support/message/tcp/tcp_message.h"
#include "core/include/result.h"

namespace boost_support {
namespace socket {
namespace tcp {

/**
 * @brief       Class used to create a tls socket for handling transmission and reception of tcp message from driver
 */
class TlsClientSocket final {
 public:
  /**
   * @brief         TLS error code
   */
  enum class TlsErrorCode : std::uint8_t {
    kOpenFailed,
    kBindingFailed,
    kConnectFailed,
    kTlsHandshakeFailed,
    kGenericError
  };

  /**
   * @brief         Tcp function template used for reception
   */
  using TcpHandlerRead = std::function<void(message::tcp::TcpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TlsClientSocket
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     tcp_handler_read
   *                The handler to send received data to user
   * @param[in]     ca_certification_path
   *                The path to root ca certificate
   */
  TlsClientSocket(std::string_view local_ip_address, std::uint16_t local_port_num, TcpHandlerRead tcp_handler_read,
                  std::string_view ca_certification_path);

  /**
   * @brief         Destruct an instance of TcpClientSocket
   */
  ~TlsClientSocket();

  /**
   * @brief         Function to Open the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TlsErrorCode> Open();

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TlsErrorCode> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num);

  /**
   * @brief         Function to Disconnect from host
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TlsErrorCode> DisconnectFromHost();

  /**
   * @brief         Function to trigger transmission
   * @param[in]     tcp_message
   *                The tcp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TlsErrorCode> Transmit(message::tcp::TcpMessageConstPtr tcp_message);

  /**
   * @brief         Function to destroy the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TlsErrorCode> Destroy();

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
   * @brief  Type alias for tls stream wrapping tcp socket
   */
  using TlsStream = boost::asio::ssl::stream<TcpSocket>;

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
  std::uint16_t local_port_num_;

  /**
   * @brief  boost io context
   */
  boost::asio::io_context io_context_;

  /**
   * @brief  boost io ssl context
   */
  boost::asio::ssl::context io_ssl_context_;

  /**
   * @brief  Store ssl socket
   */
  TlsStream tls_socket_;

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
   * @brief  mutex to lock critical section
   */
  std::mutex mutex_;

  /**
   * @brief  The thread itself
   */
  std::thread thread_;

  /**
   * @brief  Store the handler
   */
  TcpHandlerRead tcp_handler_read_;

 private:
  /**
   * @brief  Function to get the native tcp socket under tls socket
   */
  TlsStream::lowest_layer_type& GetNativeTcpSocket();

  /**
   * @brief  Function to handle the reception of tcp message
   */
  void HandleMessage();
};
}  // namespace tcp
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TLS_CLIENT_H_
