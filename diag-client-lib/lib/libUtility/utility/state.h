/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_STATE_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_STATE_H

#include <string>
#include <utility>
#include <map>
#include <memory>
#include <cstdint>

namespace libUtility {
namespace state {

template <typename EnumState>
class State {
public:
  // ctor
  explicit State(EnumState state):
          state_{state} {}

  // dtor
  virtual ~State() = default;

  // start the state
  virtual void Start() = 0;

  // Update the state
  virtual void Stop() = 0;

  // Handle invoked asynchronously
  virtual void HandleMessage() = 0;

  // Get the State index
  auto GetState() noexcept -> EnumState {
      return state_;
  }
protected:
  // state index number
  EnumState state_;
};

template <typename EnumState>
class StateContext {
public:
  // ctor
  StateContext():
    current_state_{nullptr} ,
    state_map_{} {}

  // dtor
  ~StateContext() = default;

  // Add the needed state
  void AddState(EnumState state, std::unique_ptr<State<EnumState>> state_ptr) {
    state_map_.insert(
      std::pair<EnumState, std::unique_ptr<State<EnumState>>>(
        state,
        std::move(state_ptr)
      ));
  }

  // Get the current state
  auto GetActiveState() noexcept -> State<EnumState>& {
    return *current_state_;
  }

  // Function to transition state to provided state
  void TransitionTo(EnumState state) {
    // stop the current state
    Stop();
    // Update to new state
    Update(state);
    // Start new state
    Start();
  }

  // Get Context
  auto GetContext() noexcept-> StateContext* {
    return this;
  }
private:
  // Start the current state
  void Start() {
    if(this->current_state_ != nullptr) {
      this->current_state_->Start();
    }
  }

  // Stop the current state
  void Stop() {
    if(this->current_state_ != nullptr) {
      this->current_state_->Stop();
    }
  }

  // Update to new state
  void Update(EnumState state) {
    auto it = state_map_.find(state);
    if(it != state_map_.end()) {
      this->current_state_ = it->second.get();
    }
    else {
      // failure condition
    }
  }

  // pointer to store the active state
  State<EnumState>* current_state_;

  // mapping of state to state ref
  std::map<EnumState, std::unique_ptr<State<EnumState>>> state_map_;
};

} // state
} // libUtility

#endif // DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_STATE_H