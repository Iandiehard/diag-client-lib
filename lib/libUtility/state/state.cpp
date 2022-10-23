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
State::State(StateContext *context):
        context_{context} {
}

// State context ctor
StateContext::StateContext() :
        current_state_{nullptr} ,
        state_map_() {
}

// Function to transition state to supplied state
void StateContext::TransitionTo(std::uint8_t state_index) {
    // std::cout << "Context: Transition to " << typeid(*state).name() << ".\n";
    auto it = state_map_.find(state_index);
    if(it != state_map_.end()) {
        this->current_state_ = it->second.get();
    }
    else {
        // failure condition
    }
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
// Update the current state
void StateContext::Update() {
    this->current_state_->Update();
}

} // state
} // libUtility