/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DOIP_CLIENT_TRANSPORT_PROTOCOL_HANDLER_DOIP_TRANSPORT_PROTOCOL_HANDLER_H_
#define DOIP_CLIENT_TRANSPORT_PROTOCOL_HANDLER_DOIP_TRANSPORT_PROTOCOL_HANDLER_H_

#include <memory>
#include <string_view>

#include "uds_transport-layer-api/protocol_handler.h"

namespace doip_client {
namespace transport_protocol_handler {

/**
 * @brief           Protocol handler class to manage the whole Doip protocol communication
 * @details         This class must be instantiated by user for using the DoIP functionalities.
 *                  This will inherit uds transport protocol handler
 */
class DoipTransportProtocolHandler final : public uds_transport::UdsTransportProtocolHandler {
 public:
  /**
   * @brief  Type alias for handler id
   */
  using UdsTransportProtocolHandlerId =
      uds_transport::UdsTransportProtocolHandler::UdsTransportProtocolHandlerId;

  /**
   * @brief  Type alias for Initialization result
   */
  using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

  /**
   * @brief         Constructs an instance of DoipTransportProtocolHandler
   * @param[in]     handler_id
   *                The id of this transport protocol handler
   * @param[in]     transport_protocol_mgr
   *                The reference to transport protocol manager
   */
  DoipTransportProtocolHandler(
      UdsTransportProtocolHandlerId handler_id,
      uds_transport::UdsTransportProtocolMgr const &transport_protocol_mgr);

  /**
   * @brief         Destruct an instance of DoipTransportProtocolHandler
   */
  ~DoipTransportProtocolHandler() override;

  /**
   * @brief        Function to initialize the handler
   * @return       The initialization result
   */
  InitializationResult Initialize() override;

  /**
   * @brief        Function to start the protocol handler
   */
  void Start() override;

  /**
   * @brief        Function to stop the protocol handler
   */
  void Stop() override;

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
      uds_transport::ConversionHandler &conversation, std::string_view tcp_ip_address,
      std::uint16_t port_num) override;

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
      uds_transport::ConversionHandler &conversation, std::string_view udp_ip_address,
      std::uint16_t port_num) override;

 private:
  /**
   * @brief    Forward declaration of doip protocol handler implementation
   */
  class DoipTransportProtocolHandlerImpl;

  /**
   * @brief    Unique pointer to doip protocol handler implementation
   */
  std::unique_ptr<DoipTransportProtocolHandlerImpl> doip_transport_protocol_handler_impl_;
};

}  // namespace transport_protocol_handler
}  // namespace doip_client

#endif  // DOIP_CLIENT_TRANSPORT_PROTOCOL_HANDLER_DOIP_TRANSPORT_PROTOCOL_HANDLER_H_
