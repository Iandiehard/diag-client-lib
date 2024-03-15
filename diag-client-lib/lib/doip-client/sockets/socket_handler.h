/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_SOCKET_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_SOCKET_HANDLER_H_

#include <string_view>

#include "boost-support/client/tcp/tcp_client.h"
#include "boost-support/client/udp/udp_client.h"
#include "core/include/result.h"

namespace doip_client {
namespace sockets {

/**
 * @brief  Class used to create a tcp socket for handling transmission and reception of tcp message from driver
 */
template<typename ClientType>
class SocketHandler final {
 public:
  /**
   * @brief  Type alias for client
   */
  using Client = ClientType;

  /**
   * @brief  Type alias for message
   */
  using Message = typename Client::Message;

  /**
   * @brief  Type alias for message pointer
   */
  using MessagePtr = typename Client::MessagePtr;

  /**
   * @brief  Type alias for message const pointer
   */
  using MessageConstPtr = typename Client::MessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(MessagePtr)>;

  /**
   * @brief         Constructs an instance of TcpSocketHandler
   * @param[in]     socket
   *                The socket used
   */
  explicit SocketHandler(Client client) noexcept : client_{std::move(client)} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  SocketHandler(const SocketHandler &other) noexcept = delete;
  SocketHandler &operator=(const SocketHandler &other) noexcept = delete;

  /**
   * @brief  Move assignment and Move constructor
   */
  SocketHandler(SocketHandler &&other) noexcept = default;
  SocketHandler &operator=(SocketHandler &&other) noexcept = default;

  /**
   * @brief         Destruct an instance of TcpSocketHandler
   */
  ~SocketHandler() = default;

  /**
   * @brief        Function to start the socket handler
   */
  void Initialize() noexcept { client_.Initialize(); }

  /**
   * @brief        Function to stop the socket handler
   */
  void DeInitialize() noexcept { client_.DeInitialize(); }

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) { client_.SetReadHandler(std::move(read_handler)); }

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
    return client_.ConnectToHost(host_ip_address, host_port_num);
  }

  /**
   * @brief         Function to disconnect from remote host if already connected
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> DisconnectFromHost() { return client_.DisconnectFromHost(); }

  /**
   * @brief         Function to get the connection status
   * @return        True if connected, False otherwise
   */
  auto IsConnectedToHost() const noexcept -> bool { return client_.IsConnectedToHost(); }

  /**
   * @brief         Function to transmit the provided message
   * @param[in]     message
   *                The message to be sent
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(MessageConstPtr message) { return client_.Transmit(std::move(message)); }

 private:
  /**
   * @brief  Store the client object
   */
  Client client_;
};

/**
 * @brief  Type alias of Tcp socket handler
 */
using TcpSocketHandler = SocketHandler<boost_support::client::tcp::TcpClient>;

/**
 * @brief  Type alias of Udp socket handler
 */
using UdpSocketHandler = SocketHandler<boost_support::client::udp::UdpClient>;

}  // namespace sockets
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_SOCKET_HANDLER_H_
