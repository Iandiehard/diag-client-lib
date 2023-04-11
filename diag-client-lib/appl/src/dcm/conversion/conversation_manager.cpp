/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "src/dcm/conversion/conversation_manager.h"

#include "src/dcm/conversion/dm_conversation.h"

namespace diag {
namespace client {
namespace conversation_manager {
//ctor
ConversationManager::ConversationManager(diag::client::config_parser::ConversationConfig config,
                                         diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr)
    : uds_transport_mgr_(uds_transport_mgr) {
  CreateConversationConfig(config);
}

// Initialize
void ConversationManager::Startup() {}

// Shutdown
void ConversationManager::Shutdown() {}

// Get the required conversation
std::unique_ptr<diag::client::conversation::DmConversation> ConversationManager::GetDiagnosticClientConversion(
    std::string conversation_name) {
  std::unique_ptr<diag::client::conversation::DmConversation> dm_conversation{};
  auto it = conversation_config_.find(conversation_name);
  if (it != conversation_config_.end()) {
    // create the conversation
    dm_conversation = std::make_unique<diag::client::conversation::DmConversation>(it->first, it->second);
    // Register the connection
    dm_conversation->RegisterConnection(uds_transport_mgr_.doip_transport_handler->FindOrCreateTcpConnection(
        dm_conversation->dm_conversion_handler_, it->second.tcp_address, it->second.port_num));
  }
  return dm_conversation;
}

std::unique_ptr<diag::client::conversation::VdConversation>
ConversationManager::GetDiagnosticClientVehicleDiscoveryConversation(std::string &conversation_name) {
  std::unique_ptr<diag::client::conversation::VdConversation> vd_conversation{};
  auto it = vd_conversation_config_.find(conversation_name);
  if (it != vd_conversation_config_.end()) {
    // create the conversation
    vd_conversation = std::make_unique<diag::client::conversation::VdConversation>(it->first, it->second);
    // Register the connection
    vd_conversation->RegisterConnection(uds_transport_mgr_.doip_transport_handler->FindOrCreateUdpConnection(
        vd_conversation->GetConversationHandler(), it->second.udp_address, it->second.port_num));
  }
  return vd_conversation;
}

// function to find or create conversation
void ConversationManager::CreateConversationConfig(diag::client::config_parser::ConversationConfig &config) {
  {  // Create Vehicle discovery config
    ::ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier{};
    conversion_identifier.udp_address = config.udp_ip_address;
    conversion_identifier.udp_broadcast_address = config.udp_broadcast_address;
    conversion_identifier.port_num = 0U;  // random selection of port number
    (void) vd_conversation_config_.insert(
        std::pair<std::string, ::ara::diag::conversion_manager::ConversionIdentifierType>("VehicleDiscovery",
                                                                                          conversion_identifier));
  }

  {  // Create Conversation config
    for (uint8_t conv_count = 0U; conv_count < config.num_of_conversation; conv_count++) {
      ::ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier{};
      conversion_identifier.tx_buffer_size = config.conversations[conv_count].txBufferSize;
      conversion_identifier.rx_buffer_size = config.conversations[conv_count].rxBufferSize;
      conversion_identifier.p2_client_max = config.conversations[conv_count].p2ClientMax;
      conversion_identifier.p2_star_client_max = config.conversations[conv_count].p2StarClientMax;
      conversion_identifier.source_address = config.conversations[conv_count].sourceAddress;
      conversion_identifier.target_address = config.conversations[conv_count].targetAddress;
      conversion_identifier.tcp_address = config.conversations[conv_count].network.tcpIpAddress;
      conversion_identifier.port_num = 0U;  // random selection of port number
      // push to config map
      (void) conversation_config_.insert(
          std::pair<std::string, ::ara::diag::conversion_manager::ConversionIdentifierType>(
              config.conversations[conv_count].conversationName, conversion_identifier));
    }
  }
}

}  // namespace conversation_manager
}  // namespace client
}  // namespace diag