/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOIP_CLIENT_DOIP_CLIENT_H_
#define DOIP_CLIENT_DOIP_CLIENT_H_

#include "doip-client/connection/connection_manager/connection_manager.h"
#include "uds_transport-layer-api/connection.h"

namespace doip_client {

/**
 * @brief    Class to manage Doip Client
 */
class DoipClient final {
 public:
  DoipClient() noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  DoipClient(const DoipClient &other) noexcept = delete;
  DoipClient &operator=(const DoipClient &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  DoipClient(DoipClient &&other) noexcept = delete;
  DoipClient &operator=(DoipClient &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of DiagClient
   */
  ~DoipClient() noexcept = default;

  /**
   * @brief       Function to create a new Tcp connection
   * @param[in]   conversation
   *              The conversation handler used by tcp connection to communicate
   * @param[in]   tcp_ip_address
   *              The local tcp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to the connection created
   */
  std::unique_ptr<uds_transport::Connection> CreateTcpConnection(
      uds_transport::ConversionHandler const &conversation, std::string_view tcp_ip_address,
      std::uint16_t port_num);

  /**
   * @brief       Function to create a new Udp connection
   * @param[in]   conversation
   *              The conversation handler used by udp connection to communicate
   * @param[in]   udp_ip_address
   *              The local udp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to the connection created
   */
  std::unique_ptr<uds_transport::Connection> CreateUdpConnection(
      uds_transport::ConversionHandler const &conversation, std::string_view udp_ip_address,
      std::uint16_t port_num);

 private:
  /**
   * @brief         Store Doip Connection manager
   */
  connection::connection_manager::ConnectionManager connection_mgr_;
};

}  // namespace doip_client

#endif  // DOIP_CLIENT_DOIP_CLIENT_H_
