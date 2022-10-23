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
        : context_(std::make_unique<StateContext>()) {
    // create and add state
    // kIdle
    GetContex().AddState(uint8_t(routingActivateState::kIdle),
                       std::move(std::make_unique<kIdle>(context_.get())));

    // kSendRoutingActivationReq
    GetContex().AddState(uint8_t(routingActivateState::kSendRoutingActivationReq),
                std::move(std::make_unique<kSendRoutingActivationReq>(context_.get())));

    // kWaitForRoutingActivationRes
    GetContex().AddState(uint8_t(routingActivateState::kWaitForRoutingActivationRes),
                       std::move(std::make_unique<kWaitForRoutingActivationRes>(context_.get())));

    // kProcessRoutingActivationRes
    GetContex().AddState(uint8_t(routingActivateState::kProcessRoutingActivationRes),
                       std::move(std::make_unique<kProcessRoutingActivationRes>(context_.get())));

    // kRoutingActivationResTimeout
    GetContex().AddState(uint8_t(routingActivateState::kRoutingActivationResTimeout),
                       std::move(std::make_unique<kRoutingActivationResTimeout>(context_.get())));

    // kRoutingActivationFailed
    GetContex().AddState(uint8_t(routingActivateState::kRoutingActivationFailed),
                       std::move(std::make_unique<kRoutingActivationFailed>(context_.get())));
}

auto TcpChannelStateImpl::GetContex() noexcept -> StateContext & {
    return (*context_.get());
}

kIdle::kIdle(StateContext *context)
    : State(context) {
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

kSendRoutingActivationReq::kSendRoutingActivationReq(StateContext *context)
    : State(context){
}

void kSendRoutingActivationReq::Start() {
}

void kSendRoutingActivationReq::Stop() {
}

void kSendRoutingActivationReq::HandleMessage() {
}

kWaitForRoutingActivationRes::kWaitForRoutingActivationRes(StateContext *context)
    : State(context) {
}

void kWaitForRoutingActivationRes::Start() {
}

void kWaitForRoutingActivationRes::Stop() {
}

void kWaitForRoutingActivationRes::HandleMessage() {
}

kProcessRoutingActivationRes::kProcessRoutingActivationRes(StateContext *context)
    : State(context) {
}

void kProcessRoutingActivationRes::Start() {

}

void kProcessRoutingActivationRes::Stop() {

}

void kProcessRoutingActivationRes::HandleMessage() {

}

kRoutingActivationResTimeout::kRoutingActivationResTimeout(StateContext *context) :
    State(context) {
}

void kRoutingActivationResTimeout::Start() {

}

void kRoutingActivationResTimeout::Stop() {

}

void kRoutingActivationResTimeout::HandleMessage() {

}

kRoutingActivationFailed::kRoutingActivationFailed(StateContext *context) :
    State(context) {
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