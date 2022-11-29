/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "tcp_channel_state_impl.h"

namespace ara{
namespace diag{
namespace doip {
namespace tcpChannelStateImpl {

// ctor
TcpChannelStateImpl::TcpChannelStateImpl()
    : routing_activation_state_context_{std::make_unique<StateContext<routingActivationState>>()} {
    // create and add state
    // kIdle
    GetRoutingActivationStateContext().AddState(routingActivationState::kIdle,
             std::move(std::make_unique<kIdle>(routingActivationState::kIdle)));

    // kWaitForRoutingActivationRes
    GetRoutingActivationStateContext().AddState(routingActivationState::kWaitForRoutingActivationRes,
             std::move(std::make_unique<kWaitForRoutingActivationRes>(routingActivationState::kWaitForRoutingActivationRes)));

    // kRoutingActivationSuccessful
    GetRoutingActivationStateContext().AddState(routingActivationState::kRoutingActivationSuccessful,
             std::move(std::make_unique<kRoutingActivationSuccessful>(routingActivationState::kRoutingActivationSuccessful)));

    // kRoutingActivationFailed
    GetRoutingActivationStateContext().AddState(routingActivationState::kRoutingActivationFailed,
             std::move(std::make_unique<kRoutingActivationFailed>(routingActivationState::kRoutingActivationFailed)));

    // transit to idle state
    GetRoutingActivationStateContext().TransitionTo(routingActivationState::kIdle);
}

auto TcpChannelStateImpl::GetRoutingActivationStateContext()
    noexcept -> StateContext<routingActivationState> & {
    return *routing_activation_state_context_.get();
}

kIdle::kIdle(routingActivationState state)
    : State<routingActivationState>(state) {}

void kIdle::Start() {}

void kIdle::Stop() {}

void kIdle::HandleMessage() {}

kWaitForRoutingActivationRes::kWaitForRoutingActivationRes(routingActivationState state)
    : State<routingActivationState>(state) {}

void kWaitForRoutingActivationRes::Start() {
    // wait for routing activation response till DoIPRoutingActivationTimeout
    timer_sync_.Start(kDoIPRoutingActivationTimeout);
}

void kWaitForRoutingActivationRes::Stop() { timer_sync_.Stop(); }

void kWaitForRoutingActivationRes::HandleMessage() {}

kRoutingActivationSuccessful::kRoutingActivationSuccessful(routingActivationState state)
    : State<routingActivationState>(state) {}

void kRoutingActivationSuccessful::Start() {}

void kRoutingActivationSuccessful::Stop() {}

void kRoutingActivationSuccessful::HandleMessage() {}

kRoutingActivationFailed::kRoutingActivationFailed(routingActivationState state)
    : State<routingActivationState>(state) {}

void kRoutingActivationFailed::Start() {}

void kRoutingActivationFailed::Stop() {}

void kRoutingActivationFailed::HandleMessage() {}

} // tcpChannelStateImpl
} // doip
} // diag
} // ara