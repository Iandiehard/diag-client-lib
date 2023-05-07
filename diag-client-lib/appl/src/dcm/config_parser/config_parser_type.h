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
#include "common_Header.h"

namespace diag {
namespace client {
namespace config_parser {

struct DoipNetworkType {
  std::string tcpIpAddress;
};

struct ConversationType {
  uint16_t p2ClientMax;
  uint16_t p2StarClientMax;
  uint16_t rxBufferSize;
  uint16_t sourceAddress;
  std::string conversationName;
  DoipNetworkType network;
};

struct ConversationConfig {
  // local udp address
  std::string udp_ip_address;
  // broadcast address
  std::string udp_broadcast_address;
  // number of conversation
  uint8_t num_of_conversation;
  // store all conversations
  std::vector<ConversationType> conversations;
};
}  // namespace config_parser
}  // namespace client
}  // namespace diag
#endif  //  DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONFIG_PARSER_CONFIG_PARSER_TYPE_H