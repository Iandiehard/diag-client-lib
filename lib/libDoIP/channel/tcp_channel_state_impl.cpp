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
TcpChannelStateImpl::TcpChannelStateImpl(tcpChannel::tcpChannel& tcp_channel)
        : tcp_channel_{tcp_channel}{
    // create and add state
    // kIdle
    AddState(uint8_t(routingActivateState::kIdle),
                       std::move(std::make_unique<kIdle>(GetContext(),
                                                         uint8_t(routingActivateState::kIdle))));

    // kWaitForRoutingActivationRes
    AddState(uint8_t(routingActivateState::kWaitForRoutingActivationRes),
                       std::move(std::make_unique<kWaitForRoutingActivationRes>(GetContext(),
                                                                                uint8_t(routingActivateState::kWaitForRoutingActivationRes))));

    // kProcessRoutingActivationRes
    AddState(uint8_t(routingActivateState::kProcessRoutingActivationRes),
                       std::move(std::make_unique<kProcessRoutingActivationRes>(GetContext(),
                                                                                uint8_t(routingActivateState::kProcessRoutingActivationRes))));

    // kRoutingActivationResTimeout
    AddState(uint8_t(routingActivateState::kRoutingActivationResTimeout),
                       std::move(std::make_unique<kRoutingActivationResTimeout>(GetContext(),
                                                                                uint8_t(routingActivateState::kRoutingActivationResTimeout))));

    // kRoutingActivationFailed
    AddState(uint8_t(routingActivateState::kRoutingActivationFailed),
                       std::move(std::make_unique<kRoutingActivationFailed>(GetContext(),
                                                                            uint8_t(routingActivateState::kRoutingActivationFailed))));
}

kIdle::kIdle(StateContext *context, uint8_t state_indx)
    : State(context, state_indx) {
}

void kIdle::Start() {
    // do something
}

void kIdle::Stop() {
    //
}

void kIdle::HandleMessage() {
    // do nothing
}

kWaitForRoutingActivationRes::kWaitForRoutingActivationRes(StateContext *context, uint8_t state_indx)
    : State(context, state_indx) {
}

void kWaitForRoutingActivationRes::Start() {
    // wait for routing activation response till DoIPRoutingActivationTimeout
    timer_sync_.Start(kDoIPRoutingActivationTimeout);
}

void kWaitForRoutingActivationRes::Stop() {
    // Stop the timer here
    timer_sync_.Stop();
}

void kWaitForRoutingActivationRes::HandleMessage() {


}

kProcessRoutingActivationRes::kProcessRoutingActivationRes(StateContext *context, uint8_t state_indx)
    : State(context, state_indx) {
}

void kProcessRoutingActivationRes::Start() {

}

void kProcessRoutingActivationRes::Stop() {

}

void kProcessRoutingActivationRes::HandleMessage() {

}

kRoutingActivationResTimeout::kRoutingActivationResTimeout(StateContext *context, uint8_t state_indx) :
    State(context, state_indx) {
}

void kRoutingActivationResTimeout::Start() {

}

void kRoutingActivationResTimeout::Stop() {

}

void kRoutingActivationResTimeout::HandleMessage() {

}

kRoutingActivationFailed::kRoutingActivationFailed(StateContext *context, uint8_t state_indx) :
    State(context, state_indx) {
}

void kRoutingActivationFailed::Start() {

}

void kRoutingActivationFailed::Stop() {
    GetContext().TransitionTo(uint8_t(TcpChannelStateImpl::routingActivateState::kIdle));
}

void kRoutingActivationFailed::HandleMessage() {

}

} // tcpChannelStateImpl
} // doip
} // diag
} // ara