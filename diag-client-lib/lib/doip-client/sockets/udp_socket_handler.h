/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_UDP_SOCKET_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_UDP_SOCKET_HANDLER_H_
//includes
#include <string>
#include <string_view>

#include "boost-support/socket/udp/udp_client.h"
#include "core/include/result.h"

namespace doip_client {
// forward declaration
namespace channel {
namespace udp_channel {
class DoipUdpChannel;
}  // namespace udp_channel
}  // namespace channel

namespace sockets {

/**
 * @brief  Class used to create a udp socket for handling transmission and reception of udp message from driver
 */
class UdpSocketHandler final {
 public:
  /**
   * @brief  Type alias for port type
   */
  using PortType = boost_support::socket::udp::UdpClientSocket::PortType;

  /**
   * @brief  Type alias for Udp message
   */
  using UdpMessage = boost_support::socket::udp::UdpMessage;

  /**
   * @brief  Type alias for Udp message pointer
   */
  using UdpMessagePtr = boost_support::socket::udp::UdpMessagePtr;

  /**
   * @brief  Type alias for Udp message const pointer
   */
  using UdpMessageConstPtr = boost_support::socket::udp::UdpMessageConstPtr;

  /**
   * @brief  Type alias for Udp message
   */
  using DoipUdpChannel = channel::udp_channel::DoipUdpChannel;

 public:
  /**
   * @brief         Constructs an instance of UdpSocketHandler
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     channel
   *                The reference to tcp transport handler
   */
  UdpSocketHandler(std::string_view local_ip_address, std::uint16_t port_num, PortType port_type,
                   DoipUdpChannel &channel);

  /**
   * @brief         Destruct an instance of UdpSocketHandler
   */
  ~UdpSocketHandler() = default;

  /**
   * @brief        Function to start the socket handler
   */
  void Start();

  /**
   * @brief        Function to stop the socket handler
   */
  void Stop();

  /**
   * @brief         Function to transmit the provided udp message
   * @param[in]     udp_message
   *                The udp message
   * @return        The
   */
  core_type::Result<void> Transmit(UdpMessageConstPtr udp_message);

 private:
  /**
   * @brief  Type alias for tcp client socket
   */
  using UdpSocket = boost_support::socket::udp::UdpClientSocket;

  /**
   * @brief  Store the local ip address
   */
  std::string local_ip_address_;

  /**
   * @brief  Store the local port number
   */
  std::uint16_t local_port_num_;

  /**
   * @brief  Store the port type
   */
  UdpSocket::PortType port_type_;

  /**
   * @brief  Store the socket object
   */
  std::unique_ptr<UdpSocket> udp_socket_;

  /**
   * @brief  Store the reference to tcp channel
   */
  DoipUdpChannel &channel_;
};
}  // namespace sockets
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_SOCKETS_UDP_SOCKET_HANDLER_H_
