/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TLS_TLS_CLIENT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TLS_TLS_CLIENT_H_

#include <functional>
#include <string_view>

#include "boost-support/client/tls/tls_version.h"
#include "boost-support/message/tcp/tcp_message.h"
#include "core/include/result.h"

namespace boost_support {
namespace client {
namespace tls {

// Forward declaration
template<typename TlsVersion>
class TlsClient;

/**
 * @brief    Client that manages secured tcp connection using Tls version 1.2
 */
using TlsClient12 = TlsClient<TlsVersion12>;

/**
 * @brief    Client that manages secured tcp connection using Tls version 1.3
 */
using TlsClient13 = TlsClient<TlsVersion13>;

/**
 * @brief    Client that manages secured tcp connection
 * @tparam   TlsVersion
 *           The tls version to be used by client for communication
 */
template<typename TlsVersion>
class TlsClient final {
 public:
  /**
   * @brief  Type alias for Tcp message
   */
  using Message = boost_support::message::tcp::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using MessagePtr = boost_support::message::tcp::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using MessageConstPtr = boost_support::message::tcp::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(MessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TlsClient
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     ca_certification_path
   *                The path to root ca certificate
   */
  TlsClient(std::string_view local_ip_address, std::uint16_t local_port_num,
            std::string_view ca_certification_path, TlsVersion tls_version) noexcept;

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TlsClient(const TlsClient &other) noexcept = delete;
  TlsClient &operator=(const TlsClient &other) noexcept = delete;

  /**
   * @brief         Move assignment and move constructor
   */
  TlsClient(TlsClient &&other) noexcept;
  TlsClient &operator=(TlsClient &&other) noexcept;

  /**
   * @brief         Destruct an instance of TlsClient
   */
  ~TlsClient() noexcept;

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept;

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) noexcept;

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> ConnectToHost(std::string_view host_ip_address,
                                        std::uint16_t host_port_num);

  /**
   * @brief         Function to disconnect from remote host if already connected
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> DisconnectFromHost();

  /**
   * @brief         Function to get the connection status
   * @return        True if connected, False otherwise
   */
  auto IsConnectedToHost() const noexcept -> bool;

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     tcp_message
   *                The tcp message
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(MessageConstPtr tcp_message);

 private:
  /**
   * @brief    Forward declaration of tls client implementation
   */
  class TlsClientImpl;

  /**
   * @brief    Unique pointer to tls client implementation
   */
  std::unique_ptr<TlsClientImpl> tls_client_impl_;
};

}  // namespace tls
}  // namespace client
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_TLS_TLS_CLIENT_H_
