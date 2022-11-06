/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/tcp_channel_handler_impl.h"
#include "channel/tcp_channel.h"

namespace ara{
namespace diag{
namespace doip{
namespace tcpChannelHandlerImpl{


auto RoutingActivationHandler::ProcessDoIPRoutingActivationResponse(
            tcpChannel::tcpChannel &channel,
            doipPayloadType &doip_payload) noexcept -> RoutingActivationChannelState {
    RoutingActivationChannelState tcp_channel_final_state{RoutingActivationChannelState::kRoutingActivationFailed};

    if(channel
        .GetChannelState()
        .GetRoutingActivationStateContext()
        .GetActiveState()
        .GetState()
           == RoutingActivationChannelState::kWaitForRoutingActivationRes) {

        // get the logical address of client
        uint16_t client_address = (uint16_t)((payload[BYTE_POS_ZERO] << 8) & 0xFF00) |
                                  (uint16_t)(payload[BYTE_POS_ONE] & 0x00FF);
        // get the logical address of Server
        uint16_t server_address = (uint16_t)((payload[BYTE_POS_TWO] << 8) & 0xFF00) |
                                  (uint16_t)(payload[BYTE_POS_THREE] & 0x00FF);
        // get the ack code
        uint8_t act_type = payload[BYTE_POS_FOUR];
        switch(act_type) {
            case kDoip_RoutingActivation_ResCode_RoutingSuccessful: {
                // routing successful
                tcp_channel_final_state = RoutingActivationChannelState::kRoutingActivationSuccessful;
            }
                break;
            case kDoip_RoutingActivation_ResCode_ConfirmtnRequired: {
                // trigger routing activation after sometime, not implemented yet
            }
                break;
            default:
                break;
        }
    }
    else {
        /* ignore */
    }
    return tcp_channel_final_state;
}


} // tcpChannelHandlerImpl
} // doip
} // diag
} // ara