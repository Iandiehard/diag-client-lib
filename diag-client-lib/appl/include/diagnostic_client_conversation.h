/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H

#include <cstdint>

#include "diagnostic_client_uds_message_type.h"
#include "include/diagnostic_client_result.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief       Conversation class to establish connection with a Diagnostic Server
 * @details     This only support DoIP communication protocol for diagnostic communication with remote Diagnostic Server
 */
class DiagClientConversation final {
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
    kConnectSuccess = 0U, /**< Successfully connected to the Diagnostic Server */
    kConnectFailed = 1U,  /**< Connection failure to the Diagnostic Server, check logs for more failure information */
    kConnectTimeout = 2U, /**< No Connection response received from the Diagnostic Server */
    kTlsRequired = 3U     /**< The Diagnostic Server only accepts secured connections, open TLS connection */
  };

  /**
   * @brief      Definitions of Disconnection results
   */
  enum class DisconnectResult : std::uint8_t {
    kDisconnectSuccess = 0U,  /**< Successfully disconnected from Diagnostic Server */
    kDisconnectFailed = 1U,   /**< Disconnection failure with Diagnostic Server, check logs for more information */
    kAlreadyDisconnected = 2U /**< Not connected to Diagnostic Server */
  };

  /**
   * @brief      Definitions of Diagnostics Request Response results
   */
  enum class DiagError : std::uint8_t {
    kDiagGenericFailure = 1U,    /**< Generic Diagnostic Error, see logs for more information */
    kDiagRequestSendFailed = 2U, /**< Diagnostic request message transmission failure */
    kDiagAckTimeout = 3U,        /**< No diagnostic acknowledgement response received within 2 seconds */
    kDiagNegAckReceived = 4U,    /**< Diagnostic negative acknowledgement received */
    kDiagResponseTimeout = 5U,   /**< No diagnostic response message received within P2/P2Star time */
    kDiagInvalidParameter = 6U,  /**< Passed parameter value is not valid */
    kDiagBusyProcessing = 7U     /**< Conversation is already busy processing previous request */
  };

  /**
   * @brief         Constructor an instance of DiagClientConversation
   * @param[in]     conversation_name
   *                The name of conversation configured as json parameter under "ConversationName"
   * @implements    DiagClientLib-Conversation-Construction, DiagClientLib-DoIP-Support
   */
  explicit DiagClientConversation(std::string_view conversation_name) noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  DiagClientConversation(const DiagClientConversation &other) noexcept = delete;
  DiagClientConversation &operator=(const DiagClientConversation &other) noexcept = delete;

  /**
   * @brief  Move assignment and move constructor
   */
  DiagClientConversation(DiagClientConversation &&other) noexcept = default;
  DiagClientConversation &operator=(DiagClientConversation &&other) noexcept = default;

  /**
   * @brief         Destructor an instance of DiagClientConversation
   * @implements    DiagClientLib-Conversation-Destruction
   */
  ~DiagClientConversation() noexcept;

  /**
   * @brief         Function to startup the Diagnostic Client Conversation
   * @details       Must be called once and before using any other functions of DiagClientConversation
   * @implements    DiagClientLib-Conversation-StartUp
   */
  void Startup() noexcept;

  /**
   * @brief         Function to shutdown the Diagnostic Client Conversation
   * @details       Must be called during shutdown phase, no further processing of any function will
   *                be allowed after this call
   * @implements    DiagClientLib-Conversation-Shutdown
   */
  void Shutdown() noexcept;

  /**
   * @brief         Function to connect to Diagnostic Server using Target address and IP address of the server
   * @details       This shall initiate a TCP connection with server and then send DoIP Routing Activation request
   * @param[in]     target_address
   *                Logical address of the Remote server
   * @param[in]     host_ip_addr
   *                IP address of the Remote server
   * @return        ConnectResult
   *                The connection result
   * @implements    DiagClientLib-Conversation-Connect
   */
  ConnectResult ConnectToDiagServer(std::uint16_t target_address, IpAddress host_ip_addr) noexcept;

  /**
   * @brief         Function to disconnect from Diagnostic Server
   * @details       This will close the existing TCP connection with Server and reset Routing Activation state
   * @return        DisconnectResult
   *                Disconnection result returned
   * @implements    DiagClientLib-Conversation-Disconnect
   */
  DisconnectResult DisconnectFromDiagServer() noexcept;

  /**
   * @brief         Function to send Diagnostic Request and get Diagnostic Response
   * @details       This is a blocking function i.e. function call either returns with final diagnostic response (Positive/Negative)
   *                or with error. It also handles reception of pending response(NRC 0x78) internally within this function call
   * @param[in]     message
   *                The diagnostic request message wrapped in a unique pointer
   * @pre           Must be connected to diagnostic server
   * @return        uds_message::UdsResponseMessagePtr
   *                Diagnostic Response message received, DiagError in case of error
   * @implements    DiagClientLib-Conversation-DiagRequestResponse
   */
  Result<uds_message::UdsResponseMessagePtr, DiagError> SendDiagnosticRequest(
      uds_message::UdsRequestMessageConstPtr message) noexcept;

 private:
  /**
   * @brief    Forward declaration of diag client conversation implementation
   */
  class DiagClientConversationImpl;

  /**
   * @brief     Unique pointer to diag client conversation implementation
   */
  std::unique_ptr<DiagClientConversationImpl> diag_client_conversation_impl_;
};
}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
