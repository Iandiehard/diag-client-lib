/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_COMMUNICATION_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_COMMUNICATION_MANAGER_H
/* includes */
#include <string_view>

#include "core/include/result.h"
#include "src/common/diagnostic_manager.h"
#include "src/dcm/config_parser/config_parser_type.h"
#include "src/dcm/connection/uds_transport_protocol_manager.h"
#include "src/dcm/conversation/conversation_manager.h"

namespace diag {
namespace client {
namespace dcm {

/**
 * @brief    Class to create Diagnostic Manager Client functionality
 */
class DCMClient final : public diag::client::common::DiagnosticManager {
public:
  /**
   * @brief         Constructs an instance of DCMClient
   * @param[in]     dcm_client_config
   *                The configuration of dcm client
   */
  explicit DCMClient(config_parser::DcmClientConfig dcm_client_config);

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  DCMClient(const DCMClient &other) noexcept = delete;
  DCMClient &operator=(const DCMClient &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  DCMClient(DCMClient &&other) noexcept = delete;
  DCMClient &operator=(DCMClient &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of DCMClient
   */
  ~DCMClient() noexcept override;

  /**
   * @brief         Function to initialize the DCMClient
   */
  void Initialize() noexcept override;

  /**
   * @brief         Function to run DCMClient
   */
  void Run() noexcept override;

  /**
   * @brief         Function to shutdown the DCMClient
   */
  void Shutdown() noexcept override;

  /**
   * @brief       Function to get required diag client conversation object based on conversation name
   * @param[in]   conversation_name
   *              Name of conversation configured as json parameter "ConversationName"
   * @return      Diag client conversation object as per passed conversation name
   */
  conversation::DiagClientConversation GetDiagnosticClientConversation(
      std::string_view conversation_name) noexcept override;

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   */
  core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                    DiagClient::VehicleInfoResponseError>
  SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept override;

private:
  /**
   * @brief         Stores the uds transport protocol manager
   */
  std::unique_ptr<uds_transport::UdsTransportProtocolManager> uds_transport_protocol_mgr_;

  /**
   * @brief         Stores the conversation manager instance
   */
  conversation_manager::ConversationManager conversation_mgr_;

  /**
   * @brief         Store the conversation for vehicle discovery
   */
  conversation::Conversation &vehicle_discovery_conversation_;
};

/**
 * @brief       Function to get the reference to conversation manager
 * @return      Reference to conversation manager
 */
auto GetConversationManager() noexcept -> conversation_manager::ConversationManager &;

}  // namespace dcm
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_COMMUNICATION_MANAGER_H
