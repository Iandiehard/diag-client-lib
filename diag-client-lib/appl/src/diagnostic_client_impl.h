/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_CLIENT_IMPL_H_
#define DIAG_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_CLIENT_IMPL_H_

#include <memory>
#include <string_view>

#include "common/diagnostic_manager.h"
#include "dcm/dcm_client.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_conversation.h"

namespace diag {
namespace client {

/**
 * @brief    Class to provide implementation of diag client 
 */
class DiagClientImpl final {
public:
  /**
   * @brief         Constructs an instance of DiagClient
   * @param[in]     diag_client_config_path
   *                path to diag client config file
   * @remarks       Implemented requirements:
   *                DiagClientLib-Construction
   */
  explicit DiagClientImpl(std::string_view diag_client_config_path) noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  DiagClientImpl(const DiagClientImpl &other) noexcept = delete;
  DiagClientImpl &operator=(const DiagClientImpl &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  DiagClientImpl(DiagClientImpl &&other) noexcept = delete;
  DiagClientImpl &operator=(DiagClientImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of DiagClient
   * @remarks       Implemented requirements:
   *                DiagClientLib-Destruction
   */
  ~DiagClientImpl() noexcept = default;

  /**
   * @brief        Function to initialize the already created instance of DiagClient
   * @details      Must be called once and before using any other functions of DiagClient
   * @return       Result with void in case of success, otherwise error is returned
   * @remarks      Implemented requirements:
   *               DiagClientLib-Initialization
   */
  Result<void, DiagClient::InitDeInitErrorCode> Initialize() noexcept;

  /**
   * @brief        Function to de-initialize the already initialized instance of DiagClient
   * @details      Must be called during shutdown phase, no further processing of any
   *               function will be allowed after this call
   * @return       Result with void in case of success, otherwise error is returned
   * @remarks      Implemented requirements:
   *               DiagClientLib-DeInitialization
   */
  Result<void, DiagClient::InitDeInitErrorCode> DeInitialize() noexcept;

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   * @remarks     Implemented requirements:
   *              DiagClientLib-VehicleDiscovery
   */
  Result<vehicle_info::VehicleInfoMessageResponseUniquePtr, DiagClient::VehicleInfoResponseErrorCode>
  SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept;

  /**
   * @brief       Function to get required diag client conversation object based on conversation name
   * @param[in]   conversation_name
   *              Name of conversation configured as json parameter "ConversationName"
   * @return      Result containing reference to diag client conversation as per passed conversation name, otherwise error
   * @remarks     Implemented requirements:
   *              DiagClientLib-MultipleTester-Connection, DiagClientLib-Conversation-Construction
   */
  Result<conversation::DiagClientConversation&, DiagClient::ConversationErrorCode> GetDiagnosticClientConversation(
      std::string_view conversation_name) noexcept;

private:
  /**
   * @brief    Unique pointer to dcm client instance
   */
  std::unique_ptr<diag::client::common::DiagnosticManager> dcm_instance_;

  /**
   * @brief    Thread to handle dcm client lifecycle
   */
  std::thread dcm_thread_;
};
}  // namespace client
}  // namespace diag
#endif  // DIAG_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_CLIENT_IMPL_H_
