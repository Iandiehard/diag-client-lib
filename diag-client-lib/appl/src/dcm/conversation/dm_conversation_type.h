/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_TYPE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_TYPE_H

#include <cstdint>
#include <string>

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief       Structure containing DM conversation type
 */
struct DMConversationType {
  /**
   * @brief       The reception buffer size type
   */
  std::uint32_t rx_buffer_size{};

  /**
   * @brief       The maximum p2 client time
   */
  uint16_t p2_client_max{};

  /**
   * @brief       The maximum p2 star client time
   */
  uint16_t p2_star_client_max{};

  /**
   * @brief       The source address of conversation
   */
  uint16_t source_address{};

  /**
   * @brief       The Tcp IP address of conversation
   */
  std::string tcp_address{};

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
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_TYPE_H
