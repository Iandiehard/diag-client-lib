/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "channel/tcp_channel/doip_routing_activation_handler.h"

#include <utility>

#include "channel/tcp_channel/doip_tcp_channel.h"
#include "common/common_doip_types.h"
#include "common/logger.h"
#include "utility/state.h"
#include "utility/sync_timer.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {
namespace {

/**
 * @brief  Routing Activation request activation types
 */
constexpr std::uint8_t kDoip_RoutingActivation_ReqActType_Default{0x00};
constexpr std::uint8_t kDoip_RoutingActivation_ReqActType_WWHOBD{0x01};
constexpr std::uint8_t kDoip_RoutingActivation_ReqActType_CentralSec{0xE0};

/**
 * @brief  Routing Activation response code values
 */
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_UnknownSA{0x00};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_AllSocktActive{0x01};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_DifferentSA{0x02};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_ActiveSA{0x03};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_AuthentnMissng{0x04};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_ConfirmtnRejectd{0x05};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_UnsupportdActType{0x06};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_TLSRequired{0x07};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_RoutingSuccessful{0x10};
constexpr std::uint8_t kDoip_RoutingActivation_ResCode_ConfirmtnRequired{0x11};

/**
 * @brief  Routing Activation request lengths
 */
constexpr std::uint32_t kDoip_RoutingActivation_ReqMinLen{7u};   //without OEM specific use byte
constexpr std::uint32_t kDoip_RoutingActivation_ResMinLen{9u};   //without OEM specific use byte
constexpr std::uint32_t kDoip_RoutingActivation_ReqMaxLen{11u};  //with OEM specific use byte
constexpr std::uint32_t kDoip_RoutingActivation_ResMaxLen{13u};  //with OEM specific use byte

/**
 * @brief  Routing Activation response Type
 */
constexpr std::uint16_t kDoip_RoutingActivation_ReqType{0x0005};
constexpr std::uint16_t kDoip_RoutingActivation_ResType{0x0006};

/**
 * @brief  The timeout value for a DoIP Routing Activation request
 */
constexpr std::uint32_t kDoIPRoutingActivationTimeout{1000u};  // 1 sec

/**
 * @brief  Different routing activation state
 */
enum class RoutingActivationState : std::uint8_t {
  kIdle = 0U,
  kWaitForRoutingActivationRes,
  kRoutingActivationSuccessful,
  kRoutingActivationFailed
};

/**
 * @brief       Class implements idle state
 */
class kIdle final : public utility::state::State<RoutingActivationState> {
 public:
  /**
   * @brief         Constructs an instance of kIdle
   * @param[in]     state
   *                The kIdle state
   */
  explicit kIdle(RoutingActivationState state) : State<RoutingActivationState>(state) {}

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
 * @brief       Class implements wait for routing activation response
 */
class kWaitForRoutingActivationRes final : public utility::state::State<RoutingActivationState> {
 public:
  /**
   * @brief         Constructs an instance of kWaitForRoutingActivationRes
   * @param[in]     state
   *                The kIdle state
   */
  explicit kWaitForRoutingActivationRes(RoutingActivationState state) : State<RoutingActivationState>(state) {}

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
 * @brief       Class implements routing activation success
 */
class kRoutingActivationSuccessful final : public utility::state::State<RoutingActivationState> {
 public:
  /**
   * @brief         Constructs an instance of kRoutingActivationSuccessful
   * @param[in]     state
   *                The kIdle state
   */
  explicit kRoutingActivationSuccessful(RoutingActivationState state) : State<RoutingActivationState>(state) {}

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
 * @brief       Class implements routing activation failure
 */
class kRoutingActivationFailed final : public utility::state::State<RoutingActivationState> {
 public:
  /**
   * @brief         Constructs an instance of kRoutingActivationFailed
   * @param[in]     state
   *                The kIdle state
   */
  explicit kRoutingActivationFailed(RoutingActivationState state) : State<RoutingActivationState>(state) {}

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
 * @brief  Type holding activation type
 */
struct RoutingActivationAckType {
  std::uint8_t act_type_;
};

/**
 * @brief  Function to stream the routing activation type
 */
std::ostream &operator<<(std::ostream &msg, RoutingActivationAckType act_type) {
  switch (act_type.act_type_) {
    case kDoip_RoutingActivation_ResCode_UnknownSA:
      msg << "unknown source address.";
      break;
    case kDoip_RoutingActivation_ResCode_AllSocktActive:
      msg << "all Socket active.";
      break;
    case kDoip_RoutingActivation_ResCode_DifferentSA:
      msg << "SA different on already connected socket.";
      break;
    case kDoip_RoutingActivation_ResCode_ActiveSA:
      msg << "SA active on different socket.";
      break;
    case kDoip_RoutingActivation_ResCode_AuthentnMissng:
      msg << "missing authentication.";
      break;
    case kDoip_RoutingActivation_ResCode_ConfirmtnRejectd:
      msg << "rejected confirmation.";
      break;
    case kDoip_RoutingActivation_ResCode_UnsupportdActType:
      msg << "unsupported routing activation type.";
      break;
    case kDoip_RoutingActivation_ResCode_TLSRequired:
      msg << "required TLS socket.";
      break;
    default:
      msg << "unknown reason.";
      break;
  }
  msg << " (0x" << std::hex << static_cast<int>(act_type.act_type_) << ")";
  return msg;
}

/**
 * @brief       Class implements routing activation handler
 */
class RoutingActivationHandler::RoutingActivationHandlerImpl {
 public:
  /**
   * @brief  Type alias for state context
   */
  using RoutingActivationStateContext = utility::state::StateContext<RoutingActivationState>;

