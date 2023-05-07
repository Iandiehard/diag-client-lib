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

#include "src/common/diagnostic_manager.h"
#include "src/dcm/config_parser/config_parser_type.h"
#include "src/dcm/connection/uds_transport_protocol_manager.h"
#include "src/dcm/conversion/conversation_manager.h"

namespace diag {
namespace client {
namespace dcm {
/*
 @ Class Name        : DCM Client
 @ Class Description : Class to create Diagnostic Manager Client functionality                           
 */
class DCMClient final : public diag::client::common::DiagnosticManager {
public:
  //ctor
  explicit DCMClient(diag::client::common::property_tree &ptree);

  //dtor
  ~DCMClient() override;

  // Initialize
  void Initialize() override;

  // Run
  void Run() override;

  // Shutdown
  void Shutdown() override;

  // Function to get the diagnostic client conversation
  diag::client::conversation::DiagClientConversation &GetDiagnosticClientConversation(
      std::string_view conversation_name) override;

  // Send Vehicle Identification Request and get response
  std::pair<diag::client::DiagClient::VehicleResponseResult,
            diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr>
  SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) override;

private:
  // uds transport protocol Manager
  std::unique_ptr<uds_transport::UdsTransportProtocolManager> uds_transport_protocol_mgr;
  // conversation manager
  std::unique_ptr<conversation_manager::ConversationManager> conversation_mgr;
  // map to store conversation pointer along with conversation name
  std::unordered_map<std::string, std::unique_ptr<diag::client::conversation::DiagClientConversation>>
      diag_client_conversation_map;
  // store the diag client conversation for vehicle discovery
  std::unique_ptr<conversation::VdConversation> diag_client_vehicle_discovery_conversation;

  // function to read from property tree to config structure
  diag::client::config_parser::ConversationConfig GetConversationConfig(diag::client::common::property_tree &ptree);
};
}  // namespace dcm
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_COMMUNICATION_MANAGER_H
