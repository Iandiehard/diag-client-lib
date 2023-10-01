/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "channel/udp_channel/doip_vehicle_identification_handler.h"

#include "channel/udp_channel/doip_udp_channel.h"
#include "common/common_doip_types.h"
#include "common/logger.h"
#include "utility/state.h"
#include "utility/sync_timer.h"

namespace doip_client {
namespace channel {
namespace udp_channel {
namespace {

/**
* @brief       Type alias of vehicle payload type
*/
using VehiclePayloadType = std::pair<std::uint16_t, std::uint8_t>;

/**
* @brief  Different vehicle identification state
*/
enum class VehicleIdentificationState : std::uint8_t {
  kIdle = 0U,
  kSendVehicleIdentificationReq,
  kWaitForVehicleIdentificationRes,
  kDoIPCtrlTimeout
};

/**
* @brief       Class implements idle state
*/
class kIdle final : public utility::state::State<VehicleIdentificationState> {
 public:
  /**
  * @brief         Constructs an instance of kIdle
  * @param[in]     state
  *                The kIdle state
  */
  explicit kIdle(VehicleIdentificationState state) : State<VehicleIdentificationState>(state) {}

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
* @brief       Class implements sending of vehicle identification request state
*/
class kSendVehicleIdentificationReq final : public utility::state::State<VehicleIdentificationState> {
 public:
  /**
  * @brief         Constructs an instance of kSendVehicleIdentificationReq
  * @param[in]     state
  *                The kSendVehicleIdentificationReq state
  */
  explicit kSendVehicleIdentificationReq(VehicleIdentificationState state) : State<VehicleIdentificationState>(state) {}

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
* @brief       Class implements wait for vehicle identification response state
*/
class kWaitForVehicleIdentificationRes final : public utility::state::State<VehicleIdentificationState> {
 public:
  /**
  * @brief         Constructs an instance of kWaitForVehicleIdentificationRes
  * @param[in]     state
  *                The kWaitForVehicleIdentificationRes state
  */
  explicit kWaitForVehicleIdentificationRes(VehicleIdentificationState state)
      : State<VehicleIdentificationState>(state) {}

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
* @brief       Class implements Ctrl timeout state
*/
class kDoIPCtrlTimeout final : public utility::state::State<VehicleIdentificationState> {
 public:
  /**
  * @brief         Constructs an instance of kDoIPCtrlTimeout
  * @param[in]     state
  *                The kDoIPCtrlTimeout state
  */
  explicit kDoIPCtrlTimeout(VehicleIdentificationState state) : State<VehicleIdentificationState>(state) {}

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
 * @brief         Create the doip generic header
 */
void CreateDoipGenericHeader(std::vector<std::uint8_t> &doip_header_buffer, std::uint16_t payload_type,
                             std::uint32_t payload_len) {
  doip_header_buffer.emplace_back(kDoip_ProtocolVersion);
  doip_header_buffer.emplace_back(~(static_cast<std::uint8_t>(kDoip_ProtocolVersion)));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_type & 0xFF00) >> 8));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>(payload_type & 0x00FF));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0xFF000000) >> 24));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x00FF0000) >> 16));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x0000FF00) >> 8));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>(payload_len & 0x000000FF));
}

/**
 * @brief         Get the vehicle identification payload type based on preselection mode
 * @param[in]     preselection_mode
 *                The preselection mode
 * @return        The payload type with request type and request length
 */
