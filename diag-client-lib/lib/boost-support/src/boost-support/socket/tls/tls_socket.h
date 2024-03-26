/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_SOCKET_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_SOCKET_H_

#include <boost/asio.hpp>

#include "boost-support/message/tcp/tcp_message.h"
#include "boost-support/socket/io_context.h"
#include "boost-support/socket/tls/tls_context.h"
#include "core/include/result.h"

namespace boost_support {
namespace socket {
namespace tls {

/**
 * @brief       Class used to create a tcp socket for handling transmission and reception of tcp message from driver
 */
class TlsSocket final {
 public:
  /**
   * @brief         Socket error code
   */
  enum class SocketError : std::uint8_t {
    kOpenFailed,
    kBindingFailed,
    kRemoteDisconnected,
    kTlsHandshakeFailed,
    kGenericError
  };

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = boost_support::message::tcp::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = boost_support::message::tcp::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = boost_support::message::tcp::TcpMessageConstPtr;

  /**
   * @brief  Type alias for tcp protocol
   */
  using Tcp = boost::asio::ip::tcp;

  /**
   * @brief  Type alias for tcp socket
   */
  using TcpSocket = Tcp::socket;

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
  TlsSocket(std::string_view local_ip_address, std::uint16_t local_port_num, TlsContext &tls_context,
            IoContext &io_context) noexcept;

  /**
   * @brief         Constructs an instance of TcpSocket
   * @param[in]     socket
   *                The socket
   */
  TlsSocket(TcpSocket tcp_socket, TlsContext &tls_context) noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TlsSocket(const TlsSocket &other) noexcept = delete;
  TlsSocket &operator=(const TlsSocket &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment
   */
  TlsSocket &operator=(TlsSocket &&other) noexcept = delete;

  /**
   * @brief  Move constructor
   */
  TlsSocket(TlsSocket &&other) noexcept;

  /**
   * @brief         Destruct an instance of TcpSocket
   */
  ~TlsSocket() noexcept;

  /**
   * @brief         Function to open and bind the socket to provided ip address & port
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Open() noexcept;

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Connect(std::string_view host_ip_address, std::uint16_t host_port_num) noexcept;

  /**
   * @brief         Function to Disconnect from host
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Disconnect() noexcept;

  /**
   * @brief         Function to trigger transmission
   * @param[in]     tcp_message
   *                The tcp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Transmit(TcpMessageConstPtr tcp_message) noexcept;

  /**
   * @brief         Function to read message from socket
   * @return        Tcp message on success otherwise error code
   */
  core_type::Result<TcpMessagePtr, SocketError> Read() noexcept;

  /**
   * @brief         Function to destroy the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, SocketError> Close() noexcept;

 private:
  /**
   * @brief  Type alias for tcp ip address
   */
  using TcpIpAddress = boost::asio::ip::address;

  /**
   * @brief  Type alias for tcp error codes
   */
  using TcpErrorCodeType = boost::system::error_code;

  /**
   * @brief  Type alias for tcp socket
   */
  using SslStream = boost::asio::ssl::stream<Tcp::socket>;

  /**
   * @brief  Store the underlying tcp socket
   */
  SslStream ssl_stream_;

  /**
   * @brief  Store the local endpoints
   */
  Tcp::endpoint local_endpoint_;

 private:
  /**
   * @brief  Function to get the native tcp socket under tls socket
   */
  SslStream::lowest_layer_type &GetNativeTcpSocket();
};
}  // namespace tls
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TLS_TLS_SOCKET_H_
