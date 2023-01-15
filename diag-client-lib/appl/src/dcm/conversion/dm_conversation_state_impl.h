/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_STATE_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_STATE_IMPL_H
/* includes */
#include "common_Header.h"
#include "utility/state.h"

namespace diag {
namespace client {
namespace conversation_state_impl {
using namespace libUtility::state;
// Conversation States
enum class ConversationState : uint8_t {
  kIdle = 0x00,
  kDiagWaitForRes,
  kDiagStartP2StarTimer,
  kDiagRecvdPendingRes,
  kDiagRecvdFinalRes,
  kDiagSuccess,
};

class ConversationStateImpl {
public:
  // ctor
  ConversationStateImpl();
  
  // dtor
  ~ConversationStateImpl() = default;
  
  // Function to get the Conversation State context
  auto GetConversationStateContext() noexcept -> StateContext<ConversationState> &;

private:
  // conversation state
  std::unique_ptr<StateContext<ConversationState>> conversation_state_;
};

class kIdle final : public State<ConversationState> {
public:
  // ctor
  kIdle(ConversationState state);
  
  // dtor
  ~kIdle() = default;
  
  // start the state
  void Start() override;
  
  // Stop the state
  void Stop() override;
  
  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagWaitForRes final : public State<ConversationState> {
public:
  // ctor
  kDiagWaitForRes(ConversationState state);
  
  // dtor
  ~kDiagWaitForRes() = default;
  
  // start the state
  void Start() override;
  
  // Stop the state
  void Stop() override;
  
  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagStartP2StarTimer final : public State<ConversationState> {
public:
  // ctor
  kDiagStartP2StarTimer(ConversationState state);
  
  // dtor
  ~kDiagStartP2StarTimer() = default;
  
  // start the state
  void Start() override;
  
  // Stop the state
  void Stop() override;
  
  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagRecvdPendingRes final : public State<ConversationState> {
public:
  // ctor
  kDiagRecvdPendingRes(ConversationState state);
  
  // dtor
  ~kDiagRecvdPendingRes() = default;
  
  // start the state
  void Start() override;
  
  // Stop the state
  void Stop() override;
  
  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagRecvdFinalRes final : public State<ConversationState> {
public:
  // ctor
  kDiagRecvdFinalRes(ConversationState state);
  
  // dtor
  ~kDiagRecvdFinalRes() = default;
  
  // start the state
  void Start() override;
  
  // Stop the state
  void Stop() override;
  
  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagSuccess final : public State<ConversationState> {
public:
  // ctor
  kDiagSuccess(ConversationState state);
  
  // dtor
  ~kDiagSuccess() = default;
  
  // start the state
  void Start() override;
  
  // Stop the state
  void Stop() override;
  
  // Handle invoked asynchronously
  void HandleMessage() override;
};
}  // namespace conversation_state_impl
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_STATE_IMPL_H