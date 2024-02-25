/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_HANDLER_H_

#include <mutex>

#include "channel/tcp_channel/doip_diagnostic_message_handler.h"
#include "channel/tcp_channel/doip_routing_activation_handler.h"
#include "common/doip_message.h"
#include "sockets/socket_handler.h"
#include "uds_transport/protocol_mgr.h"
#include "uds_transport/uds_message.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

// Forward declaration
class DoipTcpChannel;

/**
 * @brief       Class to handle tcp received messages from lower layer
 */
class DoipTcpChannelHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = sockets::TcpSocketHandler::MessagePtr;

  /**
   * @brief         Constructs an instance of DoipTcpChannelHandler
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to tcp transport handler
   */
  DoipTcpChannelHandler(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel);

  /**
   * @brief        Function to start the handler
   */
  void Start();

  /**
   * @brief        Function to stop the handler
   * @details      This will reset all the internal handler back to default state
   */
  void Stop();

  /**
   * @brief        Function to reset the handler
   * @details      This will reset all the internal handler back to default state
   */
  void Reset();

  /**
   * @brief         Function to send routing activation request
   * @param[in]     routing_activation_request
   *                The routing activation request
   * @return        ConnectionResult
   *                The connection result
   */
  auto SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr routing_activation_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::ConnectionResult;

  /**
   * @brief         Function to send diagnostic request
   * @param[in]     diagnostic_request
   *                The diagnostic request
   * @return        TransmissionResult
   *                The transmission result
   */
  auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  /**
   * @brief         Function to process the received message
   * @param[in]     tcp_rx_message
   *                The message received
   */
  void HandleMessage(TcpMessagePtr tcp_rx_message) noexcept;

  /**
   * @brief       Check if routing activation is active for this handler
   * @return      True if activated, otherwise False
   */
  auto IsRoutingActivated() noexcept -> bool;

 private:
  /**
   * @brief         Function to process doip header in received response
   * @param[in]     doip_rx_message
   *                The received doip rx message
   * @param[in]     nack_code
   *                The negative ack code
   */
  auto ProcessDoIPHeader(DoipMessage &doip_rx_message, std::uint8_t &nack_code) noexcept -> bool;

  /**
   * @brief         Function to verify payload length of various payload type
   * @param[in]     payload_len
   *                The length of payload received
   * @param[in]     payload_type
   *                The type of payload
   */
  auto ProcessDoIPPayloadLength(std::uint32_t payload_len, std::uint16_t payload_type) noexcept -> bool;

  /**
   * @brief         Function to process the doip payload
   * @param[in]     doip_payload
   *                The reference to received payload
   */
  void ProcessDoIPPayload(DoipMessage &doip_payload) noexcept;

  /**
   * @brief         Handler to process routing activation req/ resp
   */
  RoutingActivationHandler routing_activation_handler_;

  /**
   * @brief         Handler to process diagnostic message req/ resp
   */
  DiagnosticMessageHandler diagnostic_message_handler_;

  /**
   * @brief         Mutex to protect critical section
   */
  std::mutex channel_handler_lock;
};

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  //DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_HANDLER_H_
