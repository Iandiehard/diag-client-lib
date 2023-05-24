/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONFIG_PARSER_CONFIG_PARSER_TYPE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONFIG_PARSER_CONFIG_PARSER_TYPE_H
/* includes */
#include "common_header.h"
#include "parser/json_parser.h"

namespace diag {
namespace client {
namespace config_parser {

// Doip network property type
struct DoipNetworkType {
  // local tcp address
  std::string tcp_ip_address;
};

// Properties of a single conversation
struct ConversationType {
  // store p2 client timeout
  std::uint16_t p2_client_max;
  // store p2 star client timeout
  std::uint16_t p2_star_client_max;
  // store receive buffer size
  std::uint16_t rx_buffer_size;
  // store source address of client
  std::uint16_t source_address;
  // store the client conversation name
  std::string conversation_name;
  // store the doip network item
  DoipNetworkType network;
};

// Properties of diag client configuration
struct DcmClientConfig {
  // local udp address
  std::string udp_ip_address;
  // broadcast address
  std::string udp_broadcast_address;
  // number of conversation
  std::uint8_t num_of_conversation;
  // store all conversations
  std::vector<ConversationType> conversations;
};

/**
 * @brief         Function to read from config tree and get the DcmClient configuration
 * @param[in]     config_tree
 *                The reference to config tree containing the configuration
 * @return        The Dcm client configuration
 */
diag::client::config_parser::DcmClientConfig ReadDcmClientConfig(boost_support::parser::boost_tree &config_tree) {
  diag::client::config_parser::DcmClientConfig config{};
  // get the udp info for vehicle discovery
  config.udp_ip_address = config_tree.get<std::string>("UdpIpAddress");
  config.udp_broadcast_address = config_tree.get<std::string>("UdpBroadcastAddress");
  // get total number of conversation
  config.num_of_conversation = config_tree.get<std::uint8_t>("Conversation.NumberOfConversation");
  // loop through all the conversation
  for (boost_support::parser::boost_tree::value_type &conversation_ptr:
       config_tree.get_child("Conversation.ConversationProperty")) {
    diag::client::config_parser::ConversationType conversation{};
    conversation.conversation_name = conversation_ptr.second.get<std::string>("ConversationName");
    conversation.p2_client_max = conversation_ptr.second.get<std::uint16_t>("p2ClientMax");
    conversation.p2_star_client_max = conversation_ptr.second.get<std::uint16_t>("p2StarClientMax");
    conversation.rx_buffer_size = conversation_ptr.second.get<std::uint16_t>("RxBufferSize");
    conversation.source_address = conversation_ptr.second.get<std::uint16_t>("SourceAddress");
    conversation.network.tcp_ip_address = conversation_ptr.second.get<std::string>("Network.TcpIpAddress");
    config.conversations.emplace_back(conversation);
  }
  return config;
}

}  // namespace config_parser
}  // namespace client
}  // namespace diag
#endif  //  DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONFIG_PARSER_CONFIG_PARSER_TYPE_H