auto GetVehicleIdentificationPayloadType(std::uint8_t preselection_mode) noexcept -> VehiclePayloadType {
  std::uint16_t vehicle_identification_type{0u};
  std::uint8_t vehicle_identification_length{0u};
  switch (preselection_mode) {
    case 0U:
      vehicle_identification_type = kDoip_VehicleIdentification_ReqType;
      vehicle_identification_length = kDoip_VehicleIdentification_ReqLen;
      break;
    case 1U:
      vehicle_identification_type = kDoip_VehicleIdentificationVIN_ReqType;
      vehicle_identification_length = kDoip_VehicleIdentificationVIN_ReqLen;
      break;
    case 2U:
      vehicle_identification_type = kDoip_VehicleIdentificationEID_ReqType;
      vehicle_identification_length = kDoip_VehicleIdentificationEID_ReqLen;
      break;
    default:
      break;
  }
  return VehiclePayloadType{vehicle_identification_type, vehicle_identification_length};
}

}  // namespace

/**
* @brief       Class implements vehicle identification handler
*/
class VehicleIdentificationHandler::VehicleIdentificationHandlerImpl final {
 public:
  /**
   * @brief  Type alias for state context
   */
  using VehicleIdentificationStateContext = utility::state::StateContext<VehicleIdentificationState>;

  /**
   * @brief  Type alias for Sync timer
   */
  using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;

  /**
   * @brief         Constructs an instance of VehicleDiscoveryHandlerImpl
   * @param[in]     udp_socket_handler
   *                The reference to socket handler
   */
  explicit VehicleIdentificationHandlerImpl(sockets::UdpSocketHandler &udp_socket_handler, DoipUdpChannel &channel)
      : udp_socket_handler_{udp_socket_handler},
        channel_{channel},
        state_context_{} {
    // create and add state for vehicle identification
    // kIdle
    state_context_.AddState(VehicleIdentificationState::kIdle,
                            std::make_unique<kIdle>(VehicleIdentificationState::kIdle));
    // kSendVehicleIdentificationReq
    state_context_.AddState(
        VehicleIdentificationState::kSendVehicleIdentificationReq,
        std::make_unique<kSendVehicleIdentificationReq>(VehicleIdentificationState::kSendVehicleIdentificationReq));
    // kWaitForVehicleIdentificationRes
    state_context_.AddState(VehicleIdentificationState::kWaitForVehicleIdentificationRes,
                            std::make_unique<kWaitForVehicleIdentificationRes>(
                                VehicleIdentificationState::kWaitForVehicleIdentificationRes));
    // kDoIPCtrlTimeout
    state_context_.AddState(VehicleIdentificationState::kDoIPCtrlTimeout,
                            std::make_unique<kDoIPCtrlTimeout>(VehicleIdentificationState::kDoIPCtrlTimeout));
    // Transit to idle state
    state_context_.TransitionTo(VehicleIdentificationState::kIdle);
  }

  /**
   * @brief       Function to get the Vehicle Identification State context
   * @return      The reference to state context
   */
  auto GetStateContext() noexcept -> VehicleIdentificationStateContext & { return state_context_; }

  /**
   * @brief       Function to get the socket handler
   * @return      The reference to socket handler
   */
  auto GetSocketHandler() noexcept -> sockets::UdpSocketHandler & { return udp_socket_handler_; }

  /**
   * @brief       Function to get the doip channel
   * @return      The reference to channel
   */
  auto GetDoipChannel() noexcept -> DoipUdpChannel & { return channel_; }

  /**
   * @brief       Function to get the sync timer
   * @return      The reference to sync timer
   */
  auto GetSyncTimer() noexcept -> SyncTimer & { return sync_timer_; }

 private:
  /**
   * @brief  The reference to socket handler
   */
  sockets::UdpSocketHandler &udp_socket_handler_;

  /**
   * @brief  The reference to doip channel
   */
  DoipUdpChannel &channel_;

  /**
   * @brief  Stores the vehicle identification states
   */
  VehicleIdentificationStateContext state_context_;

  /**
   * @brief  Store the synchronous timer
   */
  SyncTimer sync_timer_;
};

VehicleIdentificationHandler::VehicleIdentificationHandler(sockets::UdpSocketHandler &udp_socket_handler,
                                                           DoipUdpChannel &channel)
    : handler_impl_{std::make_unique<VehicleIdentificationHandlerImpl>(udp_socket_handler, channel)} {}

