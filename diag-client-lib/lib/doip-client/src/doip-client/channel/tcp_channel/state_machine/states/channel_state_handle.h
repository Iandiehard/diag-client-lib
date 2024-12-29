/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_STATES_CHANNEL_STATE_HANDLE_H_
#define DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_STATES_CHANNEL_STATE_HANDLE_H_

#include <cstdint>

#include "utility-support/state_machine/base_state/base_state.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace state_machine {
namespace states {

/**
 * @brief  Different channel state
 */
enum class ChannelStateHandle : std::uint8_t {
  kInitialized = 0u,  //
  kConnected = 1u,    // Tcp connection between client and server established
  kRegistered = 2u,   // Routing activation successful
  kFinalized = 3u     //
};

/**
 * @brief  Type alias of base state
 */
using ChannelStateBaseState =
    utility_support::state_machine::base_state::BaseState<ChannelStateHandle>;

}  // namespace states
}  // namespace state_machine
}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  // DOIP_CLIENT_CHANNEL_TCP_CHANNEL_STATE_MACHINE_STATES_CHANNEL_STATE_HANDLE_H_
