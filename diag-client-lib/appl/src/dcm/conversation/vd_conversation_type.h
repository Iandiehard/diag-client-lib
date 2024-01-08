/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_TYPE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_TYPE_H

#include <cstdint>
#include <string>

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief       Structure containing VD conversation type
 */
struct VDConversationType {
  /**
   * @brief       The Udp IP address of conversation
   */
  std::string udp_address{};

  /**
   * @brief       The Udp broadcast IP address of conversation
   */
  std::string udp_broadcast_address{};

  /**
   * @brief       The Port number of conversation
   */
  uint16_t port_num{};

  /**
   * @brief       The handle id of conversation
   */
  std::uint8_t handler_id{};
};

}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_TYPE_H
