/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CONNECTION_CONNECTION_MANAGER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CONNECTION_CONNECTION_MANAGER_H_

#include <memory>
#include <string_view>

#include "uds_transport-layer-api/connection.h"

namespace doip_client {
namespace connection {
namespace connection_manager {

/**
 * @brief    Manages Doip tcp and udp connections
 */
class ConnectionManager final {
 public:
  /**
   * @brief  Type alias for boost context
   */
  struct IoContext {};

 public:
  /**
   * @brief         Constructs an instance of DoipConnectionManager
   */
  ConnectionManager() noexcept;

  /**
   * @brief         Destruct an instance of DoipConnectionManager
   */
  ~ConnectionManager() noexcept = default;

  /**
   * @brief       Function to find or create a new Tcp connection
   * @param[in]   conversation
   *              The conversation handler used by tcp connection to communicate
   * @param[in]   tcp_ip_address
   *              The local tcp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to tcp connection created
   */
  std::unique_ptr<uds_transport::Connection> CreateTcpConnection(
      uds_transport::ConversionHandler const &conversation, std::string_view tcp_ip_address,
      std::uint16_t port_num);

  /**
   * @brief       Function to find or create a new Udp connection
   * @param[in]   conversation
   *              The conversation handler used by tcp connection to communicate
   * @param[in]   udp_ip_address
   *              The local udp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to udp connection created
   */
  std::unique_ptr<uds_transport::Connection> CreateUdpConnection(
      uds_transport::ConversionHandler const &conversation, std::string_view udp_ip_address,
      std::uint16_t port_num);

 private:
  /**
   * @brief  Stores the io context
   */
  IoContext io_context_;
};

}  // namespace connection_manager
}  // namespace connection
}  // namespace doip_client

#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CONNECTION_CONNECTION_MANAGER_H_
