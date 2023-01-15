/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/** @file create_diagnostic_client.h
 *  @brief Header file for diagnostic client conversation
 *  @author Avijit Dey
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H

#include <cstdint>
#include "diagnostic_client_message_type.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief 
 */
class DiagClientConversation {
public:
  // Connect Errors
  enum class ConnectResult : std::uint8_t {
    kConnectSuccess = 0U,
    kConnectFailed,
    kConnectTimeout
  };
  
  // Disconnect Errors
  enum class DisconnectResult : std::uint8_t {
    kDisconnectSuccess = 0U,
    kDisconnectFailed
  };
  
  // Diagnostics Request Response Error
  enum class DiagResult : std::uint8_t {
    kDiagRequestSendFailed = 0U,
    kDiagFailed,
    kDiagAckTimeout,
    kDiagResponseTimeout,
    kDiagSuccess
  };
  
  // ctor
  DiagClientConversation() = default;
  
  // dtor
  virtual ~DiagClientConversation() = default;
  
  /**
   * @brief      Function to start the Diagnostic Client Conversation
   * @return     void
   */
  virtual void Startup() = 0;
  
  /**
   * @brief      Function to shut down the Diagnostic Client Conversation
   * @return     void
   */
  virtual void Shutdown() = 0;
  
  /**
   * @brief       Function to connect to Diagnostic Server.
   * @param[in]   host_ip_addr
   *              Remote server IP Address to connect with
   * @return      ConnectResult
   *              Result returned
   */
  virtual ConnectResult
  ConnectToDiagServer(IpAddress host_ip_addr) = 0;
  
  // Description   : Function to disconnect from Diagnostic Server
  // @param input  : Nothing
  // @return value : DisconnectResult
  //                 Result returned
  virtual DisconnectResult
  DisconnectFromDiagServer() = 0;
  
  // Description   : Function to send Diagnostic Request and get Diagnostic Response
  // @param input  : UdsRequestMessageConstPtr
  //                 Diagnostic request message to be sent to remote server
  // @return value : DiagResult
  //                 Result returned
  // @return value : UdsResponseMessagePtr
  //                 Diagnostic Response message received, null_ptr incase of error
  virtual std::pair<DiagResult, uds_message::UdsResponseMessagePtr>
  SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) = 0;
};
}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
