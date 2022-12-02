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
    : routing_activation_state_context_{std::make_unique<StateContext<routingActivationState>>()},
      diagnostic_message_state_context_{std::make_unique<StateContext<diagnosticState>>()} {
    // create and add state for routing activation
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

    // create and add state for Diagnostic State
    // kDiagIdle
    GetDiagnosticMessageStateContext().AddState(diagnosticState::kDiagIdle,
                       std::move(std::make_unique<kDiagIdle>(diagnosticState::kDiagIdle)));

    // kWaitForDiagnosticAck
    GetDiagnosticMessageStateContext().AddState(diagnosticState::kWaitForDiagnosticAck,
                                                std::move(std::make_unique<kWaitForDiagnosticAck>(diagnosticState::kWaitForDiagnosticAck)));

    // kSendDiagnosticReqFailed
    GetDiagnosticMessageStateContext().AddState(diagnosticState::kSendDiagnosticReqFailed,
                                                std::move(std::make_unique<kSendDiagnosticReqFailed>(diagnosticState::kSendDiagnosticReqFailed)));

    // kDiagnosticPositiveAckRecvd
    GetDiagnosticMessageStateContext().AddState(diagnosticState::kDiagnosticPositiveAckRecvd,
                                                std::move(std::make_unique<kDiagnosticPositiveAckRecvd>(diagnosticState::kDiagnosticPositiveAckRecvd)));

    // kDiagnosticNegativeAckRecvd
    GetDiagnosticMessageStateContext().AddState(diagnosticState::kDiagnosticNegativeAckRecvd,
                                                std::move(std::make_unique<kDiagnosticNegativeAckRecvd>(diagnosticState::kDiagnosticNegativeAckRecvd)));

    // kWaitForDiagnosticResponse
    GetDiagnosticMessageStateContext().AddState(diagnosticState::kWaitForDiagnosticResponse,
                                                std::move(std::make_unique<kWaitForDiagnosticResponse>(diagnosticState::kWaitForDiagnosticResponse)));
    // transit to idle state
    GetDiagnosticMessageStateContext().TransitionTo(diagnosticState::kDiagIdle);
}

auto TcpChannelStateImpl::GetRoutingActivationStateContext()
    noexcept -> StateContext<routingActivationState> & {
    return *routing_activation_state_context_.get();
}

auto TcpChannelStateImpl::GetDiagnosticMessageStateContext()
    noexcept -> StateContext<diagnosticState> & {
    return *diagnostic_message_state_context_.get();
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

kDiagIdle::kDiagIdle(diagnosticState state)
    : State<diagnosticState>(state) {}

void kDiagIdle::Start() {}

void kDiagIdle::Stop() {}

void kDiagIdle::HandleMessage() {}

kWaitForDiagnosticAck::kWaitForDiagnosticAck(diagnosticState state)
    : State<diagnosticState>(state) {}

void kWaitForDiagnosticAck::Start() {
    // wait for diagnostic acknowledgement till kDoIPDiagnosticAckTimeout
    timer_sync_.Start(kDoIPDiagnosticAckTimeout);
}

void kWaitForDiagnosticAck::Stop() { timer_sync_.Stop(); }

void kWaitForDiagnosticAck::HandleMessage() {}

kSendDiagnosticReqFailed::kSendDiagnosticReqFailed(diagnosticState state)
    : State<diagnosticState>(state) {}

void kSendDiagnosticReqFailed::Start() {}

void kSendDiagnosticReqFailed::Stop() {}

void kSendDiagnosticReqFailed::HandleMessage() {}

kDiagnosticPositiveAckRecvd::kDiagnosticPositiveAckRecvd(diagnosticState state)
    : State<diagnosticState>(state) {}

void kDiagnosticPositiveAckRecvd::Start() {}

void kDiagnosticPositiveAckRecvd::Stop() {}

kDiagnosticNegativeAckRecvd::kDiagnosticNegativeAckRecvd(diagnosticState state)
    : State<diagnosticState>(state) {}

void kDiagnosticNegativeAckRecvd::Start() {}

void kDiagnosticNegativeAckRecvd::Stop() {}

void kDiagnosticNegativeAckRecvd::HandleMessage() {}

kWaitForDiagnosticResponse::kWaitForDiagnosticResponse(diagnosticState state)
    : State<diagnosticState>(state) {}

void kWaitForDiagnosticResponse::Start() {}

void kWaitForDiagnosticResponse::Stop() {}

void kWaitForDiagnosticResponse::HandleMessage() {}

} // tcpChannelStateImpl
} // doip
} // diag
} // ara