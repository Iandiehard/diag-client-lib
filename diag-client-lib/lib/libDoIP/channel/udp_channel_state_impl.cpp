/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/udp_channel_state_impl.h"

namespace ara{
namespace diag{
namespace doip {
namespace udpChannelStateImpl {

// ctor
UdpChannelStateImpl::UdpChannelStateImpl()
  : vehicle_discovery_state_context_{std::make_unique<StateContext<VehicleDiscoveryState>>()},
    vehicle_identification_state_context_{std::make_unique<StateContext<VehicleIdentificationState>>()} {
  // create and add state for vehicle discovery
  // kVdIdle
  GetVehicleDiscoveryStateContext().AddState(
    VehicleDiscoveryState::kVdIdle,
    std::move(std::make_unique<kVdIdle>(VehicleDiscoveryState::kVdIdle)));
  
  // kWaitForVehicleAnnouncement
  GetVehicleDiscoveryStateContext().AddState(
    VehicleDiscoveryState::kWaitForVehicleAnnouncement,
    std::move(std::make_unique<kWaitForVehicleAnnouncement>(VehicleDiscoveryState::kWaitForVehicleAnnouncement)));
  
  // kVdDoIPCtrlTimeout
  GetVehicleDiscoveryStateContext().AddState(
    VehicleDiscoveryState::kVdDoIPCtrlTimeout,
    std::move(std::make_unique<kVdDoIPCtrlTimeout>(VehicleDiscoveryState::kVdDoIPCtrlTimeout)));
  
  // Transit to idle
  GetVehicleDiscoveryStateContext().TransitionTo(VehicleDiscoveryState::kVdIdle);
  
  // create and add state for vehicle identification
  // kVdIdle
  GetVehicleIdentificationStateContext().AddState(
    VehicleIdentificationState::kViIdle,
    std::move(std::make_unique<kViIdle>(VehicleIdentificationState::kViIdle)));
  
  // kViSendVehicleIdentificationReq
  GetVehicleIdentificationStateContext().AddState(
    VehicleIdentificationState::kViSendVehicleIdentificationReq,
    std::move(std::make_unique<kViSendVehicleIdentificationReq>(VehicleIdentificationState::kViSendVehicleIdentificationReq)));
  
  // kViWaitForVehicleIdentificationRes
  GetVehicleIdentificationStateContext().AddState(
    VehicleIdentificationState::kViWaitForVehicleIdentificationRes,
    std::move(std::make_unique<kViWaitForVehicleIdentificationRes>(VehicleIdentificationState::kViWaitForVehicleIdentificationRes)));
  
  // kViDoIPCtrlTimeout
  GetVehicleIdentificationStateContext().AddState(
    VehicleIdentificationState::kViDoIPCtrlTimeout,
    std::move(std::make_unique<kViDoIPCtrlTimeout>(VehicleIdentificationState::kViDoIPCtrlTimeout)));
  
  // Transit to idle
  GetVehicleIdentificationStateContext().TransitionTo(VehicleIdentificationState::kViIdle);
}

auto UdpChannelStateImpl::GetVehicleDiscoveryStateContext()
  noexcept -> StateContext<VehicleDiscoveryState> & {
  return *vehicle_discovery_state_context_;
}

auto UdpChannelStateImpl::GetVehicleIdentificationStateContext()
  noexcept -> StateContext<VehicleIdentificationState> & {
  return *vehicle_identification_state_context_;
}


kVdIdle::kVdIdle(VehicleDiscoveryState state)
: State(state) {}

void kVdIdle::Start() {}

void kVdIdle::Stop() {}

void kVdIdle::HandleMessage() {}

kWaitForVehicleAnnouncement::kWaitForVehicleAnnouncement(VehicleDiscoveryState state)
: State(state) {}

void kWaitForVehicleAnnouncement::Start() {}

void kWaitForVehicleAnnouncement::Stop() {}

void kWaitForVehicleAnnouncement::HandleMessage() {}

kVdDoIPCtrlTimeout::kVdDoIPCtrlTimeout(VehicleDiscoveryState state)
: State(state) {}

void kVdDoIPCtrlTimeout::Start() {}

void kVdDoIPCtrlTimeout::Stop() {}

void kVdDoIPCtrlTimeout::HandleMessage() {}

kViIdle::kViIdle(VehicleIdentificationState state)
: State(state) {}

void kViIdle::Start() {}

void kViIdle::Stop() {}

void kViIdle::HandleMessage() {}

kViSendVehicleIdentificationReq::kViSendVehicleIdentificationReq(VehicleIdentificationState state)
: State(state) {}

void kViSendVehicleIdentificationReq::Start() {}

void kViSendVehicleIdentificationReq::Stop() {}

void kViSendVehicleIdentificationReq::HandleMessage() {}

kViWaitForVehicleIdentificationRes::kViWaitForVehicleIdentificationRes(VehicleIdentificationState state)
: State(state) {}

void kViWaitForVehicleIdentificationRes::Start() {}

void kViWaitForVehicleIdentificationRes::Stop() {}

void kViWaitForVehicleIdentificationRes::HandleMessage() {}

kViDoIPCtrlTimeout::kViDoIPCtrlTimeout(VehicleIdentificationState state)
: State(state) {}

void kViDoIPCtrlTimeout::Start() {}

void kViDoIPCtrlTimeout::Stop() {}

void kViDoIPCtrlTimeout::HandleMessage() {}

} // udpChannelStateImpl
} // doip
} // diag
} // ara