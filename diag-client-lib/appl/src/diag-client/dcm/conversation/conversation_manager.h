/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
/* includes */
#include <optional>
#include <string_view>
#include <variant>

#include "diag-client/dcm/config_parser/config_parser_type.h"
#include "diag-client/dcm/connection/uds_transport_protocol_manager.h"
#include "diag-client/dcm/conversation/conversation.h"
#include "diag-client/dcm/conversation/dm_conversation_type.h"
#include "diag-client/dcm/conversation/vd_conversation.h"
#include "diag-client/dcm/conversation/vd_conversation_type.h"

namespace diag {
namespace client {
namespace conversation_manager {

/**
 * @brief               Class to manage all the conversation created from usr request
 */
class ConversationManager final {
 public:
  /**
   * @brief         Constructs an instance of ConversationManager
   * @param[in]     config
   *                The configuration of dcm client
   * @param[in]     uds_transport_mgr
   *                The reference to Uds transport manger
   */
  ConversationManager(
      diag::client::config_parser::DcmClientConfig config,
      diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr) noexcept;

  /**
   * @brief         Destructs an instance of ConversationManager
   */
  ~ConversationManager() noexcept = default;

  /**
   * @brief         Function to start the ConversationManager
   */
  void Startup() noexcept;

  /**
   * @brief         Function to shutdown the ConversationManager
   */
  void Shutdown() noexcept;

  /**
   * @brief       Function to get DM conversation object based on conversation name
   * @param[in]   conversation_name
   *              The conversation name
   * @return      The reference to diag client conversation as per provided conversation name
   */
  diag::client::conversation::Conversation &GetDiagnosticClientConversation(
      std::string_view conversation_name) noexcept;

 private:
  /**
   * @brief      Store Dm conversation
   */
  struct ConversationStorage {
    /**
     * @brief      Store conversation type
     */
    std::variant<conversation::DMConversationType, conversation::VDConversationType>
        conversation_type{};

    /**
     * @brief      Store pointer to conversation object
     */
    std::unique_ptr<diag::client::conversation::Conversation> conversation{};
  };

  /**
   * @brief         Store the reference to uds transport manager
   */
  uds_transport::UdsTransportProtocolManager &uds_transport_mgr_;

  /**
   * @brief         Map to store conversation object(dm) along with conversation name
   * @details       Pair of Conversation name and Conversation Object
   */
  std::unordered_map<std::string, ConversationStorage> conversation_map_;

  /**
   * @brief       Function to store the dcm client configuration internally
   * @param[in]   config
   *              The Dcm client configuration
   */
  void StoreConversationConfig(diag::client::config_parser::DcmClientConfig &config) noexcept;
};
}  // namespace conversation_manager
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
