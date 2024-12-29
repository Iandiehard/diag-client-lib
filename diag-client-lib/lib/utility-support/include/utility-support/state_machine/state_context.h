/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_STATE_MACHINE_STATE_CONTEXT_H_
#define UTILITY_SUPPORT_STATE_MACHINE_STATE_CONTEXT_H_

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

#include "utility-support/state_machine/base_state/base_state.h"

namespace utility_support {
namespace state_machine {

template<typename StateHandle>
class StateContext {
 public:
  using State = base_state::BaseState<StateHandle>;

 private:
  using StateMap = std::map<StateHandle, std::unique_ptr<State>>;
  using StateIterator = typename StateMap::iterator;

 public:
  /**
  * @brief  Construct the instance of state context
  */
  StateContext() noexcept : state_map_{}, current_state_{} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  StateContext(const StateContext &other) noexcept = delete;
  StateContext &operator=(const StateContext &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  StateContext(StateContext &&other) noexcept = delete;
  StateContext &operator=(StateContext &&other) noexcept = delete;

  /**
   * @brief  Destruct the instance of state context
   */
  ~StateContext() noexcept = default;

  void AddState(std::unique_ptr<State> state) {
    StateHandle const state_handle{state->GetState()};
    state_map_.emplace(std::make_pair(state_handle, std::move(state)));
  }

  void TransitionTo(StateHandle const state_handle) {
    std::lock_guard<std::mutex> const lck{state_lock};
    StateIterator const iterator{state_map_.find(state_handle)};
    if (iterator != state_map_.end()) {
      // Exit the current state
      current_state_->OnExit();
      // Update to new state
      current_state_ = iterator->second.get();
      // Enter to new state
      current_state_->OnEnter();
    }
  }

  auto GetActiveState() noexcept -> StateHandle {
    std::lock_guard<std::mutex> const lck{state_lock};
    StateHandle const state_handle{current_state_->GetState()};
    return state_handle;
  }

 private:
  std::mutex state_lock;

  std::map<StateHandle, std::unique_ptr<State>> state_map_;

  std::optional<std::reference_wrapper<StateHandle>> current_state_;
};

}  // namespace state_machine
}  // namespace utility_support
#endif  // UTILITY_SUPPORT_STATE_MACHINE_STATE_CONTEXT_H_
