
#include <iostream>
#include <typeinfo>

/**
 * The base State class declares methods that all Concrete State should
 * implement and also provides a backreference to the Context object, associated
 * with the State. This backreference can be used by States to transition the
 * Context to another State.
 */
class StateContext;

class State {
    /**
     * @var Context
     */
protected:
    StateContext *context_;

public:
    virtual ~State() {
    }

    void set_context(Context *context) {
        this->context_ = context;
    }

    virtual void Init() = 0;
    virtual void Update() = 0;
};

/**
 * The Context defines the interface of interest to clients. It also maintains a
 * reference to an instance of a State subclass, which represents the current
 * state of the Context.
 */
class StateContext {
    /**
     * @var State A reference to the current state of the Context.
     */
private:
    State *state_;

public:
    StateContext(State *state) : state_(nullptr) {
        this->TransitionTo(state);
    }
    ~StateContext() {
        delete state_;
    }
    /**
     * The Context allows changing the State object at runtime.
     */
    void TransitionTo(State *state) {
        std::cout << "Context: Transition to " << typeid(*state).name() << ".\n";
        if (this->state_ != nullptr)
            delete this->state_;
        this->state_ = state;
        this->state_->set_context(this);
    }
    /**
     * The Context delegates part of its behavior to the current State object.
     */
    void Request1() {
        this->state_->Enter();
    }
    void Request2() {
        this->state_->Exit();
    }
};

/**
 * Concrete States implement various behaviors, associated with a state of the
 * Context.
 */
class ConcreteStateA : public State {
public:
    void Init() override;

    void Update() override {
        std::cout << "ConcreteStateA handles request2.\n";
    }
};

class ConcreteStateB : public State {
public:
    void Init() override {
        std::cout << "ConcreteStateB handles request1.\n";
    }
    void Update() override {
        std::cout << "ConcreteStateB handles request2.\n";
        std::cout << "ConcreteStateB wants to change the state of the context.\n";
        this->context_->TransitionTo(new ConcreteStateA);
    }
};

void ConcreteStateA::Init() {
    {
        std::cout << "ConcreteStateA handles request1.\n";
        std::cout << "ConcreteStateA wants to change the state of the context.\n";

        this->context_->TransitionTo(new ConcreteStateB);
    }
}

/**
 * The client code.
 */
void ClientCode() {
    StateContext *context = new StateContext(new ConcreteStateA);
    context->Request1();
    context->Request2();
    delete context;
}

int main() {
    ClientCode();
    return 0;
}