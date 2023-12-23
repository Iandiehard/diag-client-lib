/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H_
#define DIAG_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H_

#include <functional>
#include <memory>
#include <string_view>

#include "include/diagnostic_client_conversation.h"
#include "include/diagnostic_client_result.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"

namespace diag {
namespace client {

/**
 * @brief    Class to manage Diagnostic Client
 */
class DiagClient final {
 public:
  /**
   * @brief  Definitions of Vehicle Identification response error code
   */
  enum class VehicleInfoResponseError : std::uint8_t {
    kTransmitFailed = 0U,     /**< Failure on Transmissions */
    kInvalidParameters = 1U,  /**< Invalid Parameter passed */
    kNoResponseReceived = 2U, /**< No vehicle identification response received */
  };

 public:
  /**
   * @brief         Constructs an instance of DiagClient
   * @param[in]     diag_client_config_path
   *                path to diag client config file
   * @implements    DiagClientLib-Construction
   */
  explicit DiagClient(std::string_view diag_client_config_path) noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  DiagClient(const DiagClient &other) noexcept = delete;
  DiagClient &operator=(const DiagClient &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  DiagClient(DiagClient &&other) noexcept = delete;
  DiagClient &operator=(DiagClient &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of DiagClient
   * @implements    DiagClientLib-Destruction
   */
  ~DiagClient() noexcept;

  /**
   * @brief        Function to initialize the already created instance of DiagClient
   * @details      Must be called once and before using any other functions of DiagClient
   * @return       Result with void in case of success, otherwise error is returned
   * @implements   DiagClientLib-Initialization
   */
  Result<void> Initialize() noexcept;

  /**
   * @brief        Function to de-initialize the already initialized instance of DiagClient
   * @details      Must be called during shutdown phase, no further processing of any
   *               function will be allowed after this call
   * @return       Result with void in case of success, otherwise error is returned
   * @implements   DiagClientLib-DeInitialization
   */
  Result<void> DeInitialize() noexcept;

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   * @implements  DiagClientLib-VehicleDiscovery
   */
  Result<vehicle_info::VehicleInfoMessageResponseUniquePtr, VehicleInfoResponseError> SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept;

  /**
   * @brief       Function to get required diag client conversation object based on conversation name
   * @param[in]   conversation_name
   *              Name of conversation configured as json parameter "ConversationName"
   * @return      Diag client conversation object
   * @implements  DiagClientLib-MultipleTester-Connection, DiagClientLib-Conversation-Construction
   */
  conversation::DiagClientConversation GetDiagnosticClientConversation(std::string_view conversation_name) noexcept;

 private:
  /**
   * @brief    Forward declaration of diag client implementation
   */
  class DiagClientImpl;

  /**
   * @brief    Unique pointer to diag client implementation
   */
  std::unique_ptr<DiagClientImpl> diag_client_impl_;
};

}  // namespace client
}  // namespace diag
#endif  // DIAG_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H_
