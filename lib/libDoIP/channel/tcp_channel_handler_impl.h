/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H

//includes
#include "common_Header.h"
#include "common/common_doip_types.h"
#include "channel/tcp_channel_state_impl.h"
#include "common/doip_payload_type.h"

namespace ara{
namespace diag{
namespace doip{

// forward declaration
namespace tcpChannel{
class tcpChannel;
}

namespace tcpChannelHandlerImpl {

using RoutingActivationChannelState = tcpChannelStateImpl::routingActivationState;
using DiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;

/*
 @ Class Name        : RoutingActivationHandler
 @ Class Description : Class used as a handler to process routing activation messages
 */
class RoutingActivationHandler {
public:
    // ctor
    RoutingActivationHandler() = default;

    // dtor
    ~RoutingActivationHandler() = default;

    // Function to process Routing activation response
    auto ProcessDoIPRoutingActivationResponse(
                tcpChannel::tcpChannel &channel, DoipMessage &doip_payload) noexcept -> RoutingActivationChannelState;

    // Function to send Routing activation request
};

/*
 @ Class Name        : DiagnosticMessageHandler
 @ Class Description : Class used as a handler to process diagnostic messages
 */
class DiagnosticMessageHandler {
public:
    // ctor
    DiagnosticMessageHandler() = default;

    // dtor
    ~DiagnosticMessageHandler() = default;

    // Function to process Routing activation response
    auto ProcessDoIPDiagnosticMessageResponse(
            tcpChannel::tcpChannel &channel, DoipMessage &doip_payload) noexcept -> DiagnosticMessageChannelState;

    // Function to send Diagnostic request
};



} // tcpChannelHandlerImpl
} // doip
} // diag
} // ara

#endif //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
