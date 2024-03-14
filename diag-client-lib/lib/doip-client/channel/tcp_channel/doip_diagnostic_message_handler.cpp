/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "channel/tcp_channel/doip_diagnostic_message_handler.h"

#include <algorithm>
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
 * @brief  Diagnostic message type
 */
constexpr std::uint16_t kDoipDiagMessage{0x8001};
constexpr std::uint16_t kDoipDiagMessagePosAck{0x8002};
constexpr std::uint16_t kDoipDiagMessageNegAck{0x8003};

/**
 * @brief  Diagnostic Message negative acknowledgement code
 */
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeInvalidSa{0x02};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeUnknownTa{0x03};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeMessageTooLarge{0x04};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeOutOfMemory{0x05};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeTargetUnreachable{0x06};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeUnknownNetwork{0x07};
constexpr std::uint8_t kDoipDiagnosticMessageNegAckCodeTpError{0x08};

/**
 * @brief  Diagnostic Message positive acknowledgement code
 */
constexpr std::uint8_t kDoipDiagnosticMessagePosAckCodeConfirm{0x00};

/**
 * @brief  Diagnostic Message request/response lengths
 */
constexpr std::uint8_t kDoipDiagMessageReqResMinLen = 4U;  // considering SA and TA
constexpr std::uint8_t kDoipDiagMessageAckResMinLen = 5U;  // considering SA, TA, Ack code

/* Description: This timeout specifies the maximum time that
                the test equipment waits for a confirmation ACK or NACK
                from the DoIP entity after the last byte of a DoIP Diagnostic
                request message has been sent
*/
/**
 * @brief   The timeout specifies the maximum time that the test equipment waits for a confirmation ACK or NACK
 *          from the DoIP entity after the last byte of a DoIP Diagnostic request message has been sent
 */
constexpr std::uint32_t kDoIPDiagnosticAckTimeout{2000u};  // 2 sec

/**
 * @brief  Different diagnostic message state
 */
enum class DiagnosticMessageState : std::uint8_t {
  kIdle = 0U,
  kSendDiagnosticReqFailed,
  kWaitForDiagnosticAck,
  kDiagnosticPositiveAckRecvd,
  kDiagnosticNegativeAckRecvd,
  kWaitForDiagnosticResponse,
  kDiagnosticFinalResRecvd
};

/**
 * @brief       Class implements idle state
 */
