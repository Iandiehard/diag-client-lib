/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "src/dcm/conversation/dm_conversation.h"

#include "src/common/logger.h"
#include "src/dcm/service/dm_uds_message.h"
#include "uds_transport/conversation_handler.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief    Class to manage reception from transport protocol handler to dm connection handler
 */
class DmConversationHandler final : public ::uds_transport::ConversionHandler {
 public:
  /**
   * @brief         Constructs an instance of DmConversationHandler
   * @param[in]     handler_id
   *                The handle id of conversation
   * @param[in]     dm_conversion
   *                The reference of dm conversation
   */
  DmConversationHandler(::uds_transport::conversion_manager::ConversionHandlerID handler_id,
                        DmConversation &dm_conversion)
      : uds_transport::ConversionHandler{handler_id},
        dm_conversation_{dm_conversion} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  DmConversationHandler(const DmConversationHandler &other) noexcept = delete;
  DmConversationHandler &operator=(const DmConversationHandler &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  DmConversationHandler(DmConversationHandler &&other) noexcept = delete;
  DmConversationHandler &operator=(DmConversationHandler &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of DmConversationHandler
   */
  ~DmConversationHandler() override = default;

  /**
   * @brief       Function to indicate a start of reception of message
   * @details     This is called to indicate the reception of new message by underlying transport protocol handler
   * @param[in]   source_addr
   *              The UDS source address of message
   * @param[in]   target_addr
   *              The UDS target address of message
   * @param[in]   type
   *              The indication whether its is phys/func request
   * @param[in]   channel_id
   *              The transport protocol channel on which message start happened
   * @param[in]   size
   *              The size in bytes of the UdsMessage starting from SID
   * @param[in]   priority
   *              The priority of the given message, used for prioritization of conversations
   * @param[in]   protocol_kind
   *              The identifier of protocol kind associated to message
   * @param[in]   payload_info
   *              The View onto the first received payload bytes, if any. This view shall be used only within this function call.
   *              It is recommended that the TP provides at least the first two bytes of the request message,
   *              so the DM can identify a functional TesterPresent
   * @return      std::pair< IndicationResult, UdsMessagePtr >
   *              The pair of IndicationResult and a pointer to UdsMessage owned/created by DM core and returned
   *              to the handler to get filled
   */
  std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult, ::uds_transport::UdsMessagePtr> IndicateMessage(
      ::uds_transport::UdsMessage::Address source_addr, ::uds_transport::UdsMessage::Address target_addr,
      ::uds_transport::UdsMessage::TargetAddressType type, ::uds_transport::ChannelID channel_id, std::size_t size,
      ::uds_transport::Priority priority, ::uds_transport::ProtocolKind protocol_kind,
      core_type::Span<std::uint8_t const> payload_info) const noexcept override {
    return (dm_conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                             payload_info));
  }

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(::uds_transport::UdsMessagePtr message) const noexcept override {
    dm_conversation_.HandleMessage(std::move(message));
  }

 private:
  /**
   * @brief         Store the reference of dm conversation
   */
  DmConversation &dm_conversation_;
};

DmConversation::DmConversation(std::string_view conversion_name, DMConversationType &conversion_identifier)
    : Conversation{},
      active_session_{SessionControlType::kDefaultSession},
      active_security_level_{SecurityLevelType::kLocked},
      rx_buffer_size_{conversion_identifier.rx_buffer_size},
      p2_client_max_{conversion_identifier.p2_client_max},
      p2_star_client_max_{conversion_identifier.p2_star_client_max},
      source_address_{conversion_identifier.source_address},
      target_address_{},
      conversation_name_{conversion_name},
      dm_conversion_handler_{std::make_unique<DmConversationHandler>(conversion_identifier.handler_id, *this)} {
  (void) (active_session_);
  (void) (active_security_level_);
}

DmConversation::~DmConversation() = default;

void DmConversation::Startup() noexcept {
  // initialize the connection
  static_cast<void>(connection_->Initialize());
  // start the connection
  connection_->Start();
  // Change the state to Active
  activity_status_ = ActivityStatusType::kActive;
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(__FILE__, __LINE__, __func__,
                                                                      [&](std::stringstream &msg) {
                                                                        msg << "'" << conversation_name_ << "'"
                                                                            << "-> "
                                                                            << "Startup completed";
                                                                      });
}

void DmConversation::Shutdown() noexcept {
  if (GetActivityStatus() == ActivityStatusType::kActive) {
    // shutdown connection
    connection_->Stop();
    // Change the state to InActive
    activity_status_ = ActivityStatusType::kInactive;
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(__FILE__, __LINE__, __func__,
                                                                        [&](std::stringstream &msg) {
                                                                          msg << "'" << conversation_name_ << "'"
                                                                              << "-> "
                                                                              << "Shutdown completed";
                                                                        });
  }
}

DiagClientConversation::ConnectResult DmConversation::ConnectToDiagServer(std::uint16_t const target_address,
                                                                          IpAddress const host_ip_addr) noexcept {
  // create an uds message just to get the port number
  // source address required for Routing Activation
  uds_transport::ByteVector payload{};  // empty payload
  // Send Connect request to doip layer
  DiagClientConversation::ConnectResult const connection_result{static_cast<DiagClientConversation::ConnectResult>(
      connection_->ConnectToHost(std::make_unique<diag::client::uds_message::DmUdsMessage>(
          source_address_, target_address, host_ip_addr, payload)))};
  remote_address_ = host_ip_addr;
  target_address_ = target_address;
  if (connection_result == DiagClientConversation::ConnectResult::kConnectSuccess) {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          msg << "'" << conversation_name_ << "'"
              << "-> "
              << "Successfully connected to Server with IP <" << remote_address_ << ">"
              << " and LA= 0x" << std::hex << target_address_;
        });
  } else {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          msg << "'" << conversation_name_ << "'"
              << "-> "
              << "Failed connecting to Server with IP <" << remote_address_ << ">"
              << " and LA= 0x" << std::hex << target_address_;
        });
  }
  return connection_result;
}

