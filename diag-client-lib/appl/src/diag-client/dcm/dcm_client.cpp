/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "dcm_client.h"

#include <optional>

#include "diag-client/common/logger.h"

namespace diag {
namespace client {
namespace dcm {
namespace {

/**
 * @brief    Store the conversation manager reference optionally
 */
std::optional<std::reference_wrapper<conversation_manager::ConversationManager>> conversation_manager_ref{};

/**
 * @brief    String representing of vehicle discovery conversation name
 */
constexpr std::string_view VehicleDiscoveryConversation{"VdConversation"};
}  // namespace

DCMClient::DCMClient(config_parser::DcmClientConfig dcm_client_config)
    : DiagnosticManager{},
      uds_transport_protocol_mgr_{std::make_unique<uds_transport::UdsTransportProtocolManager>()},
      conversation_mgr_{std::move(dcm_client_config), *uds_transport_protocol_mgr_},
      vehicle_discovery_conversation_{conversation_mgr_.GetDiagnosticClientConversation(VehicleDiscoveryConversation)} {
  // make the conversation manager reference available externally
  conversation_manager_ref.emplace(conversation_mgr_);
}

DCMClient::~DCMClient() noexcept = default;

void DCMClient::Initialize() noexcept {
  // start Conversation Manager
  conversation_mgr_.Startup();
  // start all the udsTransportProtocol Layer
  uds_transport_protocol_mgr_->Startup();
  // start Vehicle Discovery
  vehicle_discovery_conversation_.Startup();

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
  vehicle_discovery_conversation_.Shutdown();
  // shutdown udsTransportProtocol layer
  uds_transport_protocol_mgr_->Shutdown();
  // shutdown Conversation Manager
  conversation_mgr_.Shutdown();

  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client Shutdown completed"; });
}

conversation::DiagClientConversation DCMClient::GetDiagnosticClientConversation(
    std::string_view conversation_name) noexcept {
  return conversation::DiagClientConversation{conversation_name};
}

core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr, DiagClient::VehicleInfoResponseError>
DCMClient::SendVehicleIdentificationRequest(
    diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept {
  return vehicle_discovery_conversation_.SendVehicleIdentificationRequest(vehicle_info_request);
}

auto GetConversationManager() noexcept -> conversation_manager::ConversationManager & {
  if (!conversation_manager_ref.has_value()) {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatal(
        __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "DiagClient is not Initialized"; });
  }
  return conversation_manager_ref.value();
}

}  // namespace dcm
}  // namespace client
}  // namespace diag
