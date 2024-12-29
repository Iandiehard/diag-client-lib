/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip-client/channel/tcp_channel/state_machine/states/channel_state_registered.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace state_machine {
namespace states {

ChannelStateRegistered::ChannelStateRegistered() noexcept
    : ChannelStateBaseState(ChannelStateHandle::kRegistered) {}

void ChannelStateRegistered::OnEnter() noexcept {
  // Store the active connection
}

void ChannelStateRegistered::OnExit() noexcept {
  // Reset the active connection
}

}  // namespace states
}  // namespace state_machine
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