DiagClientConversation::DisconnectResult DmConversation::DisconnectFromDiagServer() noexcept {
  DiagClientConversation::DisconnectResult ret_val{DiagClientConversation::DisconnectResult::kDisconnectFailed};
  // Check if already connected before disconnecting
  if (connection_->IsConnectToHost()) {
    // Send disconnect request to doip layer
    ret_val = static_cast<DiagClientConversation::DisconnectResult>(connection_->DisconnectFromHost());
    if (ret_val == DiagClientConversation::DisconnectResult::kDisconnectSuccess) {
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
          __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
            msg << "'" << conversation_name_ << "'"
                << "-> "
                << "Successfully disconnected from Server with IP <" << remote_address_ << ">"
                << " and LA= 0x" << std::hex << target_address_;
          });
    } else {
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
          __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
            msg << "'" << conversation_name_ << "'"
                << "-> "
                << "Failed to disconnect from Server with IP <" << remote_address_ << ">";
          });
    }
  } else {
    ret_val = DiagClientConversation::DisconnectResult::kAlreadyDisconnected;
  }
  return ret_val;
}

Result<uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError> DmConversation::SendDiagnosticRequest(
    uds_message::UdsRequestMessageConstPtr message) noexcept {
  Result<uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError> result{
      Result<uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError>::FromError(
          DiagClientConversation::DiagError::kDiagRequestSendFailed)};
  if (message) {
    // fill the data
    uds_transport::ByteVector payload{message->GetPayload()};
    // Initiate Sending of diagnostic request
    uds_transport::UdsTransportProtocolMgr::TransmissionResult const transmission_result{
        connection_->Transmit(std::make_unique<diag::client::uds_message::DmUdsMessage>(
            source_address_, target_address_, message->GetHostIpAddress(), payload))};
    if (transmission_result == uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      // Diagnostic Request Sent successful
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
          __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
            msg << "'" << conversation_name_ << "'"
                << "-> "
                << "Diagnostic Request Sent & Positive Ack received";
          });
      conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagWaitForRes);
      // Wait P6Max / P2ClientMax
      sync_timer_.WaitForTimeout(
          [this, &result]() {
            result.EmplaceError(DiagClientConversation::DiagError::kDiagResponseTimeout);
            conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kIdle);
            logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                __FILE__, __LINE__, "", [&](std::stringstream &msg) {
                  msg << "'" << conversation_name_ << "'"
                      << "-> "
                      << "Diagnostic Response P2 Timeout happened after " << p2_client_max_ << " milliseconds";
                });
          },
          [this]() {
            // pending or pos/neg response
            if (conversation_state_.GetConversationStateContext().GetActiveState().GetState() ==
                ConversationState::kDiagRecvdFinalRes) {
              // pos/neg response received
            } else if (conversation_state_.GetConversationStateContext().GetActiveState().GetState() ==
                       ConversationState::kDiagRecvdPendingRes) {
              // first pending received
              conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagStartP2StarTimer);
            }
          },
          std::chrono::milliseconds{p2_client_max_});

      // Wait until final response or timeout
      while (conversation_state_.GetConversationStateContext().GetActiveState().GetState() !=
             ConversationState::kIdle) {
        // Check the active state
        switch (conversation_state_.GetConversationStateContext().GetActiveState().GetState()) {
          case ConversationState::kDiagRecvdPendingRes:
            conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagStartP2StarTimer);
            break;
          case ConversationState::kDiagRecvdFinalRes:
            // do nothing
            break;
          case ConversationState::kDiagStartP2StarTimer:
            // wait P6Star/ P2 star client time
            sync_timer_.WaitForTimeout(
                [this, &result]() {
                  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                      __FILE__, __LINE__, "", [&](std::stringstream &msg) {
                        msg << "'" << conversation_name_ << "'"
                            << "-> "
                            << "Diagnostic Response P2 Star Timeout "
                               "happened after "
                            << p2_star_client_max_ << " milliseconds";
                      });
                  result.EmplaceError(DiagClientConversation::DiagError::kDiagResponseTimeout);
                  conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kIdle);
                },
                [this]() {
                  // pending or pos/neg response
                  if (conversation_state_.GetConversationStateContext().GetActiveState().GetState() ==
                      ConversationState::kDiagRecvdFinalRes) {
                    // pos/neg response received
                  } else if (conversation_state_.GetConversationStateContext().GetActiveState().GetState() ==
                             ConversationState::kDiagRecvdPendingRes) {
                    // pending received again
                    conversation_state_.GetConversationStateContext().TransitionTo(
                        ConversationState::kDiagStartP2StarTimer);
                  }
                },
                std::chrono::milliseconds{p2_star_client_max_});
            break;
          case ConversationState::kDiagSuccess:
            // change state to idle, form the uds response and return
            result.EmplaceValue(std::make_unique<diag::client::uds_message::DmUdsResponse>(payload_rx_buffer_));
            conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kIdle);
            break;
          default:
            // nothing
            break;
        }
      }
    } else {
      // failure
      result.EmplaceError(ConvertResponseType(transmission_result));
    }
  } else {
    result.EmplaceError(DiagClientConversation::DiagError::kDiagInvalidParameter);
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogWarn(__FILE__, __LINE__, "",
                                                                        [&](std::stringstream &msg) {
                                                                          msg << "'" << conversation_name_ << "'"
                                                                              << "-> "
                                                                              << "Diagnostic Request message is empty";
                                                                        });
  }
  return result;
}

