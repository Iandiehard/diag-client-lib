/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_DOIP_TCP_CHANNEL_STATE_CONTEXT_H_
#define DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_DOIP_TCP_CHANNEL_STATE_CONTEXT_H_

#include "doip-client/channel/tcp_channel/state_machine/states/channel_state_handle.h"
#include "utility-support/state_machine/state_context.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace state_machine {

class DoipTcpChannelStateContext final
    : public utility_support::state_machine::StateContext<states::ChannelStateHandle> {
 public:
  DoipTcpChannelStateContext() noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  DoipTcpChannelStateContext(const DoipTcpChannelStateContext &other) noexcept = delete;
  DoipTcpChannelStateContext &operator=(const DoipTcpChannelStateContext &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  DoipTcpChannelStateContext(DoipTcpChannelStateContext &&other) noexcept = delete;
  DoipTcpChannelStateContext &operator=(DoipTcpChannelStateContext &&other) noexcept = delete;

  /**
   * @brief  Destructor
   */
  ~DoipTcpChannelStateContext() noexcept = default;
};

}  // namespace state_machine
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client

#endif  // DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_DOIP_TCP_CHANNEL_STATE_CONTEXT_H_
