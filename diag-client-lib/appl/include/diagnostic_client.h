/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/** @file diagnostic_client.h
 *  @brief Header file of Diag Client Class
 *  @author Avijit Dey
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H

#include <string_view>

#include "diagnostic_client_conversation.h"
#include "diagnostic_client_vehicle_info_message_type.h"

namespace diag {
namespace client {

/**
 * @brief    Class to manage Diagnostic Client
 */
class DiagClient {
public:
  /**
   * @brief  Definitions of Vehicle Identification response result
   */
  enum class VehicleResponseResult : std::uint8_t {
    kTransmitFailed = 0U,     /**< Failure on Transmissions */
    kInvalidParameters = 1U,  /**< Invalid Parameter passed */
    kNoResponseReceived = 2U, /**< No vehicle identification response received */
    kStatusOk = 3U            /**< Vehicle identification response received success */
  };

public:
  /**
   * @brief         Constructs an instance of DiagClient
   * @remarks       Implemented requirements:
   *                DiagClientLib-Construction
   */
  DiagClient() = default;

  /**
   * @brief         Destruct an instance of DiagClient
   * @remarks       Implemented requirements:
   *                DiagClientLib-Destruction
   */
  virtual ~DiagClient() = default;

  /**
   * @brief        Function to initialize the already created instance of DiagClient
   * @details      Must be called once and before using any other functions of DiagClient
   * @remarks      Implemented requirements:
   *               DiagClientLib-Initialization
   */
  virtual void Initialize() = 0;

  /**
   * @brief        Function to de-initialize the already initialized instance of DiagClient
   * @details      Must be called during shutdown phase, no further processing of any
   *               function will be allowed after this call
   * @remarks      Implemented requirements:
   *               DiagClientLib-DeInitialization
   */
  virtual void DeInitialize() = 0;

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      std::pair<VehicleResponseResult, diag::client::vehicle_info::VehicleInfoMessageResponsePtr>
   *              Pair consisting the result & response, contains valid response when result = kStatusOk
   * @remarks     Implemented requirements:
   *              DiagClientLib-VehicleDiscovery
   */
  virtual std::pair<VehicleResponseResult, diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr>
  SendVehicleIdentificationRequest(diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) = 0;

  /**
   * @brief       Function to get required diag client conversation object based on conversation name
   * @param[in]   conversation_name
   *              Name of conversation configured as json parameter "ConversationName"
   * @return      DiagClientConversation&
   *              Reference to diag client conversation
   * @remarks     Implemented requirements:
   *              DiagClientLib-MultipleTester-Connection, DiagClientLib-Conversation-Construction
   */
  virtual diag::client::conversation::DiagClientConversation& GetDiagnosticClientConversation(
      std::string_view conversation_name) = 0;
};

}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H
