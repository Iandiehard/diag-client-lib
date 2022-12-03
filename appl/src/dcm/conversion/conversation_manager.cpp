/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* includes */
#include "dcm/conversion/conversation_manager.h"
#include "dcm/conversion/dm_conversation.h"

namespace diag {
namespace client {
namespace conversation_manager{

//ctor
ConversationManager::ConversationManager(
            diag::client::config_parser::ConversationConfig config,
            diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr)
                 : uds_transport_mgr_e(uds_transport_mgr) {
    CreateConversationConfig(config);
}

// Initialize
void ConversationManager::Startup() {
}

// Shutdown
void ConversationManager::Shutdown() {
}

// Get the required conversion
std::unique_ptr<diag::client::conversation::DmConversation>
    ConversationManager::GetDiagnosticClientConversion(std::string conversation_name) {
    std::unique_ptr<diag::client::conversation::DmConversation> dm_conversation {};
    auto it = conversation_config_e.find(conversation_name);
    if(it != conversation_config_e.end()) {
        dm_conversation = std::make_unique<diag::client::conversation::DmConversation>(
                                                                it->first,
                                                                it->second);
        // Register the connection
        dm_conversation->RegisterConnection(
            uds_transport_mgr_e.doip_transport_handler->FindorCreateConnection(
                                      dm_conversation->dm_conversion_handler,
                                            it->second.tcp_address,
                                            it->second.udp_address,
                                       it->second.port_num
            ));
    }
    return dm_conversation;
}

// function to find or create conversion
void ConversationManager::CreateConversationConfig(
    diag::client::config_parser::ConversationConfig config) {
    for(uint8_t conv_count = 0; conv_count < config.num_of_conversation; conv_count ++) {
        ::ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier;
        conversion_identifier.tx_buffer_size = config.conversations[conv_count].txBufferSize;
        conversion_identifier.rx_buffer_size = config.conversations[conv_count].rxBufferSize;
        conversion_identifier.p2_client_max = config.conversations[conv_count].p2ClientMax;
        conversion_identifier.p2_star_client_max = config.conversations[conv_count].p2StarClientMax;
        conversion_identifier.source_address = config.conversations[conv_count].sourceAddress;
        conversion_identifier.target_address = config.conversations[conv_count].targetAddress;
        conversion_identifier.tcp_address = config.conversations[conv_count].network.tcpIpAddress;
        conversion_identifier.udp_address = config.conversations[conv_count].network.udpIpAddress;
        conversion_identifier.udp_broadcast_address = config.conversations[conv_count].network.udpBroadcastAddress;
        conversion_identifier.port_num = config.conversations[conv_count].network.portNum;
        // push to config map
        conversation_config_e.insert(
            std::pair<std::string, ::ara::diag::conversion_manager::ConversionIdentifierType>(
                                                    config.conversations[conv_count].conversationName,
                                                    conversion_identifier));
    }
}

} // conversation_manager
} // client
} // diag