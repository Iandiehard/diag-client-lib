/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOIP_CLIENT_CHANNEL_TCP_CHANNEL_ALIVE_CHECK_HANDLER_ALIVE_CHECK_HANDLER_H_
#define DOIP_CLIENT_CHANNEL_TCP_CHANNEL_ALIVE_CHECK_HANDLER_ALIVE_CHECK_HANDLER_H_

#include "doip-client/message/doip_message.h"
#include "doip-client/sockets/tcp_socket_handler.h"
#include "uds_transport-layer-api/connection.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace alive_check_handler {

class AliveCheckHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = sockets::TcpSocketHandler::MessagePtr;

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = sockets::TcpSocketHandler::Message;

  /**
   * @brief  Type alias for Doip message
   */
  using DoipMessage = message::DoipMessage;

 public:
  /**
   * @brief         Constructs an instance of RoutingActivationHandler
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   * @param[in]     connection
   *                The reference to upper layer connection
   */
  AliveCheckHandler(sockets::TcpSocketHandler &tcp_socket_handler,
                    uds_transport::Connection &connection);

  /**
   * @brief         Destruct an instance of AliveCheckHandler
   */
  ~AliveCheckHandler() = default;

  /**
   * @brief       Function to process received routing activation response
   * @param[in]   doip_payload
   *              The doip message received
   */
  void ProcessAliveCheckRequest(DoipMessage const &doip_payload) const noexcept;

 private:
  /**
   * @brief  The reference to socket handler
   */
  sockets::TcpSocketHandler &tcp_socket_handler_;

  /**
   * @brief  The reference to upper layer connection
   */
  uds_transport::Connection &connection_;
};

}  // namespace alive_check_handler
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client

#endif  // DOIP_CLIENT_CHANNEL_TCP_CHANNEL_ALIVE_CHECK_HANDLER_ALIVE_CHECK_HANDLER_H_
