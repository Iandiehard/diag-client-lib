/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_HANDLER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_HANDLER_H

#include <memory>
#include <string_view>

#include "uds_transport/protocol_mgr.h"

namespace uds_transport {
// forward declaration
class Connection;
class ConversionHandler;

class UdsTransportProtocolHandler {
 public:
  /**
   * @brief  Type alias for handler id
   */
  using UdsTransportProtocolHandlerId = std::uint8_t;

  /**
   * @brief  Definitions of different initialization result
   */
  enum class InitializationResult : std::uint8_t { kInitializeOk = 0, kInitializeFailed = 1 };

  /**
   * @brief         Constructs an instance of UdsTransportProtocolHandler
   * @param[in]     handler_id
   *                The id of this transport protocol handler
   * @param[in]     transport_protocol_mgr
   *                The reference to transport protocol manager
   */
  UdsTransportProtocolHandler(UdsTransportProtocolHandlerId const handler_id, UdsTransportProtocolMgr const&)
      : handler_id_{handler_id} {}

  /**
   * @brief         Destruct an instance of UdsTransportProtocolHandler
   */
  virtual ~UdsTransportProtocolHandler() = default;

  /**
   * @brief       Function to get the handler id
   * @return      The handler id
   */
  virtual UdsTransportProtocolHandlerId GetHandlerID() const { return handler_id_; };

  /**
   * @brief        Function to initialize the handler
   * @return       The initialization result
   */
  virtual InitializationResult Initialize() = 0;

  /**
   * @brief        Function to start the handler
   */
  virtual void Start() = 0;

  /**
   * @brief        Function to stop the handler
   */
  virtual void Stop() = 0;

  /**
   * @brief       Function to create a new Tcp connection
   * @param[in]   conversation
   *              The conversation handler used by tcp connection to communicate
   * @param[in]   tcp_ip_address
   *              The local tcp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to Connection created
   */
  virtual std::unique_ptr<Connection> CreateTcpConnection(ConversionHandler& conversion_handler,
                                                          std::string_view tcpIpaddress, uint16_t portNum) = 0;

  /**
   * @brief       Function to create a new Udp connection
   * @param[in]   conversation
   *              The conversation handler used by tcp connection to communicate
   * @param[in]   udp_ip_address
   *              The local udp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to Connection created
   */
  virtual std::unique_ptr<Connection> CreateUdpConnection(ConversionHandler& conversion_handler,
                                                          std::string_view udpIpaddress, uint16_t portNum) = 0;

 protected:
  /**
   * @brief         Store the handler id
   */
  UdsTransportProtocolHandlerId handler_id_;
};
}  // namespace uds_transport
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_PROTOCOL_HANDLER_H
