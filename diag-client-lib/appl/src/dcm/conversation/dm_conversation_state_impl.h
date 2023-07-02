/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_STATE_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_STATE_IMPL_H
/* includes */
#include "common_header.h"
#include "utility/state.h"

namespace diag {
namespace client {
namespace conversation_state_impl {
using namespace utility::state;
// Conversation States
enum class ConversationState : std::uint8_t {
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
  explicit kIdle(ConversationState state);

  // dtor
  ~kIdle() override = default;

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
  explicit kDiagWaitForRes(ConversationState state);

  // dtor
  ~kDiagWaitForRes() override = default;

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
  explicit kDiagStartP2StarTimer(ConversationState state);

  // dtor
  ~kDiagStartP2StarTimer() override = default;

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
  explicit kDiagRecvdPendingRes(ConversationState state);

  // dtor
  ~kDiagRecvdPendingRes() override = default;

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
  explicit kDiagRecvdFinalRes(ConversationState state);

  // dtor
  ~kDiagRecvdFinalRes() override = default;

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
  explicit kDiagSuccess(ConversationState state);

  // dtor
  ~kDiagSuccess() override = default;

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