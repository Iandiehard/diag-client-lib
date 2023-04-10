/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_CLIENT_IMPL_H_
#define DIAG_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_CLIENT_IMPL_H_

#include <memory>
#include <string>

#include "common/diagnostic_manager.h"
#include "common_Header.h"
#include "dcm/diagnostic_communication_manager.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_conversation.h"
#include "libJsonParser/jsonParser.h"

namespace diag {
namespace client {
class DiagClientImpl final : public diag::client::DiagClient {
public:
  // ctor
  explicit DiagClientImpl(std::string dm_client_config);

  // dtor
  ~DiagClientImpl() = default;

  // Initialize
  void Initialize() override;

  // De-Initialize
  void DeInitialize() override;

  // Get Required Conversation based on Conversation Name
  diag::client::conversation::DiagClientConversation& GetDiagnosticClientConversation(
      std::string conversation_name) override;

  // Send Vehicle Identification Request and get response
  std::pair<diag::client::DiagClient::VehicleResponseResult, diag::client::vehicle_info::VehicleInfoMessageResponsePtr>
  SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) override;

private:
  // Used to read json
  diag::client::common::property_tree ptree;
  // dcm client instance
  std::unique_ptr<diag::client::common::DiagnosticManager> dcm_instance_ptr;
  // thread to hold dcm client instance
  std::thread dcm_thread_;
};
}  // namespace client
}  // namespace diag
#endif  // DIAG_CLIENT_LIB_APPL_SRC_DIAGNOSTIC_CLIENT_IMPL_H_
