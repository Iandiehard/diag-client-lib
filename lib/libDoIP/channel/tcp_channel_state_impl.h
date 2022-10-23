/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H

#include <cstdint>
#include <vector>
#include "state/state.h"
#include "common/common_doip_types.h"

namespace ara{
namespace diag{
namespace doip {
namespace tcpChannelStateImpl {

using namespace libUtility::state;
using namespace ara::diag::uds_transport;

class kIdle final: public State {
public:
    // ctor
    kIdle(StateContext *context);

    // dtor
    ~kIdle() = default;

    // start the state
    void Start() override;

    // Stop the state
    void Stop() override;

    // Handle invoked asynchronously
    void HandleMessage() override;
};

/*
 * Transition table:-
 *  kSendRoutingActivationReq -> kWaitForRoutingActivationRes
 *  kSendRoutingActivationReq -> kRoutingActivationFailed
 */
class kSendRoutingActivationReq final: public State {
public:
    // ctor
    kSendRoutingActivationReq(StateContext *context);

    // dtor
    ~kSendRoutingActivationReq() = default;

    // start the state
    void Start() override;

    // Stop the state
    void Stop() override;

    // Handle invoked asynchronously
    void HandleMessage() override;
};

/*
 * Transition table:-
 *  kWaitForRoutingActivationRes -> kProcessRoutingActivationRes
 *  kWaitForRoutingActivationRes -> kRoutingActivationResTimeout
 */
class kWaitForRoutingActivationRes final: public State {
public:
    // ctor
    kWaitForRoutingActivationRes(StateContext *context);

    // dtor
    ~kWaitForRoutingActivationRes() = default;

    // start the state
    void Start() override;

    // Stop the state
    void Stop() override;

    // Handle invoked asynchronously
    void HandleMessage() override;
};

class kProcessRoutingActivationRes : public State {
public:
    // ctor
    kProcessRoutingActivationRes(StateContext *context);

    // dtor
    ~kProcessRoutingActivationRes() = default;

    // start the state
    void Start() override;

    // Stop the state
    void Stop() override;

    // Handle invoked asynchronously
    void HandleMessage() override;
};

class kRoutingActivationResTimeout : public State {
public:
    // ctor
    kRoutingActivationResTimeout(StateContext *context);

    // dtor
    ~kRoutingActivationResTimeout() = default;

    // start the state
    void Start() override;

    // Stop the state
    void Stop() override;

    // Handle invoked asynchronously
    void HandleMessage() override;
};

class kRoutingActivationFailed : public State {
public:
    // ctor
    kRoutingActivationFailed(StateContext *context);

    // dtor
    ~kRoutingActivationFailed() = default;

    // start the state
    void Start() override;

    // Stop the state
    void Stop() override;

    // Handle invoked asynchronously
    void HandleMessage() override;
};

class TcpChannelStateImpl {
public:
    // routing activation state
    enum class routingActivateState : std::uint8_t {
        kIdle   = 0U,
        kSendRoutingActivationReq,
        kWaitForRoutingActivationRes,
        kProcessRoutingActivationRes,
        kRoutingActivationResTimeout,
        kRoutingActivationSuccessful,
        kRoutingActivationFailed
    };

    // ctor
    TcpChannelStateImpl();

    // dtor
    ~TcpChannelStateImpl() = default;

private:
    // Get the context
    auto GetContex() noexcept -> StateContext&;

    // store context
    std::unique_ptr<StateContext> context_;
};

} // tcpChannelStateImpl
} // doip
} // diag
} // ara

#endif //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H
