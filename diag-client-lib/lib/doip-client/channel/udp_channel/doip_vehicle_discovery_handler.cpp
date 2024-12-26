/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "channel/udp_channel/doip_vehicle_discovery_handler.h"

#include "common/logger.h"
#include "utility/state.h"

namespace doip_client {
namespace channel {
namespace udp_channel {
namespace {
/**
 * @brief  Different vehicle discovery state
 */
enum class VehicleDiscoveryState : std::uint8_t {
  kIdle = 0U,
  kWaitForVehicleAnnouncement,
  kDoIPCtrlTimeout
};

/**
 * @brief       Class implements idle state
 */
class kIdle final : public utility::state::State<VehicleDiscoveryState> {
 public:
  /**
   * @brief         Constructs an instance of kIdle
   * @param[in]     state
   *                The kIdle state
   */
  explicit kIdle(VehicleDiscoveryState state) : State<VehicleDiscoveryState>(state) {}

  /**
   * @brief         Function to start the current state
   */
  void Start() override {}

  /**
   * @brief         Function to stop the current state
   */
  void Stop() override {}
};

/**
 * @brief       Class implements idle state
 */
class kWaitForVehicleAnnouncement final : public utility::state::State<VehicleDiscoveryState> {
 public:
  /**
   * @brief         Constructs an instance of kWaitForVehicleAnnouncement
   * @param[in]     state
   *                The kWaitForVehicleAnnouncement state
   */
  explicit kWaitForVehicleAnnouncement(VehicleDiscoveryState state)
      : State<VehicleDiscoveryState>(state) {}

  /**
   * @brief         Function to start the current state
   */
  void Start() override {}

  /**
   * @brief         Function to stop the current state
   */
  void Stop() override {}
};

/**
 * @brief       Class implements idle state
 */
class kDoIPCtrlTimeout final : public utility::state::State<VehicleDiscoveryState> {
 public:
  /**
   * @brief         Constructs an instance of kDoIPCtrlTimeout
   * @param[in]     state
   *                The kDoIPCtrlTimeout state
   */
  explicit kDoIPCtrlTimeout(VehicleDiscoveryState state) : State<VehicleDiscoveryState>(state) {}

  /**
   * @brief         Function to start the current state
   */
  void Start() override {}

  /**
   * @brief         Function to stop the current state
   */
  void Stop() override {}
};

}  // namespace

/**
 * @brief       Class implements vehicle discovery handler
 */
class VehicleDiscoveryHandler::VehicleDiscoveryHandlerImpl final {
 public:
  /**
   * @brief  Type alias for state context
   */
  using VehicleDiscoveryStateContext = utility::state::StateContext<VehicleDiscoveryState>;

  /**
   * @brief         Constructs an instance of VehicleDiscoveryHandlerImpl
   * @param[in]     udp_socket_handler
   *                The reference to socket handler
   */
  explicit VehicleDiscoveryHandlerImpl(sockets::UdpSocketHandler &udp_socket_handler)
      : udp_socket_handler_{udp_socket_handler},
        state_context_{} {
    // create and add state for vehicle discovery
    // kIdle
    state_context_.AddState(VehicleDiscoveryState::kIdle,
                            std::make_unique<kIdle>(VehicleDiscoveryState::kIdle));
    // kWaitForVehicleAnnouncement
    state_context_.AddState(VehicleDiscoveryState::kWaitForVehicleAnnouncement,
                            std::make_unique<kWaitForVehicleAnnouncement>(
                                VehicleDiscoveryState::kWaitForVehicleAnnouncement));
    // kDoIPCtrlTimeout
    state_context_.AddState(
        VehicleDiscoveryState::kDoIPCtrlTimeout,
        std::make_unique<kDoIPCtrlTimeout>(VehicleDiscoveryState::kDoIPCtrlTimeout));
    // Transit to wait for vehicle announcement
    state_context_.TransitionTo(VehicleDiscoveryState::kWaitForVehicleAnnouncement);
  }

  /**
   * @brief       Function to get the Vehicle Discovery State context
   * @return      The reference to state context
   */
  auto GetStateContext() noexcept -> VehicleDiscoveryStateContext & { return state_context_; }

  /**
   * @brief       Function to get the socket handler
   * @return      The reference to socket handler
   */
  auto GetSocketHandler() noexcept -> sockets::UdpSocketHandler & { return udp_socket_handler_; }

 private:
  /**
   * @brief  The reference to socket handler
   */
  sockets::UdpSocketHandler &udp_socket_handler_;

  /**
   * @brief  Stores the vehicle discovery states
   */
  VehicleDiscoveryStateContext state_context_;
};

udp_channel::VehicleDiscoveryHandler::VehicleDiscoveryHandler(
    sockets::UdpSocketHandler &udp_socket_handler, DoipUdpChannel &)
    : handler_impl_{std::make_unique<VehicleDiscoveryHandlerImpl>(udp_socket_handler)} {}

VehicleDiscoveryHandler::~VehicleDiscoveryHandler() = default;

void VehicleDiscoveryHandler::ProcessVehicleAnnouncementResponse(DoipMessage &) noexcept {
  if (handler_impl_->GetStateContext().GetActiveState().GetState() ==
      VehicleDiscoveryState::kWaitForVehicleAnnouncement) {
    // Deserialize and Add to task executor
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
        FILE_NAME, __LINE__, __func__, [](std::stringstream &msg) {
          msg << "Processing of vehicle announcement is not implemented";
        });
  } else {
    // ignore
  }
}

}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client