/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_ROUTING_ACTIVATION_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_ROUTING_ACTIVATION_HANDLER_H_

#include "channel/tcp_channel/doip_tcp_channel.h"
#include "common/doip_message.h"
#include "sockets/tcp_socket_handler.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

/**
 * @brief       Class used as a handler to process routing activation messages
 */
class RoutingActivationHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = sockets::TcpSocketHandler::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = sockets::TcpSocketHandler::TcpMessage;

 public:
  /**
   * @brief         Constructs an instance of RoutingActivationHandler
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to doip channel
   */
  RoutingActivationHandler(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel);

  /**
   * @brief         Destruct an instance of RoutingActivationHandler
   */
  ~RoutingActivationHandler() = default;

  /**
   * @brief       Function to process received routing activation response
   * @param[in]   doip_payload
   *              The doip message received
   */
  void ProcessDoIPRoutingActivationResponse(DoipMessage &doip_payload) noexcept;

  /**
   * @brief       Function to handle sending of routing activation request
   * @param[in]   routing_activation_request
   *              The routing activation request
   * @return      Transmission result
   */
  auto HandleRoutingActivationRequest(uds_transport::UdsMessageConstPtr routing_activation_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::ConnectionResult;

 private:
  /**
   * @brief       Function to send routing activation request
   * @param[in]   routing_activation_request
   *              The routing activation request
   * @return      Transmission result
   */
  auto SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr &routing_activation_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  /**
   * @brief            Function to create doip generic header
   * @param[in,out]    doip_header
   *                   The doip header
   * @param[in]        payload_type
   *                   The type of payload
   * @param[in]        payload_len
   *                   The length of payload
   */
  static void CreateDoipGenericHeader(std::vector<std::uint8_t> &doip_header, std::uint16_t payload_type,
                                      std::uint32_t payload_len);

 private:
  /**
   * @brief  Forward declaration Handler implementation
   */
  class RoutingActivationHandlerImpl;

  /**
   * @brief  Stores the Handler implementation
   */
  std::unique_ptr<RoutingActivationHandlerImpl> handler_impl_;
};

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  //DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_ROUTING_ACTIVATION_HANDLER_H_
