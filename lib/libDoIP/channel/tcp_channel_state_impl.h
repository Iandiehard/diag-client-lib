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
#include "../../libUtility/state/state.h"

namespace ara{
namespace diag{
namespace doip {
namespace tcpChannelStateImpl {

/*
 * Transition table:-
 *  kSendRoutingActivationReq -> kWaitForRoutingActivationRes
 *  kSendRoutingActivationReq -> kRoutingActivationFailed
 */
class kSendRoutingActivationReq : public State {
public:
    void Init() override;

    void Update() override;
};

/*
 * Transition table:-
 *  kWaitForRoutingActivationRes -> kWaitForRoutingActivationRes
 *  kWaitForRoutingActivationRes -> kRoutingActivationFailed
 */
class kWaitForRoutingActivationRes : public State {
public:
    void Init() override;

    void Update() override;
};

class kProcessRoutingActivationRes : public State {
public:
    void Init() override;

    void Update() override;
};

class kRoutingActivationResTimeout : public State {
public:
    void Init() override;

    void Update() override;
};

class kRoutingActivationFailed : public State {
public:
    void Init() override;

    void Update() override;
};

class TcpChannelRoutingActivationStateImpl {
public:
    // routing activation state
    enum class routingActivateState : std::uint8_t {
        kIdle   = 0,
        kSendRoutingActivationReq,
        kWaitForRoutingActivationRes,
        kProcessRoutingActivationRes,
        kRoutingActivationSuccessful,
        kRoutingActivationResTimeout,
        kRoutingActivationFailed
    };

    auto GetActiveState() noexcept -> routingActivateState;

    auto GetActiveStateRef() noexcept -> State&;
private:


};


} // tcpChannelStateImpl
} // doip
} // diag
} // ara

#endif //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H
