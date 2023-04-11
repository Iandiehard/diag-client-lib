/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
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
class DoipNetworkType {
public:
  std::string tcpIpAddress;
};

class ConversationType {
public:
  uint16_t p2ClientMax;
  uint16_t p2StarClientMax;
  uint16_t txBufferSize;
  uint16_t rxBufferSize;
  uint16_t sourceAddress;
  uint16_t targetAddress;
  std::string conversationName;
  DoipNetworkType network;
};

class ConversationConfig {
public:
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