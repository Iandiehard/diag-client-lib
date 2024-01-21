/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TLS_SERVER_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TLS_SERVER_H_

// includes
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <optional>
#include <string_view>
#include <vector>

#include "core/include/result.h"
#include "tcp_message.h"

namespace boost_support {
namespace socket {
namespace tcp {

/**
 * @brief       Tcp Server connection class to create connection with client
 */
class TcpServerConnection final {
 public:
  /**
   * @brief         Tcp error code
   */
  enum class TcpErrorCode : std::uint8_t { kOpenFailed, kBindingFailed, kGenericError };

  /**
   * @brief         Tcp function template used for reception
   */
  using TcpHandlerRead = std::function<void(TcpMessagePtr)>;

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

 public:
  /**
   * @brief         Constructs an instance of TcpServerConnection
   * @param[in]     io_context
   *                The local ip address
   * @param[in]     tcp_handler_read
   *                The handler to send received data to user
   */
  TcpServerConnection(TlsStream tls_socket, TcpHandlerRead tcp_handler_read);

  /**
   * @brief         Destruct an instance of TcpServerConnection
   */
  ~TcpServerConnection() = default;

  /**
   * @brief         Default move ctor and assignment operator
   */
  TcpServerConnection(TcpServerConnection &&) noexcept = default;
  TcpServerConnection &operator=(TcpServerConnection &&) noexcept = default;

  /**
   * @brief         Deleted copy ctor and assignment operator
   */
  TcpServerConnection(TcpServerConnection const &) = delete;
  TcpServerConnection &operator=(TcpServerConnection const &) = delete;

  /**
   * @brief         Function to trigger transmission
   * @param[in]     tcp_message
   *                The tcp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> Transmit(TcpMessageConstPtr tcp_message);

  /**
   * @brief  Function to initiate reception of tcp message
   */
  bool TryReceivingMessage();

  /**
   * @brief         Function to shutdown the socket
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void, TcpErrorCode> Shutdown();

 private:
  /**
   * @brief  Store ssl socket
   */
  TlsStream tls_socket_;

  /**
   * @brief  Store the handler
   */
  TcpHandlerRead tcp_handler_read_;

  /**
   * @brief  Function to get the native tcp socket under tls socket
   */
  TlsStream::lowest_layer_type &GetNativeTcpSocket();
};

/**
 * @brief       Class used to create a tls socket for server for handling transmission and reception of tcp message from driver
 */
class TlsServerSocket final {
 public:
  /**
   * @brief         Tcp function template used for reception
   */
  using TcpHandlerRead = std::function<void(TcpMessagePtr)>;

  /**
   * @brief  Type alias for tcp acceptor
   */
  using TcpAcceptor = boost::asio::ip::tcp::acceptor;

 public:
  /**
   * @brief         Constructs an instance of TlsServerSocket
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   */
  TlsServerSocket(std::string_view local_ip_address, std::uint16_t local_port_num);

  /**
   * @brief         Destruct an instance of TlsServerSocket
   */
  ~TlsServerSocket() = default;

  /**
   * @brief         Get the tcp connection to communicate
   * @param[in]     tcp_handler_read
   *                The local ip address
   */
  std::optional<TcpServerConnection> GetTcpServerConnection(TcpHandlerRead tcp_handler_read);

 private:
  /**
   * @brief  Type alias for tcp protocol
   */
  using Tcp = boost::asio::ip::tcp;

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
   * @brief  Store tcp acceptor
   */
  TcpAcceptor tcp_acceptor_;
};

}  // namespace tcp
}  // namespace socket
}  // namespace boost_support

#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_TCP_TLS_SERVER_H_