  /**
   * @brief  Type alias for Sync timer
   */
  using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;

  /**
   * @brief         Constructs an instance of RoutingActivationHandlerImpl
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   */
  RoutingActivationHandlerImpl(sockets::TcpSocketHandler &tcp_socket_handler)
      : tcp_socket_handler_{tcp_socket_handler},
        state_context_{},
        sync_timer_{} {
    // create and add state for routing activation
    // kIdle
    state_context_.AddState(RoutingActivationState::kIdle, std::make_unique<kIdle>(RoutingActivationState::kIdle));
    // kWaitForRoutingActivationRes
    state_context_.AddState(
        RoutingActivationState::kWaitForRoutingActivationRes,
        std::make_unique<kWaitForRoutingActivationRes>(RoutingActivationState::kWaitForRoutingActivationRes));
    // kRoutingActivationSuccessful
    state_context_.AddState(
        RoutingActivationState::kRoutingActivationSuccessful,
        std::make_unique<kRoutingActivationSuccessful>(RoutingActivationState::kRoutingActivationSuccessful));
    // kRoutingActivationFailed
    state_context_.AddState(
        RoutingActivationState::kRoutingActivationFailed,
        std::make_unique<kRoutingActivationFailed>(RoutingActivationState::kRoutingActivationFailed));
    // Transit to idle state
    state_context_.TransitionTo(RoutingActivationState::kIdle);
  }

  /**
   * @brief        Function to start the handler
   */
  void Start() {}

  /**
   * @brief        Function to stop the handler
   * @details      This will reset all the internal handler back to default state
   */
  void Stop() {
    if (sync_timer_.IsTimerActive()) { sync_timer_.CancelWait(); }
    state_context_.TransitionTo(RoutingActivationState::kIdle);
  }

  /**
   * @brief        Function to reset the handler
   * @details      This will reset all the internal handler back to default state
   */
  void Reset() { Stop(); }

  /**
   * @brief       Function to get the Routing Activation State context
   * @return      The reference to state context
   */
  auto GetStateContext() noexcept -> RoutingActivationStateContext & { return state_context_; }

  /**
   * @brief       Function to get the socket handler
   * @return      The reference to socket handler
   */
  auto GetSocketHandler() noexcept -> sockets::TcpSocketHandler & { return tcp_socket_handler_; }

  /**
   * @brief       Function to get the sync timer
   * @return      The reference to sync timer
   */
  auto GetSyncTimer() noexcept -> SyncTimer & { return sync_timer_; }

 private:
  /**
   * @brief  The reference to socket handler
   */
  sockets::TcpSocketHandler &tcp_socket_handler_;

  /**
   * @brief  Stores the routing activation states
   */
  RoutingActivationStateContext state_context_;

  /**
   * @brief  Store the synchronous timer
   */
  SyncTimer sync_timer_;
};

RoutingActivationHandler::RoutingActivationHandler(sockets::TcpSocketHandler &tcp_socket_handler)
    : handler_impl_{std::make_unique<RoutingActivationHandlerImpl>(tcp_socket_handler)} {}

RoutingActivationHandler::~RoutingActivationHandler() = default;

void RoutingActivationHandler::Start() { handler_impl_->Start(); }

void RoutingActivationHandler::Stop() { handler_impl_->Stop(); }

void RoutingActivationHandler::Reset() { handler_impl_->Reset(); }

auto RoutingActivationHandler::ProcessDoIPRoutingActivationResponse(DoipMessage &doip_payload) noexcept -> void {
  RoutingActivationState final_state{RoutingActivationState::kRoutingActivationFailed};
  if (handler_impl_->GetStateContext().GetActiveState().GetState() ==
      RoutingActivationState::kWaitForRoutingActivationRes) {
    // get the ack code
    RoutingActivationAckType const rout_act_type{doip_payload.GetPayload()[0u]};
    switch (rout_act_type.act_type_) {
      case kDoip_RoutingActivation_ResCode_RoutingSuccessful: {
        // routing successful
        final_state = RoutingActivationState::kRoutingActivationSuccessful;
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&doip_payload](std::stringstream &msg) {
              msg << "RoutingActivation successfully activated in remote server with logical Address"
                  << " (0x" << std::hex << doip_payload.GetServerAddress() << ")";
            });
      } break;
      case kDoip_RoutingActivation_ResCode_ConfirmtnRequired: {
        // trigger routing activation after sometime, not implemented yet
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&doip_payload](std::stringstream &msg) {
              msg << "RoutingActivation is activated, confirmation required in remote server with logical Address"
                  << " (0x" << std::hex << doip_payload.GetServerAddress() << ")";
            });
      } break;
      default:
        // failure, do nothing
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
            __FILE__, __LINE__, __func__,
            [&rout_act_type](std::stringstream &msg) { msg << "Routing activation denied due to " << rout_act_type; });
        break;
    }
    handler_impl_->GetStateContext().TransitionTo(final_state);
    handler_impl_->GetSyncTimer().CancelWait();
  } else {
    /* ignore */
  }
}