class kIdle final : public utility::state::State<DiagnosticMessageState> {
 public:
  /**
   * @brief         Constructs an instance of kIdle
   * @param[in]     state
   *                The kIdle state
   */
  explicit kIdle(DiagnosticMessageState state) : State<DiagnosticMessageState>(state) {}

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
 * @brief       Class implements sending of diagnostic request failed
 */
class kSendDiagnosticReqFailed final : public utility::state::State<DiagnosticMessageState> {
 public:
  /**
   * @brief         Constructs an instance of kSendDiagnosticReqFailed
   * @param[in]     state
   *                The kIdle state
   */
  explicit kSendDiagnosticReqFailed(DiagnosticMessageState state) : State<DiagnosticMessageState>(state) {}

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
 * @brief       Class implements wait for diagnostic acknowledgement response
 */
class kWaitForDiagnosticAck final : public utility::state::State<DiagnosticMessageState> {
 public:
  /**
   * @brief         Constructs an instance of kWaitForDiagnosticAck
   * @param[in]     state
   *                The kIdle state
   */
  explicit kWaitForDiagnosticAck(DiagnosticMessageState state) : State<DiagnosticMessageState>(state) {}

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
 * @brief       Class implements reception of diagnostic positive acknowledgement response
 */
class kDiagnosticPositiveAckRecvd final : public utility::state::State<DiagnosticMessageState> {
 public:
  /**
   * @brief         Constructs an instance of kDiagnosticPositiveAckRecvd
   * @param[in]     state
   *                The kDiagnosticPositiveAckRecvd state
   */
  explicit kDiagnosticPositiveAckRecvd(DiagnosticMessageState state) : State<DiagnosticMessageState>(state) {}

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
 * @brief       Class implements reception of diagnostic negative acknowledgement response
 */
class kDiagnosticNegativeAckRecvd final : public utility::state::State<DiagnosticMessageState> {
 public:
  /**
   * @brief         Constructs an instance of kDiagnosticNegativeAckRecvd
   * @param[in]     state
   *                The kDiagnosticNegativeAckRecvd state
   */
  explicit kDiagnosticNegativeAckRecvd(DiagnosticMessageState state) : State<DiagnosticMessageState>(state) {}

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
 * @brief       Class implements wait for diagnostic message positive/negative response
 */
class kWaitForDiagnosticResponse final : public utility::state::State<DiagnosticMessageState> {
 public:
  /**
   * @brief         Constructs an instance of kWaitForDiagnosticResponse
   * @param[in]     state
   *                The kWaitForDiagnosticResponse state
   */
  explicit kWaitForDiagnosticResponse(DiagnosticMessageState state) : State<DiagnosticMessageState>(state) {}

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
 * @brief  Type holding acknowledgement type
 */
struct DiagAckType {
  std::uint8_t ack_type_;
};

/**
 * @brief  Function to stream the diagnostic acknowledgement type
 */
std::ostream &operator<<(std::ostream &msg, DiagAckType diag_ack_type) {
  switch (diag_ack_type.ack_type_) {
    case kDoipDiagnosticMessageNegAckCodeInvalidSa:
      msg << "invalid source address.";
      break;
    case kDoipDiagnosticMessageNegAckCodeUnknownTa:
      msg << "unknown target address.";
      break;
    case kDoipDiagnosticMessageNegAckCodeMessageTooLarge:
      msg << "diagnostic message too large.";
      break;
    case kDoipDiagnosticMessageNegAckCodeOutOfMemory:
      msg << "server out of memory.";
      break;
    case kDoipDiagnosticMessageNegAckCodeTargetUnreachable:
      msg << "target unreachable.";
      break;
    case kDoipDiagnosticMessageNegAckCodeUnknownNetwork:
      msg << "unknown network.";
      break;
    case kDoipDiagnosticMessageNegAckCodeTpError:
      msg << "transport protocol error.";
      break;
    default:
      msg << "unknown reason.";
      break;
  }
  return msg;
}

/**
 * @brief            Function to create doip generic header
 * @param[in]        payload_type
 *                   The type of payload
 * @param[in]        payload_len
 *                   The length of payload
 */
auto CreateDoipGenericHeader(std::uint16_t payload_type, std::uint32_t payload_len) noexcept
    -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> output_buffer{};
  output_buffer.emplace_back(kDoip_ProtocolVersion);
  output_buffer.emplace_back(~(static_cast<std::uint8_t>(kDoip_ProtocolVersion)));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_type & 0xFF00) >> 8));
  output_buffer.emplace_back(static_cast<std::uint8_t>(payload_type & 0x00FF));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0xFF000000) >> 24));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x00FF0000) >> 16));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x0000FF00) >> 8));
  output_buffer.emplace_back(static_cast<std::uint8_t>(payload_len & 0x000000FF));
  return output_buffer;
}

}  // namespace

/**
 * @brief       Class implements routing activation handler
 */
class DiagnosticMessageHandler::DiagnosticMessageHandlerImpl {
 public:
  /**
   * @brief  Type alias for state context
   */
  using DiagnosticMessageStateContext = utility::state::StateContext<DiagnosticMessageState>;

  /**
   * @brief  Type alias for Sync timer
   */
  using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;

  /**
   * @brief         Constructs an instance of DiagnosticMessageHandlerImpl
   * @param[in]     tcp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to doip channel
   */
  DiagnosticMessageHandlerImpl(sockets::TcpSocketHandler &tcp_socket_handler, DoipTcpChannel &channel)
      : tcp_socket_handler_{tcp_socket_handler},
        channel_{channel},
        state_context_{},
        sync_timer_{} {
    // create and add state for Diagnostic State
    // kIdle
    state_context_.AddState(DiagnosticMessageState::kIdle, std::make_unique<kIdle>(DiagnosticMessageState::kIdle));
    // kSendDiagnosticReqFailed
    state_context_.AddState(
        DiagnosticMessageState::kSendDiagnosticReqFailed,
        std::make_unique<kSendDiagnosticReqFailed>(DiagnosticMessageState::kSendDiagnosticReqFailed));
    // kWaitForDiagnosticAck
    state_context_.AddState(DiagnosticMessageState::kWaitForDiagnosticAck,
                            std::make_unique<kWaitForDiagnosticAck>(DiagnosticMessageState::kWaitForDiagnosticAck));
    // kDiagnosticPositiveAckRecvd
    state_context_.AddState(
        DiagnosticMessageState::kDiagnosticPositiveAckRecvd,
        std::make_unique<kDiagnosticPositiveAckRecvd>(DiagnosticMessageState::kDiagnosticPositiveAckRecvd));
    // kDiagnosticNegativeAckRecvd
    state_context_.AddState(
        DiagnosticMessageState::kDiagnosticNegativeAckRecvd,
        std::make_unique<kDiagnosticNegativeAckRecvd>(DiagnosticMessageState::kDiagnosticNegativeAckRecvd));
    // kWaitForDiagnosticResponse
    state_context_.AddState(
        DiagnosticMessageState::kWaitForDiagnosticResponse,
        std::make_unique<kWaitForDiagnosticResponse>(DiagnosticMessageState::kWaitForDiagnosticResponse));
    // Transit to idle state
    state_context_.TransitionTo(DiagnosticMessageState::kIdle);
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
    state_context_.TransitionTo(DiagnosticMessageState::kIdle);
  }

