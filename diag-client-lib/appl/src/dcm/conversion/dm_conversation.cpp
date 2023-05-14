/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */

#include "src/dcm/conversion/dm_conversation.h"

#include "src/common/logger.h"
#include "src/dcm/service/dm_uds_message.h"

namespace diag {
namespace client {
namespace conversation {
//ctor
DmConversation::DmConversation(std::string_view conversion_name,
                               ::uds_transport::conversion_manager::ConversionIdentifierType &conversion_identifier)
    : diag::client::conversation::DiagClientConversation(),
      activity_status_{ActivityStatusType::kInactive},
      active_session_{SessionControlType::kDefaultSession},
      active_security_{SecurityLevelType::kLocked},
      rx_buffer_size_{conversion_identifier.rx_buffer_size},
      p2_client_max_{conversion_identifier.p2_client_max},
      p2_star_client_max_{conversion_identifier.p2_star_client_max},
      source_address_{conversion_identifier.source_address},
      target_address_{},
      conversation_name_{conversion_name},
      dm_conversion_handler_{std::make_shared<DmConversationHandler>(conversion_identifier.handler_id, *this)} {}

//dtor
DmConversation::~DmConversation() = default;

// startup
void DmConversation::Startup() {
  // initialize the connection
  connection_ptr_->Initialize();
  // start the connection
  connection_ptr_->Start();
  // Change the state to Active
  activity_status_ = ActivityStatusType::kActive;
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(__FILE__, __LINE__, __func__,
                                                                      [&](std::stringstream &msg) {
                                                                        msg << "'" << conversation_name_ << "'"
                                                                            << "-> "
                                                                            << "Startup completed";
                                                                      });
}

// shutdown
void DmConversation::Shutdown() {
  // shutdown connection
  connection_ptr_->Stop();
  // Change the state to InActive
  activity_status_ = ActivityStatusType::kInactive;
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(__FILE__, __LINE__, __func__,
                                                                      [&](std::stringstream &msg) {
                                                                        msg << "'" << conversation_name_ << "'"
                                                                            << "-> "
                                                                            << "Shutdown completed";
                                                                      });
}

DiagClientConversation::ConnectResult DmConversation::ConnectToDiagServer(std::uint16_t target_address,
                                                                          IpAddress host_ip_addr) {
  // create an uds message just to get the port number
  // source address required for Routing Activation
  uds_transport::ByteVector payload{};  // empty payload
  // Send Connect request to doip layer
  DiagClientConversation::ConnectResult const connection_result{static_cast<DiagClientConversation::ConnectResult>(
      connection_ptr_->ConnectToHost(std::move(std::make_unique<diag::client::uds_message::DmUdsMessage>(
          source_address_, target_address, host_ip_addr, payload))))};
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

DiagClientConversation::DisconnectResult DmConversation::DisconnectFromDiagServer() {
  DiagClientConversation::DisconnectResult ret_val{DiagClientConversation::DisconnectResult::kDisconnectFailed};

  if (connection_ptr_->IsConnectToHost()) {
    // Send disconnect request to doip layer
    ret_val = static_cast<DiagClientConversation::DisconnectResult>(connection_ptr_->DisconnectFromHost());
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

std::pair<DiagClientConversation::DiagResult, uds_message::UdsResponseMessagePtr> DmConversation::SendDiagnosticRequest(
    uds_message::UdsRequestMessageConstPtr message) {
  std::pair<DiagClientConversation::DiagResult, uds_message::UdsResponseMessagePtr> ret_val{
      DiagClientConversation::DiagResult::kDiagGenericFailure, nullptr};
  if (message) {
    // fill the data
    uds_transport::ByteVector payload{message->GetPayload()};
    // Initiate Sending of diagnostic request
    uds_transport::UdsTransportProtocolMgr::TransmissionResult transmission_result{
        connection_ptr_->Transmit(std::move(std::make_unique<diag::client::uds_message::DmUdsMessage>(
            source_address_, target_address_, message->GetHostIpAddress(), payload)))};
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
      WaitForResponse(
          [&]() {
            ret_val.first = DiagClientConversation::DiagResult::kDiagResponseTimeout;
            conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kIdle);
            logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                __FILE__, __LINE__, "", [&](std::stringstream &msg) {
                  msg << "'" << conversation_name_ << "'"
                      << "-> "
                      << "Diagnostic Response P2 Timeout happened: " << p2_client_max_ << " milliseconds";
                });
          },
          [&]() {
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
          p2_client_max_);

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
            WaitForResponse(
                [&]() {
                  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                      __FILE__, __LINE__, "", [&](std::stringstream &msg) {
                        msg << "'" << conversation_name_ << "'"
                            << "-> "
                            << "Diagnostic Response P2 Star Timeout happened: " << p2_star_client_max_
                            << " milliseconds";
                        ;
                      });
                  ret_val.first = DiagClientConversation::DiagResult::kDiagResponseTimeout;
                  conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kIdle);
                },
                [&]() {
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
                p2_star_client_max_);
            break;
          case ConversationState::kDiagSuccess:
            // change state to idle, form the uds response and return
            ret_val.second = std::move(std::make_unique<diag::client::uds_message::DmUdsResponse>(payload_rx_buffer));
            ret_val.first = DiagClientConversation::DiagResult::kDiagSuccess;
            conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kIdle);
            break;
          default:
            // nothing
            break;
        }
      }
    } else {
      // failure
      ret_val.first = ConvertResponseType(transmission_result);
    }
  } else {
    ret_val.first = DiagClientConversation::DiagResult::kDiagInvalidParameter;
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogWarn(__FILE__, __LINE__, "",
                                                                        [&](std::stringstream &msg) {
                                                                          msg << "'" << conversation_name_ << "'"
                                                                              << "-> "
                                                                              << "Diagnostic Request message is empty";
                                                                        });
  }
  return ret_val;
}

