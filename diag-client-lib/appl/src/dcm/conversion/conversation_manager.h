/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
/* includes */
#include <string_view>

#include "common_header.h"
#include "src/dcm/config_parser/config_parser_type.h"
#include "src/dcm/connection/uds_transport_protocol_manager.h"
#include "src/dcm/conversion/dm_conversation.h"
#include "src/dcm/conversion/vd_conversation.h"

namespace diag {
namespace client {
namespace conversation_manager {
/*
 @ Class Name        : ConversationManager
 @ Class Description : Class to manage all the conversion created from usr request                           
 */
class ConversationManager {
public:
  // ctor
  ConversationManager(diag::client::config_parser::ConversationConfig config,
                      diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr);

  // dtor
  ~ConversationManager() = default;

  // startup
  void Startup();

  // shutdown
  void Shutdown();

  // Get the required conversion
  std::unique_ptr<diag::client::conversation::DmConversation> GetDiagnosticClientConversation(
      std::string_view conversion_name);

  // Get the required conversion
  std::unique_ptr<diag::client::conversation::VdConversation> GetDiagnosticClientVehicleDiscoveryConversation(
      std::string_view conversion_name);

private:
  // store uds transport manager
  uds_transport::UdsTransportProtocolManager &uds_transport_mgr_;

  // store the conversion name with conversion configurations
  std::map<std::string, ::uds_transport::conversion_manager::ConversionIdentifierType> conversation_config_;

  // store the vehicle discovery with conversation configuration
  std::map<std::string, ::uds_transport::conversion_manager::ConversionIdentifierType> vd_conversation_config_;

  // function to create or find new conversion
  void CreateConversationConfig(diag::client::config_parser::ConversationConfig &config);
};
}  // namespace conversation_manager
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
