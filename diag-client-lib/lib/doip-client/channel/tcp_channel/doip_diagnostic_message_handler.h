/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_DIAGNOSTIC_MESSAGE_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_DIAGNOSTIC_MESSAGE_HANDLER_H_

#include <memory>
#include <vector>

#include "common/doip_message.h"
#include "sockets/tcp_socket_handler.h"
#include "uds_transport/protocol_mgr.h"
#include "uds_transport/uds_message.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

// Forward declaration
class DoipTcpChannel;

/**
 * @brief       Class used as a handler to process routing activation messages
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

 public:
  /**
   * @brief         Constructs an instance of DiagnosticMessageHandler
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to doip channel
   */
  DiagnosticMessageHandler(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel);

  /**
   * @brief         Destruct an instance of DiagnosticMessageHandler
   */
  ~DiagnosticMessageHandler();

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
   * @brief       Function to process received diagnostic acknowledgement from server
   * @param[in]   doip_payload
   *              The doip message received
   */
  void ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept;

  /**
   * @brief       Function to process received diagnostic positive/negative response from server
   * @param[in]   doip_payload
   *              The doip message received
   */
  void ProcessDoIPDiagnosticMessageResponse(DoipMessage &doip_payload) noexcept;

  /**
   * @brief       Function to handle sending of diagnostic request
   * @param[in]   diagnostic_request
   *              The diagnostic request
   * @return      The Transmission result
   */
  auto HandleDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

 private:
  /**
   * @brief       Function to send diagnostic request
   * @param[in]   diagnostic_request
   *              The routing activation request
   * @return      The Transmission result
   */
  auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
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
  class DiagnosticMessageHandlerImpl;

  /**
   * @brief  Stores the Handler implementation
   */
  std::unique_ptr<DiagnosticMessageHandlerImpl> handler_impl_;
};

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  //DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_DOIP_DIAGNOSTIC_MESSAGE_HANDLER_H_