  /**
   * @brief        Function to reset the handler
   * @details      This will reset all the internal handler back to default state
   */
  void Reset() { Stop(); }

  /**
   * @brief       Function to get the Diagnostic Message State context
   * @return      The reference to state context
   */
  auto GetStateContext() noexcept -> DiagnosticMessageStateContext & { return state_context_; }

  /**
   * @brief       Function to get the socket handler
   * @return      The reference to socket handler
   */
  auto GetSocketHandler() noexcept -> sockets::TcpSocketHandler & { return tcp_socket_handler_; }

  /**
   * @brief       Function to get the doip channel
   * @return      The reference to channel
   */
  auto GetDoipChannel() noexcept -> DoipTcpChannel & { return channel_; }

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
   * @brief  The reference to doip channel
   */
  DoipTcpChannel &channel_;

  /**
   * @brief  Stores the diagnostic message states
   */
  DiagnosticMessageStateContext state_context_;

  /**
   * @brief  Store the synchronous timer
   */
  SyncTimer sync_timer_;
};

DiagnosticMessageHandler::DiagnosticMessageHandler(sockets::TcpSocketHandler &tcp_socket_handler,
                                                   DoipTcpChannel &channel)
    : handler_impl_{std::make_unique<DiagnosticMessageHandlerImpl>(tcp_socket_handler, channel)} {}

DiagnosticMessageHandler::~DiagnosticMessageHandler() = default;

void DiagnosticMessageHandler::Start() { handler_impl_->Start(); }

void DiagnosticMessageHandler::Stop() { handler_impl_->Stop(); }

void DiagnosticMessageHandler::Reset() { handler_impl_->Reset(); }

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept -> void {
  DiagnosticMessageState final_state{DiagnosticMessageState::kDiagnosticNegativeAckRecvd};
  if (handler_impl_->GetStateContext().GetActiveState().GetState() == DiagnosticMessageState::kWaitForDiagnosticAck) {
    // get the ack code
    DiagAckType const diag_ack_type{doip_payload.GetPayload()[0u]};
    if (doip_payload.GetPayloadType() == kDoipDiagMessagePosAck) {
      if (diag_ack_type.ack_type_ == kDoipDiagnosticMessagePosAckCodeConfirm) {
        final_state = DiagnosticMessageState::kDiagnosticPositiveAckRecvd;
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, __func__, [&doip_payload](std::stringstream &msg) {
              msg << "Diagnostic message positively acknowledged from remote "
                     "server "
                  << " (0x" << std::hex << doip_payload.GetServerAddress() << ")";
            });
      } else {
        // do nothing
      }
    } else if (doip_payload.GetPayloadType() == kDoipDiagMessageNegAck) {
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
          __FILE__, __LINE__, __func__,
          [&diag_ack_type](std::stringstream &msg) { msg << "Diagnostic request denied due to " << diag_ack_type; });
    } else {
      // do nothing
    }
    handler_impl_->GetStateContext().TransitionTo(final_state);
    handler_impl_->GetSyncTimer().CancelWait();
  } else {
    /* ignore */
  }
}

auto DiagnosticMessageHandler::ProcessDoIPDiagnosticMessageResponse(DoipMessage &doip_payload) noexcept -> void {
  if (handler_impl_->GetStateContext().GetActiveState().GetState() ==
      DiagnosticMessageState::kWaitForDiagnosticResponse) {
    // Indicate upper layer about incoming data
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
        handler_impl_->GetDoipChannel().IndicateMessage(
            doip_payload.GetServerAddress(), doip_payload.GetClientAddress(),
            uds_transport::UdsMessage::TargetAddressType::kPhysical, 0U, doip_payload.GetPayload().size(), 0u,
            "DoIPTcp", doip_payload.GetPayload())};
    if (ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending) {
      // keep channel alive since pending request received, do not change channel state
    } else {
      // Check result and udsMessagePtr
      if ((ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
          (ret_val.second != nullptr)) {
        // copy to application buffer
        (void) std::copy(doip_payload.GetPayload().begin(), doip_payload.GetPayload().end(),
                         ret_val.second->GetPayload().begin());
        handler_impl_->GetDoipChannel().HandleMessage(std::move(ret_val.second));
      } else {
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
            __FILE__, __LINE__, __func__,
            [](std::stringstream &msg) { msg << "Diagnostic message response ignored due to unknown error"; });
      }
      handler_impl_->GetStateContext().TransitionTo(DiagnosticMessageState::kIdle);
    }
  } else {
    // ignore
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          msg << "Diagnostic message response ignored due to channel in state: "
              << static_cast<int>(handler_impl_->GetStateContext().GetActiveState().GetState());
        });
  }
}