VehicleIdentificationHandler::~VehicleIdentificationHandler() = default;

auto VehicleIdentificationHandler::HandleVehicleIdentificationRequest(
    uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  if (handler_impl_->GetStateContext().GetActiveState().GetState() == VehicleIdentificationState::kIdle) {
    if (SendVehicleIdentificationRequest(std::move(vehicle_identification_request)) ==
        uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;

      handler_impl_->GetStateContext().TransitionTo(VehicleIdentificationState::kWaitForVehicleIdentificationRes);
      // Wait for 2 sec to collect all the vehicle identification response
      handler_impl_->GetSyncTimer().WaitForTimeout(
          [&]() { handler_impl_->GetStateContext().TransitionTo(VehicleIdentificationState::kDoIPCtrlTimeout); },
          [&]() {
            // do nothing
          },
          std::chrono::milliseconds{kDoIPCtrl});
      handler_impl_->GetStateContext().TransitionTo(VehicleIdentificationState::kIdle);
    } else {
      // failed, do nothing
    }
  } else {
    // not free
  }
  return ret_val;
}

void VehicleIdentificationHandler::ProcessVehicleIdentificationResponse(DoipMessage &doip_payload) noexcept {
  if (handler_impl_->GetStateContext().GetActiveState().GetState() ==
      VehicleIdentificationState::kWaitForVehicleIdentificationRes) {
    // Deserialize data to indicate to upper layer
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
        handler_impl_->GetDoipChannel().IndicateMessage(
            static_cast<uds_transport::UdsMessage::Address>(0U), static_cast<uds_transport::UdsMessage::Address>(0U),
            uds_transport::UdsMessage::TargetAddressType::kPhysical, 0U, doip_payload.GetPayload().size(), 0U,
            "DoIPUdp", doip_payload.GetPayload())};
    if ((ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
        (ret_val.second != nullptr)) {
      // Add meta info about ip address
      uds_transport::UdsMessage::MetaInfoMap meta_info_map{
          {"kRemoteIpAddress", std::string{doip_payload.GetHostIpAddress()}}};
      ret_val.second->AddMetaInfo(std::make_shared<uds_transport::UdsMessage::MetaInfoMap>(meta_info_map));
      // copy to application buffer
      (void) std::copy(doip_payload.GetPayload().begin(), doip_payload.GetPayload().end(),
                       ret_val.second->GetPayload().begin());
      handler_impl_->GetDoipChannel().HandleMessage(std::move(ret_val.second));
    }
  } else {
    // ignore
  }
}

auto VehicleIdentificationHandler::SendVehicleIdentificationRequest(
    uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  UdpMessagePtr doip_vehicle_identification_req{std::make_unique<UdpMessage>(
      vehicle_identification_request->GetHostIpAddress(), vehicle_identification_request->GetHostPortNumber())};

  // Get preselection mode
  std::uint8_t preselection_mode{vehicle_identification_request->GetPayload()[BYTE_POS_ONE]};
  // Get the payload type & length from preselection mode
  VehiclePayloadType const doip_vehicle_payload_type{GetVehicleIdentificationPayloadType(preselection_mode)};

  // create header
  CreateDoipGenericHeader(doip_vehicle_identification_req->GetTxBuffer(), doip_vehicle_payload_type.first,
                          doip_vehicle_payload_type.second);
  // Copy only if containing VIN / EID
  if (doip_vehicle_payload_type.first != kDoip_VehicleIdentification_ReqType) {
    doip_vehicle_identification_req->GetTxBuffer().insert(
        doip_vehicle_identification_req->GetTxBuffer().begin() + kDoipheadrSize,
        vehicle_identification_request->GetPayload().begin() + 2U, vehicle_identification_request->GetPayload().end());
  }
  if (handler_impl_->GetSocketHandler().Transmit(std::move(doip_vehicle_identification_req))) {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client