auto RoutingActivationHandler::HandleRoutingActivationRequest(
    uds_transport::UdsMessageConstPtr routing_activation_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::ConnectionResult {
  uds_transport::UdsTransportProtocolMgr::ConnectionResult result{
      uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
  if (handler_impl_->GetStateContext().GetActiveState().GetState() == RoutingActivationState::kIdle) {
    if (SendRoutingActivationRequest(std::move(routing_activation_request)) ==
        uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      // Wait for routing activation response
      handler_impl_->GetStateContext().TransitionTo(RoutingActivationState::kWaitForRoutingActivationRes);
      handler_impl_->GetSyncTimer().WaitForTimeout(
          [this, &result]() {
            result = uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionTimeout;
            handler_impl_->GetStateContext().TransitionTo(RoutingActivationState::kIdle);
            logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
                __FILE__, __LINE__, "", [](std::stringstream &msg) {
                  msg << "RoutingActivation response timeout, no response received in: "
                      << kDoIPRoutingActivationTimeout << " milliseconds";
                });
          },
          [this, &result]() {
            if (handler_impl_->GetStateContext().GetActiveState().GetState() ==
                RoutingActivationState::kRoutingActivationSuccessful) {
              // success
              result = uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionOk;
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "RoutingActivation successful with remote server"; });
            } else {  // failed
              handler_impl_->GetStateContext().TransitionTo(RoutingActivationState::kIdle);
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "RoutingActivation failed with remote server"; });
            }
          },
          std::chrono::milliseconds{kDoIPRoutingActivationTimeout});
    } else {
      // failed, do nothing
      handler_impl_->GetStateContext().TransitionTo(RoutingActivationState::kIdle);
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, "",
          [](std::stringstream &msg) { msg << "RoutingActivation Request send failed with remote server"; });
    }
  } else {
    // channel not free
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "RoutingActivation channel not free"; });
  }
  return result;
}

auto RoutingActivationHandler::IsRoutingActivated() noexcept -> bool {
  return (handler_impl_->GetStateContext().GetActiveState().GetState() ==
          RoutingActivationState::kRoutingActivationSuccessful);
}

auto RoutingActivationHandler::SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  TcpMessagePtr doip_routing_act_req{std::make_unique<TcpMessage>()};
  // reserve bytes in vector
  doip_routing_act_req->GetTxBuffer().reserve(kDoipheadrSize + kDoip_RoutingActivation_ReqMinLen);
  // create header
  CreateDoipGenericHeader(doip_routing_act_req->GetTxBuffer(), kDoip_RoutingActivation_ReqType,
                          kDoip_RoutingActivation_ReqMinLen);
  // Add source address
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t)((message->GetSa() & 0xFF00) >> 8u));
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t)(message->GetSa() & 0x00FF));
  // Add activation type
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t) kDoip_RoutingActivation_ReqActType_Default);
  // Add reservation byte , default zeroes
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t) 0x00);
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t) 0x00);
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t) 0x00);
  doip_routing_act_req->GetTxBuffer().emplace_back((std::uint8_t) 0x00);

  // Initiate transmission
  if (handler_impl_->GetSocketHandler().Transmit(std::move(doip_routing_act_req))) {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

void RoutingActivationHandler::CreateDoipGenericHeader(std::vector<std::uint8_t> &doip_header_buffer,
                                                       std::uint16_t payload_type, std::uint32_t payload_len) {
  doip_header_buffer.emplace_back(kDoip_ProtocolVersion);
  doip_header_buffer.emplace_back(~(static_cast<std::uint8_t>(kDoip_ProtocolVersion)));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_type & 0xFF00) >> 8));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>(payload_type & 0x00FF));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0xFF000000) >> 24));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x00FF0000) >> 16));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x0000FF00) >> 8));
  doip_header_buffer.emplace_back(static_cast<std::uint8_t>(payload_len & 0x000000FF));
}

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client