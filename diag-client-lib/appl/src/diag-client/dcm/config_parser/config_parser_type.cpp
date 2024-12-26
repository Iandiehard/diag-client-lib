/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/* includes */
#include "diag-client/dcm/config_parser/config_parser_type.h"

namespace diag {
namespace client {
namespace config_parser {

diag::client::config_parser::DcmClientConfig ReadDcmClientConfig(
    boost_support::parser::boost_tree &config_tree) {
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
    conversation.p2_client_max = conversation_ptr.second.get<std::uint16_t>("P2ClientMax");
    conversation.p2_star_client_max = conversation_ptr.second.get<std::uint16_t>("P2StarClientMax");
    conversation.rx_buffer_size = conversation_ptr.second.get<std::uint16_t>("RxBufferSize");
    conversation.source_address = conversation_ptr.second.get<std::uint16_t>("SourceAddress");
    conversation.network.tcp_ip_address =
        conversation_ptr.second.get<std::string>("Network.TcpIpAddress");
    conversation.network.tls_handling = conversation_ptr.second.get<bool>("Network.TlsHandling");
    config.conversations.emplace_back(conversation);
  }
  return config;
}

}  // namespace config_parser
}  // namespace client
}  // namespace diag
