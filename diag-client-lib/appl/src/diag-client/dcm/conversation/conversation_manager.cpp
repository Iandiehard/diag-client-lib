/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "diag-client/dcm/conversation/conversation_manager.h"

#include "core/include/variant_helper.h"
#include "diag-client/common/logger.h"
#include "diag-client/dcm/conversation/dm_conversation.h"

namespace diag {
namespace client {
namespace conversation_manager {
namespace {

/**
  * @brief         Needed to create random port number from client side
  */
constexpr std::uint16_t kRandomPortNumber{0u};

/**
  * @brief        The conversation name for Vehicle discovery
  */
constexpr std::string_view kVdConversationName{"VdConversation"};
}  // namespace

ConversationManager::ConversationManager(
    diag::client::config_parser::DcmClientConfig config,
    diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr) noexcept
    : uds_transport_mgr_{uds_transport_mgr} {
  // store the conversation config (vd & dm) out of passed config
  StoreConversationConfig(config);
}

void ConversationManager::Startup() noexcept {}

void ConversationManager::Shutdown() noexcept {
  // Loop through available conversation and check if already in shutdown state
  for (std::unordered_map<std::string, ConversationStorage>::value_type const &conversation: conversation_map_) {
    if (conversation.second.conversation != nullptr) {
      if (conversation.second.conversation->GetActivityStatus() !=
          conversation::Conversation::ActivityStatusType::kInactive) {
        // Shutdown is not called on the conversation by user, log warning and perform shutdown
        logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
            __FILE__, __LINE__, "", [&conversation](std::stringstream &msg) {
              msg << "'" << conversation.first << "'"
                  << "-> "
                  << "Shutdown is not triggered by user, will be shutdown forcefully";
            });
        conversation.second.conversation->Shutdown();
      }
    }
  }
}

diag::client::conversation::Conversation &ConversationManager::GetDiagnosticClientConversation(
    std::string_view conversation_name) noexcept {
  // find the conversation from config stored
  auto it = conversation_map_.find(std::string{conversation_name});
  if (it != conversation_map_.end()) {
    std::string const conversation_name_in_map{it->first};
    it->second.conversation = std::visit(
        core_type::visit::overloaded{
            [this, &conversation_name_in_map](conversation::DMConversationType conversation_type) noexcept {
              // Create the conversation
              std::unique_ptr<diag::client::conversation::Conversation> conversation{
                  std::make_unique<diag::client::conversation::DmConversation>(conversation_name_in_map,
                                                                               conversation_type)};
              // Register the connection
              conversation->RegisterConnection(uds_transport_mgr_.GetTransportProtocolHandler().CreateTcpConnection(
                  conversation->GetConversationHandler(), conversation_type.tcp_address, conversation_type.port_num));
              return conversation;
            },
            [this, &conversation_name_in_map](conversation::VDConversationType conversation_type) noexcept {
              // Create the conversation
              std::unique_ptr<diag::client::conversation::Conversation> conversation{
                  std::make_unique<diag::client::conversation::VdConversation>(conversation_name_in_map,
                                                                               conversation_type)};
              // Register the connection
              conversation->RegisterConnection(uds_transport_mgr_.GetTransportProtocolHandler().CreateUdpConnection(
                  conversation->GetConversationHandler(), conversation_type.udp_address, conversation_type.port_num));
              return conversation;
            }},
        it->second.conversation_type);
  } else {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatal(
        __FILE__, __LINE__, __func__, [conversation_name](std::stringstream &msg) {
          msg << "Invalid conversation name: '" << conversation_name << "', provide correct name as per config file";
        });
  }
  return *(it->second.conversation);
}

void ConversationManager::StoreConversationConfig(diag::client::config_parser::DcmClientConfig &config) noexcept {
  {  // Create Vehicle discovery config
    conversation::VDConversationType conversion_identifier{};
    conversion_identifier.udp_address = config.udp_ip_address;
    conversion_identifier.udp_broadcast_address = config.udp_broadcast_address;
    conversion_identifier.port_num = kRandomPortNumber;  // random selection of port number
    conversation_map_.emplace(kVdConversationName, ConversationStorage{conversion_identifier, nullptr});
  }

  {  // Create Conversation config
    for (std::uint8_t conv_count{0U}; conv_count < config.num_of_conversation; conv_count++) {
      conversation::DMConversationType conversion_identifier{};
      conversion_identifier.rx_buffer_size = config.conversations[conv_count].rx_buffer_size;
      conversion_identifier.p2_client_max = config.conversations[conv_count].p2_client_max;
      conversion_identifier.p2_star_client_max = config.conversations[conv_count].p2_star_client_max;
      conversion_identifier.source_address = config.conversations[conv_count].source_address;
      conversion_identifier.tcp_address = config.conversations[conv_count].network.tcp_ip_address;
      conversion_identifier.port_num = kRandomPortNumber;  // random selection of port number
      conversation_map_.emplace(config.conversations[conv_count].conversation_name,
                                ConversationStorage{conversion_identifier, nullptr});
    }
  }
}

}  // namespace conversation_manager
}  // namespace client
}  // namespace diag