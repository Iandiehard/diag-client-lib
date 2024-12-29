/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_STATES_CHANNEL_STATE_REGISTERED_H_
#define DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_STATES_CHANNEL_STATE_REGISTERED_H_

#include "doip-client/channel/tcp_channel/state_machine/states/channel_state_handle.h"
#include "utility-support/state_machine/base_state/base_state.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace state_machine {
namespace states {

/**
 * @brief  Type alias of registered base state
 */
class ChannelStateRegistered final : public ChannelStateBaseState {
 public:
  ChannelStateRegistered() noexcept;

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  ChannelStateRegistered(const ChannelStateRegistered &other) noexcept = delete;
  ChannelStateRegistered &operator=(const ChannelStateRegistered &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  ChannelStateRegistered(ChannelStateRegistered &&other) noexcept = delete;
  ChannelStateRegistered &operator=(ChannelStateRegistered &&other) noexcept = delete;

  /**
   * @brief  Destruct the instance of base state
   */
  ~ChannelStateRegistered() noexcept = default;

  /**
   * @brief     Function that is called when entering the state
   */
  void OnEnter() noexcept final;

  /**
   * @brief     Function that is called when exiting the state
   */
  void OnExit() noexcept final;
};

}  // namespace states
}  // namespace state_machine
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client

#endif  // DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_STATES_CHANNEL_STATE_REGISTERED_H_