auto DiagnosticMessageHandler::HandleDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult result{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  if (handler_impl_->GetStateContext().GetActiveState().GetState() == DiagnosticMessageState::kIdle) {
    if (SendDiagnosticRequest(std::move(diagnostic_request)) ==
        uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      handler_impl_->GetStateContext().TransitionTo(DiagnosticMessageState::kWaitForDiagnosticAck);
      handler_impl_->GetSyncTimer().WaitForTimeout(
          [this, &result]() {
            result = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNoTransmitAckReceived;
            handler_impl_->GetStateContext().TransitionTo(DiagnosticMessageState::kIdle);
            logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
                __FILE__, __LINE__, "", [](std::stringstream &msg) {
                  msg << "Diagnostic Message Ack Request timed out, no "
                         "response received in: "
                      << kDoIPDiagnosticAckTimeout << "seconds";
                });
          },
          [this, &result]() {
            if (handler_impl_->GetStateContext().GetActiveState().GetState() ==
                DiagnosticMessageState::kDiagnosticPositiveAckRecvd) {
              handler_impl_->GetStateContext().TransitionTo(DiagnosticMessageState::kWaitForDiagnosticResponse);
              // success
              result = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "Diagnostic Message Positive Ack received"; });
            } else {
              // failed with neg acknowledgement from server
              result = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNegTransmitAckReceived;
              handler_impl_->GetStateContext().TransitionTo(DiagnosticMessageState::kIdle);
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                  __FILE__, __LINE__, "", [](std::stringstream &msg) {
                    msg << "Diagnostic Message Transmission Failed Neg Ack "
                           "Received";
                  });
            }
          },
          std::chrono::milliseconds{kDoIPDiagnosticAckTimeout});
    } else {
      // Failed, do nothing
      handler_impl_->GetStateContext().TransitionTo(DiagnosticMessageState::kIdle);
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, "",
          [](std::stringstream &msg) { msg << "Diagnostic Request Message Transmission Failed"; });
    }
  } else {
    // channel not in idle state
    result = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kBusyProcessing;
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, "",
        [](std::stringstream &msg) { msg << "Diagnostic Message Transmission already in progress"; });
  }
  return result;
}

auto DiagnosticMessageHandler::SendDiagnosticRequest(uds_transport::UdsMessageConstPtr diagnostic_request) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  constexpr std::uint8_t kSourceAddressSize{4u};
  // Create header
  std::uint32_t const total_diagnostic_response_length{
      static_cast<uint32_t>(kDoipDiagMessageReqResMinLen + diagnostic_request->GetPayload().size())};
  TcpMessage::BufferType compose_diag_req{CreateDoipGenericHeader(kDoipDiagMessage, total_diagnostic_response_length)};
  compose_diag_req.reserve(kDoipheadrSize + total_diagnostic_response_length);

  // Add source address
  compose_diag_req.emplace_back(static_cast<std::uint8_t>((diagnostic_request->GetSa() & 0xFF00) >> 8u));
  compose_diag_req.emplace_back(static_cast<std::uint8_t>(diagnostic_request->GetSa() & 0x00FF));
  // Add target address
  compose_diag_req.emplace_back(static_cast<std::uint8_t>((diagnostic_request->GetSa() & 0xFF00) >> 8u));
  compose_diag_req.emplace_back(static_cast<std::uint8_t>(diagnostic_request->GetSa() & 0x00FF));
  // Copy data bytes
  compose_diag_req.insert(compose_diag_req.begin() + kDoipheadrSize + kSourceAddressSize,
                          diagnostic_request->GetPayload().begin(), diagnostic_request->GetPayload().end());
  TcpMessagePtr doip_diag_req{std::make_unique<TcpMessage>(
      diagnostic_request->GetHostIpAddress(), diagnostic_request->GetHostPortNumber(), std::move(compose_diag_req))};
  // Initiate transmission
  if (handler_impl_->GetSocketHandler().Transmit(std::move(doip_diag_req))) {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