// Function to add register Connection to conversion
void DmConversation::RegisterConnection(std::shared_ptr<uds_transport::Connection> connection) {
  connection_ptr_ = std::move(connection);
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DmConversation::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                uds_transport::UdsMessage::Address target_addr,
                                uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id,
                                std::size_t size, uds_transport::Priority priority,
                                uds_transport::ProtocolKind protocol_kind, std::vector<uint8_t> payload_info) {
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
      uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationNOk, nullptr};
  // Verify the payload received :-
  if (!payload_info.empty()) {
    // Check for size, else kIndicationOverflow
    if (size <= rx_buffer_size_) {
      // Check for pending response
      if (payload_info[2U] == 0x78U) {
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
            __FILE__, __LINE__, "", [&](std::stringstream &msg) {
              msg << "'" << conversation_name_ << "'"
                  << "-> "
                  << "Diagnostic final response received in Conversation";
            });
        // positive or negative response, provide valid buffer
        // resize the global rx buffer
        payload_rx_buffer.resize(size);
        ret_val.first = uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk;
        ret_val.second = std::move(std::make_unique<diag::client::uds_message::DmUdsMessage>(
            source_address_, target_address_, "", payload_rx_buffer));
        conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagRecvdFinalRes);
      }
      WaitCancel();
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

// Hands over a valid message to conversion
void DmConversation::HandleMessage(uds_transport::UdsMessagePtr message) {
  if (message != nullptr) {
    conversation_state_.GetConversationStateContext().TransitionTo(ConversationState::kDiagSuccess);
  }
}

void DmConversation::WaitForResponse(std::function<void()> &&timeout_func, std::function<void()> &&cancel_func,
                                     int msec) {
  if (sync_timer_.Start(std::chrono::milliseconds(msec)) == SyncTimerState::kTimeout) {
    timeout_func();
  } else {
    cancel_func();
  }
}

void DmConversation::WaitCancel() { sync_timer_.Stop(); }

DiagClientConversation::DiagResult DmConversation::ConvertResponseType(
    uds_transport::UdsTransportProtocolMgr::TransmissionResult result_type) {
  DiagClientConversation::DiagResult ret_result{DiagClientConversation::DiagResult::kDiagGenericFailure};
  switch (result_type) {
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed:
      ret_result = DiagClientConversation::DiagResult::kDiagRequestSendFailed;
      break;
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNoTransmitAckReceived:
      ret_result = DiagClientConversation::DiagResult::kDiagAckTimeout;
      break;
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNegTransmitAckReceived:
      ret_result = DiagClientConversation::DiagResult::kDiagNegAckReceived;
      break;
    case uds_transport::UdsTransportProtocolMgr::TransmissionResult::kBusyProcessing:
      ret_result = DiagClientConversation::DiagResult::kDiagBusyProcessing;
      break;
    default:
      ret_result = DiagClientConversation::DiagResult::kDiagGenericFailure;
      break;
  }
  return ret_result;
}

// ctor
DmConversationHandler::DmConversationHandler(uds_transport::conversion_manager::ConversionHandlerID handler_id,
                                             DmConversation &dm_conversion)
    : uds_transport::ConversionHandler{handler_id},
      dm_conversation_{dm_conversion} {}

// Indicate message Diagnostic message reception over TCP to user
std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
DmConversationHandler::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                       uds_transport::UdsMessage::Address target_addr,
                                       uds_transport::UdsMessage::TargetAddressType type,
                                       uds_transport::ChannelID channel_id, std::size_t size,
                                       uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                                       std::vector<uint8_t> payload_info) {
  return (dm_conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                           payload_info));
}

// Hands over a valid message to conversion
void DmConversationHandler::HandleMessage(uds_transport::UdsMessagePtr message) {
  dm_conversation_.HandleMessage(std::move(message));
}
}  // namespace conversation
}  // namespace client
}  // namespace diag