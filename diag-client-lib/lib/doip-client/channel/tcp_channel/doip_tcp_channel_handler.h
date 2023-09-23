/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_TCP_CHANNEL_HANDLER_H_
//includes
#include <mutex>

#include "channel/tcp_channel/doip_routing_activation_handler.h"
#include "channel/tcp_channel/doip_tcp_channel.h"
#include "common/common_doip_types.h"
#include "common/doip_message.h"
#include "core/include/common_header.h"
#include "doip_routing_activation_handler.h"
#include "sockets/tcp_socket_handler.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

using DiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;

/**
 * @brief       Class used as a handler to process diagnostic messages
 */
class DiagnosticMessageHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = sockets::TcpSocketHandler::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = sockets::TcpSocketHandler::TcpMessage;

  /**
   * @brief  Type holding diag acknowledgement type
   */
  struct DiagAckType {
    std::uint8_t ack_type_;
  };

 public:
  // ctor
  DiagnosticMessageHandler(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel)
      : tcp_socket_handler_{tcp_socket_handler},
        channel_{channel} {}

  // dtor
  ~DiagnosticMessageHandler() = default;

  // Function to process Routing activation response
  auto ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept -> void;

  // Function to process Diagnostic message response
  auto ProcessDoIPDiagnosticMessageResponse(DoipMessage &doip_payload) noexcept -> void;

  // Function to send Diagnostic request
  auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr &message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

 private:
  static auto CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType,
                                      uint32_t payloadLen) noexcept -> void;

  // socket reference
  tcpSocket::TcpSocketHandler &tcp_socket_handler_;
  // channel reference
  DoipTcpChannel &channel_;
};

/**
 * @brief       Class to handle received messages from lower layer
 */
class DoipTcpChannelHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = sockets::TcpSocketHandler::TcpMessagePtr;

  /**
   * @brief         Constructs an instance of DoipTcpChannelHandler
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to tcp transport handler
   */
  DoipTcpChannelHandler(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel)
      : routing_activation_handler_{tcp_socket_handler, channel},
        diagnostic_message_handler_{tcp_socket_handler, channel} {}

  /**
   * @brief         Destruct an instance of DoipTcpChannelHandler
   */
  ~DoipTcpChannelHandler() = default;

  /**
   * @brief         Function to send routing activation request
   * @param[in]     routing_activation_request
   *                The routing activation request
   * @return        TransmissionResult
   *                The transmission result
   */
  auto SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr routing_activation_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

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
