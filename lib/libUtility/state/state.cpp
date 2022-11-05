/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "state.h"

namespace libUtility {
namespace state {

// State ctor
State::State(StateContext *context, uint8_t state_indx):
        context_{context} ,
        state_indx_{state_indx} {
}

// State context ctor
StateContext::StateContext() :
        current_state_{nullptr} ,
        state_map_() {
}

// Function to transition state to supplied state
void StateContext::TransitionTo(std::uint8_t state_index) {
    // stop the current state
    Stop();

    // Update to new state
    Update(state_index);

    // Start new state
    Start();
}

// Initialize all state
void StateContext::AddState(std::uint8_t state_indx, std::unique_ptr<State> state) {
    state_map_.insert(
            std::pair<std::uint8_t, std::unique_ptr<State>>(
                    state_indx,
                    std::move(state)
            ));
}

// Start the current state
void StateContext::Start() {
    this->current_state_->Start();
}

// Stop the current state
void StateContext::Stop() {
    this->current_state_->Stop();
}

// Update to new state
void StateContext::Update(std::uint8_t state_index) {
    auto it = state_map_.find(state_index);
    if(it != state_map_.end()) {
        this->current_state_ = it->second.get();
    }
    else {
        // failure condition
    }
}

//
auto StateContext::GetActiveState() noexcept -> State& {
    return *current_state_;
}



} // state
} // libUtility