void DmConversation::RegisterConnection(std::unique_ptr<uds_transport::Connection> connection) noexcept {
  connection_ = std::move(connection);
}

::uds_transport::ConversionHandler &DmConversation::GetConversationHandler() noexcept {
  return *dm_conversion_handler_;
}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DmConversation::IndicateMessage(uds_transport::UdsMessage::Address, uds_transport::UdsMessage::Address,
                                uds_transport::UdsMessage::TargetAddressType, uds_transport::ChannelID,
                                std::size_t size, uds_transport::Priority, uds_transport::ProtocolKind,
                                core_type::Span<std::uint8_t const> payload_info) noexcept {
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
      uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationNOk, nullptr};
  // Verify the payload received :-
  if (!payload_info.empty()) {
    // Check for size, else kIndicationOverflow
    if (size <= rx_buffer_size_) {
      // Check for pending response
      // payload = 0x7F XX 0x78
      if (payload_info[0U] == 0x7F && payload_info[2U] == 0x78) {
        logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
            __FILE__, __LINE__, "", [&](std::stringstream &msg) {
              msg << "'" << conversation_name_ << "'"
                  << "-> "
                  << "Diagnostic pending response received in Conversation";
            });
        ret_val.first = uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending;
        conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagRecvdPendingRes);
      } else {
        logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
            __FILE__, __LINE__, "", [this](std::stringstream &msg) {
              msg << "'" << conversation_name_ << "'"
                  << "-> "
                  << "Diagnostic final response received in Conversation";
            });
        // positive or negative response, provide valid buffer
        // resize the global rx buffer
        payload_rx_buffer_.resize(size);
        ret_val.first = uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk;
        ret_val.second = std::make_unique<diag::client::uds_message::DmUdsMessage>(source_address_, target_address_, "",
                                                                                   payload_rx_buffer_);
        conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagRecvdFinalRes);
      }
      sync_timer_.CancelWait();
    } else {
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, "", [&](std::stringstream &msg) {
            msg << "'" << conversation_name_ << "'"
                << "-> "
                << "Diagnostic Conversation Error Indication Overflow";
          });
      ret_val.first = uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOverflow;
    }
  } else {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
        __FILE__, __LINE__, "", [&](std::stringstream &msg) {
          msg << "'" << conversation_name_ << "'"
              << "-> "
              << "Diagnostic Conversation Rx Payload size 0 received";
        });
  }
  return ret_val;
}

void DmConversation::HandleMessage(uds_transport::UdsMessagePtr message) noexcept {
  if (message != nullptr) {
    conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagSuccess);
  }
}

DiagClientConversation::DiagError DmConversation::ConvertResponseType(
    uds_transport::UdsTransportProtocolMgr::TransmissionResult result_type) {
  DiagClientConversation::DiagError ret_result{DiagClientConversation::DiagError::kDiagGenericFailure};
  switch (result_type) {
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed:
      ret_result = DiagClientConversation::DiagError::kDiagRequestSendFailed;
      break;
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNoTransmitAckReceived:
      ret_result = DiagClientConversation::DiagError::kDiagAckTimeout;
      break;
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNegTransmitAckReceived:
      ret_result = DiagClientConversation::DiagError::kDiagNegAckReceived;
      break;
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kBusyProcessing:
      ret_result = DiagClientConversation::DiagError::kDiagBusyProcessing;
      break;
    default:
      ret_result = DiagClientConversation::DiagError::kDiagGenericFailure;
      break;
  }
  return ret_result;
}

}  // namespace conversation
}  // namespace client
}  // namespace diag