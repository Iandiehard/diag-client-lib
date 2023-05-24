/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "src/dcm/conversation/conversation_manager.h"

namespace diag {
namespace client {
namespace conversation_manager {
//ctor
ConversationManager::ConversationManager(
    diag::client::config_parser::DcmClientConfig config,
    diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr) noexcept
    : uds_transport_mgr_{uds_transport_mgr},
      dm_conversation_config_{},
      vd_conversation_config_{} {
  // store the conversation config (vd & dm) out of passed config
  StoreConversationConfig(config);
}

void ConversationManager::Startup() noexcept {}

void ConversationManager::Shutdown() noexcept {}

std::optional<std::unique_ptr<diag::client::conversation::DmConversation>>
ConversationManager::GetDiagnosticClientConversation(std::string_view conversation_name) noexcept {
  std::optional<std::unique_ptr<diag::client::conversation::DmConversation>> opt_dm_conversation{};
  // find the conversation from config stored
  auto it = dm_conversation_config_.find(std::string{conversation_name});
  if (it != dm_conversation_config_.end()) {
    // create the conversation
    std::unique_ptr<diag::client::conversation::DmConversation> dm_conversation{
        std::make_unique<diag::client::conversation::DmConversation>(it->first, it->second)};
    // Register the connection
    dm_conversation->RegisterConnection(uds_transport_mgr_.doip_transport_handler->FindOrCreateTcpConnection(
        dm_conversation->dm_conversion_handler_, it->second.tcp_address, it->second.port_num));
    opt_dm_conversation.emplace(std::move(dm_conversation));
  }
  return opt_dm_conversation;
}

std::optional<std::unique_ptr<diag::client::conversation::VdConversation>>
ConversationManager::GetDiagnosticClientVehicleDiscoveryConversation(std::string_view conversation_name) noexcept {
  std::optional<std::unique_ptr<diag::client::conversation::VdConversation>> opt_vd_conversation{};
  // find the conversation from config stored
  auto it = vd_conversation_config_.find(std::string{conversation_name});
  if (it != vd_conversation_config_.end()) {
    // create the conversation
    std::unique_ptr<diag::client::conversation::VdConversation> vd_conversation{
        std::make_unique<diag::client::conversation::VdConversation>(it->first, it->second)};
    // Register the connection
    vd_conversation->RegisterConnection(uds_transport_mgr_.doip_transport_handler->FindOrCreateUdpConnection(
        vd_conversation->GetConversationHandler(), it->second.udp_address, it->second.port_num));
    opt_vd_conversation.emplace(std::move(vd_conversation));
  }
  assert(opt_vd_conversation.has_value());
  return opt_vd_conversation;
}

void ConversationManager::StoreConversationConfig(diag::client::config_parser::DcmClientConfig &config) noexcept {
  {  // Create Vehicle discovery config
    conversation::VDConversationType conversion_identifier{};
    conversion_identifier.udp_address = config.udp_ip_address;
    conversion_identifier.udp_broadcast_address = config.udp_broadcast_address;
    conversion_identifier.port_num = 0U;  // random selection of port number
    (void) vd_conversation_config_.insert(
        std::pair<std::string, conversation::VDConversationType>("VehicleDiscovery", conversion_identifier));
  }

  {  // Create Conversation config
    for (std::uint8_t conv_count{0U}; conv_count < config.num_of_conversation; conv_count++) {
      conversation::DMConversationType conversion_identifier{};
      conversion_identifier.rx_buffer_size = config.conversations[conv_count].rx_buffer_size;
      conversion_identifier.p2_client_max = config.conversations[conv_count].p2_client_max;
      conversion_identifier.p2_star_client_max = config.conversations[conv_count].p2_star_client_max;
      conversion_identifier.source_address = config.conversations[conv_count].source_address;
      conversion_identifier.tcp_address = config.conversations[conv_count].network.tcp_ip_address;
      conversion_identifier.port_num = 0U;  // random selection of port number
      // push to config map
      (void) dm_conversation_config_.insert(std::pair<std::string, conversation::DMConversationType>(
          config.conversations[conv_count].conversation_name, conversion_identifier));
    }
  }
}

}  // namespace conversation_manager
}  // namespace client
}  // namespace diag