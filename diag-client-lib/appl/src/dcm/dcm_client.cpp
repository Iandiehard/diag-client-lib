/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "src/dcm/dcm_client.h"

#include <optional>

#include "src/common/logger.h"

namespace diag {
namespace client {
namespace dcm {

// string representing of vehicle discovery conversation name
constexpr std::string_view VehicleDiscoveryConversation{"VehicleDiscovery"};

DCMClient::DCMClient(config_parser::DcmClientConfig dcm_client_config)
    : DiagnosticManager{},
      uds_transport_protocol_mgr_{std::make_unique<uds_transport::UdsTransportProtocolManager>()},
      conversation_mgr_{std::make_unique<conversation_manager::ConversationManager>(std::move(dcm_client_config),
                                                                                    *uds_transport_protocol_mgr_)},
      vehicle_discovery_conversation_{std::move(
          conversation_mgr_->GetDiagnosticClientVehicleDiscoveryConversation(VehicleDiscoveryConversation).value())} {}

DCMClient::~DCMClient() noexcept = default;

void DCMClient::Initialize() noexcept {
  // start Vehicle Discovery
  vehicle_discovery_conversation_->Startup();
  // start Conversation Manager
  conversation_mgr_->Startup();
  // start all the udsTransportProtocol Layer
  uds_transport_protocol_mgr_->Startup();
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client Initialized"; });
}

void DCMClient::Run() noexcept {
  // run udsTransportProtocol layer
  uds_transport_protocol_mgr_->Run();
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client is ready to serve"; });
}

void DCMClient::Shutdown() noexcept {
  // shutdown Vehicle Discovery
  vehicle_discovery_conversation_->Shutdown();
  // shutdown Conversation Manager
  conversation_mgr_->Shutdown();
  // shutdown udsTransportProtocol layer
  uds_transport_protocol_mgr_->Shutdown();
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client Shutdown completed"; });
}

core_type::Result<diag::client::conversation::DiagClientConversation &, DiagClient::ConversationErrorCode>
DCMClient::GetDiagnosticClientConversation(std::string_view conversation_name) noexcept {
  core_type::Result<diag::client::conversation::DiagClientConversation &, DiagClient::ConversationErrorCode>
      conversation_result{
          core_type::Result<diag::client::conversation::DiagClientConversation &, DiagClient::ConversationErrorCode>::
              FromError(DiagClient::ConversationErrorCode::kNoConversationFound)};
  // try to get the conversation from conversation manager
  std::optional<std::unique_ptr<diag::client::conversation::DiagClientConversation>> conversation{
      conversation_mgr_->GetDiagnosticClientConversation(conversation_name)};
  if (conversation) {
    // insert the conversation object to map and return the reference to it
    conversation_result.EmplaceValue(*conversation);
    conversation_map_.insert(
        std::pair<std::string, std::unique_ptr<diag::client::conversation::DiagClientConversation>>{
            conversation_name, std::move(conversation.value())});
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
        __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
          msg << "Diagnostic Client conversation created with name: " << conversation_name;
        });
  } else {
    // no conversation found
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatal(
        __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
          msg << "Diagnostic Client conversation not found with name: " << conversation_name;
        });
  }
  return conversation_result;
}

core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                  DiagClient::VehicleInfoResponseErrorCode>
DCMClient::SendVehicleIdentificationRequest(
    diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept {
  return vehicle_discovery_conversation_->SendVehicleIdentificationRequest(vehicle_info_request);
}

}  // namespace dcm
}  // namespace client
}  // namespace diag
