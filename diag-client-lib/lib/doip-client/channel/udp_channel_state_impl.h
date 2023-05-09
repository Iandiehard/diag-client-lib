/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_STATE_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_STATE_IMPL_H

#include <cstdint>
#include <vector>

#include "common/common_doip_types.h"
#include "utility/state.h"

namespace doip_client {
namespace udpChannelStateImpl {

using namespace utility::state;

// Vehicle discovery state
enum class VehicleDiscoveryState : std::uint8_t { kVdIdle = 0U, kWaitForVehicleAnnouncement, kVdDoIPCtrlTimeout };
// Vehicle Identification state
enum class VehicleIdentificationState : std::uint8_t {
  kViIdle = 0U,
  kViSendVehicleIdentificationReq,
  kViWaitForVehicleIdentificationRes,
  kViDoIPCtrlTimeout
};

class UdpChannelStateImpl {
public:
  // ctor
  UdpChannelStateImpl();

  // dtor
  ~UdpChannelStateImpl() = default;

  // Function to get Vehicle Discovery State context
  auto GetVehicleDiscoveryStateContext() noexcept -> StateContext<VehicleDiscoveryState> &;

  // Function to get Vehicle Identification State context
  auto GetVehicleIdentificationStateContext() noexcept -> StateContext<VehicleIdentificationState> &;

private:
  // routing activation state
  std::unique_ptr<StateContext<VehicleDiscoveryState>> vehicle_discovery_state_context_;
  // diagnostic state
  std::unique_ptr<StateContext<VehicleIdentificationState>> vehicle_identification_state_context_;
};

class kVdIdle final : public State<VehicleDiscoveryState> {
public:
  // ctor
  explicit kVdIdle(VehicleDiscoveryState state);

  // dtor
  ~kVdIdle() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kWaitForVehicleAnnouncement final : public State<VehicleDiscoveryState> {
public:
  // ctor
  explicit kWaitForVehicleAnnouncement(VehicleDiscoveryState state);

  // dtor
  ~kWaitForVehicleAnnouncement() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kVdDoIPCtrlTimeout final : public State<VehicleDiscoveryState> {
public:
  // ctor
  explicit kVdDoIPCtrlTimeout(VehicleDiscoveryState state);

  // dtor
  ~kVdDoIPCtrlTimeout() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kViIdle final : public State<VehicleIdentificationState> {
public:
  // ctor
  explicit kViIdle(VehicleIdentificationState state);

  // dtor
  ~kViIdle() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kViSendVehicleIdentificationReq final : public State<VehicleIdentificationState> {
public:
  // ctor
  explicit kViSendVehicleIdentificationReq(VehicleIdentificationState state);

  // dtor
  ~kViSendVehicleIdentificationReq() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kViWaitForVehicleIdentificationRes final : public State<VehicleIdentificationState> {
public:
  // ctor
  explicit kViWaitForVehicleIdentificationRes(VehicleIdentificationState state);

  // dtor
  ~kViWaitForVehicleIdentificationRes() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kViDoIPCtrlTimeout final : public State<VehicleIdentificationState> {
public:
  // ctor
  explicit kViDoIPCtrlTimeout(VehicleIdentificationState state);

  // dtor
  ~kViDoIPCtrlTimeout() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};
}  // namespace udpChannelStateImpl
}  // namespace doip_client

#endif  //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_STATE_IMPL_H
