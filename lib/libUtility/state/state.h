/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBOSABSTRACTION_LIBUTILITY_STATE_STATE_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBOSABSTRACTION_LIBUTILITY_STATE_STATE_H

#include <string>
#include <utility>
#include <map>
#include <memory>
#include <cstdint>
#include <iostream>
#include <typeinfo>

namespace libOsAbstraction {
namespace libUtility {
namespace state {

// forward declaration
class StateContext;

/**
* The base State class declares methods that all Concrete State should
* implement and also provides a backreference to the Context object, associated
* with the State. This backreference can be used by States to transition the
* Context to another State.
*/
class State {
public:
    // ctor
    explicit State(std::string name, StateContext *context);

    // dtor
    virtual ~State() = default;

    // get the name of state
    std::string Name();

    // Initialize the state
    virtual void Start() = 0;

    // Update the state
    virtual void Update() = 0;

protected:
    StateContext *context_;

private:
    std::string name_;
};

/**
* The Context defines the interface of interest to clients. It also maintains a
* reference to an instance of a State subclass, which represents the current
* state of the Context.
*/
class StateContext {
public:
    // ctor
    StateContext();

    // dtor
    ~StateContext() = default;

    // Function to transition state to provided state
    void TransitionTo(std::uint8_t state_index);

    // Initialize all state
    void Init(std::uint8_t state_indx, std::unique_ptr<State> state);

    void Start();

    // Update the current state
    void Update();
private:
    // pointer to store the active state
    State* current_state_;

    // mapping of state num to state ref
    std::map<std::uint8_t, std::unique_ptr<State>> state_map_;
};

/**
* Concrete States implement various behaviors, associated with a state of the
* Context.
*/
/*
class ConcreteStateA : public State {
public:
    void Init() override;

    void Update() override {
        std::cout << "ConcreteStateA handles request2.\n";
    }
};

class ConcreteStateB : public State {
public:
    void Start() override {
        std::cout << "ConcreteStateB handles request1.\n";
    }

    void Update() override {
        std::cout << "ConcreteStateB handles request2.\n";
        std::cout << "ConcreteStateB wants to change the state of the context.\n";
        context_->TransitionTo(new ConcreteStateA);
    }
};

void ConcreteStateA::Start() {
    {
        std::cout << "ConcreteStateA handles request1.\n";
        std::cout << "ConcreteStateA wants to change the state of the context.\n";

        context_->TransitionTo(new ConcreteStateB);
    }
}

void ClientCode() {
    StateContext *context = new StateContext(new ConcreteStateA);
    context->Start();
    context->Update();
    delete context;
}

int main() {
    ClientCode();
    return 0;
}
 */

} // state
} // libUtility
} // libOsAbstraction

#endif // DIAGNOSTIC_CLIENT_LIB_LIB_LIBOSABSTRACTION_LIBUTILITY_STATE_STATE_H