/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/** @file diagnostic_client_conversation.h
 *  @brief Header file of diagnostic client conversation
 *  @author Avijit Dey
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H

#include <cstdint>

#include "diagnostic_client_uds_message_type.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief       Conversation class to establish connection with a Diagnostic Server
 * @details     Conversation class only support DoIP communication protocol for connecting to remote ECU
 */
class DiagClientConversation {
public:
  /**
   * @brief         Type alias of ip address type
   */
  using IpAddress = uds_message::UdsMessage::IpAddress;

public:
  /**
   * @brief      Definitions of Connection results
   */
  enum class ConnectResult : std::uint8_t {
    kConnectSuccess = 0U, /**< Successfully connected to Diagnostic Server */
    kConnectFailed = 1U,  /**< Connection failure to Diagnostic Server, check logs for more failure information */
    kConnectTimeout = 2U  /**< No Connection response received from Diagnostic Server */
  };

  /**
   * @brief      Definitions of Disconnection results
   */
  enum class DisconnectResult : std::uint8_t {
    kDisconnectSuccess = 0U, /**< Successfully disconnected from Diagnostic Server */
    kDisconnectFailed = 1U /**< Disconnection failure with Diagnostic Server, check logs for more failure information */
  };

  /**
   * @brief      Definitions of Diagnostics Request Response results
   */
  enum class DiagResult : std::uint8_t {
    kDiagSuccess = 0U,           /**< Diagnostic request message transmitted and response received successfully */
    kDiagGenericFailure = 1U,    /**< Generic Diagnostic Error, see logs for more information */
    kDiagRequestSendFailed = 2U, /**< Diagnostic request message transmission failure */
    kDiagAckTimeout = 3U,        /**< No diagnostic acknowledgement response received within 2 seconds */
    kDiagNegAckReceived = 4U,    /**< Diagnostic negative acknowledgement received */
    kDiagResponseTimeout = 5U,   /**< No diagnostic response message received within P2/P2Star time */
    kDiagInvalidParameter = 6U,  /**< Passed parameter value is not valid */
    kDiagBusyProcessing = 7U     /**< Conversation is already busy processing previous request */
  };

  /**
   * @brief      Constructor an instance of DiagClientConversation
   * @remarks    Implemented requirements:
   *             DiagClientLib-Conversation-Construction, DiagClientLib-DoIP-Support
   */
  DiagClientConversation() = default;

  /**
   * @brief      Destructor an instance of DiagClientConversation
   * @remarks    Implemented requirements:
   *             DiagClientLib-Conversation-Destruction
   */
  virtual ~DiagClientConversation() = default;

  /**
   * @brief      Function to startup the Diagnostic Client Conversation
   * @details    Must be called once and before using any other functions of DiagClientConversation
   * @remarks    Implemented requirements:
   *             DiagClientLib-Conversation-StartUp
   */
  virtual void Startup() = 0;

  /**
   * @brief      Function to shutdown the Diagnostic Client Conversation
   * @details    Must be called during shutdown phase, no further processing of any
   *             function will be allowed after this call
   * @remarks    Implemented requirements:
   *             DiagClientLib-Conversation-Shutdown
   */
  virtual void Shutdown() = 0;

  /**
   * @brief       Function to connect to Diagnostic Server.
   * @param[in]   host_ip_addr
   *              Remote server IP Address to connect to
   * @return      ConnectResult
   *              Connection result returned
   * @remarks     Implemented requirements:
   *              DiagClientLib-Conversation-Connect
   */
  virtual ConnectResult ConnectToDiagServer(IpAddress host_ip_addr) = 0;

  /**
   * @brief       Function to disconnect from Diagnostic Server
   * @return      DisconnectResult
   *              Disconnection result returned
   * @remarks     Implemented requirements:
   *              DiagClientLib-Conversation-Disconnect
   */
  virtual DisconnectResult DisconnectFromDiagServer() = 0;

  /**
   * @brief       Function to send Diagnostic Request and get Diagnostic Response
   * @param[in]   message
   *              Diagnostic request message wrapped in a unique pointer
   * @return      DiagResult
   *              Result returned
   * @return      uds_message::UdsResponseMessagePtr
   *              Diagnostic Response message received, null_ptr in case of error
   * @remarks     Implemented requirements:
   *              DiagClientLib-Conversation-DiagRequestResponse
   */
  virtual std::pair<DiagResult, uds_message::UdsResponseMessagePtr> SendDiagnosticRequest(
      uds_message::UdsRequestMessageConstPtr message) = 0;
};
}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
