/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "src/dcm/conversion/dm_conversation_state_impl.h"

namespace diag {
namespace client {
namespace conversation_state_impl {
ConversationStateImpl::ConversationStateImpl()
    : conversation_state_{std::make_unique<StateContext<ConversationState>>()} {
  // create and add state
  // kIdle
  GetConversationStateContext().AddState(ConversationState::kIdle,
                                         std::move(std::make_unique<kIdle>(ConversationState::kIdle)));
  // kDiagWaitForRes
  GetConversationStateContext().AddState(
      ConversationState::kDiagWaitForRes,
      std::move(std::make_unique<kDiagWaitForRes>(ConversationState::kDiagWaitForRes)));
  // kDiagStartP2StarTimer
  GetConversationStateContext().AddState(
      ConversationState::kDiagStartP2StarTimer,
      std::move(std::make_unique<kDiagStartP2StarTimer>(ConversationState::kDiagStartP2StarTimer)));
  // kDiagRecvdPendingRes
  GetConversationStateContext().AddState(
      ConversationState::kDiagRecvdPendingRes,
      std::move(std::make_unique<kDiagRecvdPendingRes>(ConversationState::kDiagRecvdPendingRes)));
  // kDiagRecvdFinalRes
  GetConversationStateContext().AddState(
      ConversationState::kDiagRecvdFinalRes,
      std::move(std::make_unique<kDiagRecvdFinalRes>(ConversationState::kDiagRecvdFinalRes)));
  // kDiagSuccess
  GetConversationStateContext().AddState(ConversationState::kDiagSuccess,
                                         std::move(std::make_unique<kDiagSuccess>(ConversationState::kDiagSuccess)));
  // transit to idle state
  GetConversationStateContext().TransitionTo(ConversationState::kIdle);
}

auto ConversationStateImpl::GetConversationStateContext() noexcept -> StateContext<ConversationState>& {
  return *conversation_state_.get();
}

kIdle::kIdle(ConversationState state) : State<ConversationState>(state) {}

void kIdle::Start() {}

void kIdle::Stop() {}

void kIdle::HandleMessage() {}

kDiagWaitForRes::kDiagWaitForRes(ConversationState state) : State<ConversationState>(state) {}

void kDiagWaitForRes::Start() {}

void kDiagWaitForRes::Stop() {}

void kDiagWaitForRes::HandleMessage() {}

kDiagStartP2StarTimer::kDiagStartP2StarTimer(ConversationState state) : State<ConversationState>(state) {}

void kDiagStartP2StarTimer::Start() {}

void kDiagStartP2StarTimer::Stop() {}

void kDiagStartP2StarTimer::HandleMessage() {}

kDiagRecvdPendingRes::kDiagRecvdPendingRes(ConversationState state) : State<ConversationState>(state) {}

void kDiagRecvdPendingRes::Start() {}

void kDiagRecvdPendingRes::Stop() {}

void kDiagRecvdPendingRes::HandleMessage() {}

kDiagRecvdFinalRes::kDiagRecvdFinalRes(ConversationState state) : State<ConversationState>(state) {}

void kDiagRecvdFinalRes::Start() {}

void kDiagRecvdFinalRes::Stop() {}

void kDiagRecvdFinalRes::HandleMessage() {}

kDiagSuccess::kDiagSuccess(ConversationState state) : State<ConversationState>(state) {}

void kDiagSuccess::Start() {}

void kDiagSuccess::Stop() {}

void kDiagSuccess::HandleMessage() {}
}  // namespace conversation_state_impl
}  // namespace client
}  // namespace diag