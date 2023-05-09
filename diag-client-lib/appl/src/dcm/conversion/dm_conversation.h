/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DMCONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DMCONVERSATION_H
/* includes */
#include <string_view>

#include "include/diagnostic_client_conversation.h"
#include "src/dcm/conversion/dm_conversation_state_impl.h"
#include "uds_transport/connection.h"
#include "uds_transport/conversion_handler.h"
#include "uds_transport/protocol_types.h"
#include "utility/sync_timer.h"

namespace diag {
namespace client {
namespace conversation {

using ConversationState = conversation_state_impl::ConversationState;

/*
 @ Class Name        : DmConversation
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class DmConversation final : public ::diag::client::conversation::DiagClientConversation {
public:
  using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;
  using SyncTimerState = SyncTimer::TimerState;

  // ctor
  DmConversation(std::string_view conversion_name,
                 ::uds_transport::conversion_manager::ConversionIdentifierType &conversion_identifier);

  // dtor
  ~DmConversation();

  // startup
  void Startup() override;

  // shutdown
  void Shutdown() override;

  // Description   : Function to connect to Diagnostic Server
  // @param input  : Nothing
  // @return value : ConnectResult
  ConnectResult ConnectToDiagServer(std::uint16_t target_address, IpAddress host_ip_addr) override;

  // Description   : Function to disconnect from Diagnostic Server
  // @param input  : Nothing
  // @return value : DisconnectResult
  DisconnectResult DisconnectFromDiagServer() override;

  // Description   : Function to send Diagnostic Request and receive response
  // @param input  : Nothing
  // @return value : DisconnectResult
  std::pair<DiagResult, uds_message::UdsResponseMessagePtr> SendDiagnosticRequest(
      uds_message::UdsRequestMessageConstPtr message) override;

  // Register Connection
  void RegisterConnection(std::shared_ptr<::uds_transport::Connection> connection);

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult, ::uds_transport::UdsMessagePtr> IndicateMessage(
      ::uds_transport::UdsMessage::Address source_addr, ::uds_transport::UdsMessage::Address target_addr,
      ::uds_transport::UdsMessage::TargetAddressType type, ::uds_transport::ChannelID channel_id, std::size_t size,
      ::uds_transport::Priority priority, ::uds_transport::ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo);

  // Hands over a valid message to conversion
  void HandleMessage(::uds_transport::UdsMessagePtr message);

  // shared pointer to store the conversion handler
  std::shared_ptr<::uds_transport::ConversionHandler> dm_conversion_handler_;

  DiagClientConversation::DiagResult ConvertResponseType(
      ::uds_transport::UdsTransportProtocolMgr::TransmissionResult result_type);

private:
  // Type for active diagnostic session
  enum class SessionControlType : uint8_t {
    kDefaultSession = 0x01,
    kProgrammingSession = 0x02,
    kExtendedSession = 0x03,
    kSystemSafetySession = 0x04
  };
  // Type for active security level
  enum class SecurityLevelType : uint8_t {
    kLocked = 0x00,
    kUnLocked = 0x01,
  };
  // Type of current activity status
  enum class ActivityStatusType : uint8_t { kActive = 0x00, kInactive = 0x01 };

  // Function to wait for response
  void WaitForResponse(std::function<void()> &&timeout_func, std::function<void()> &&cancel_func, int msec);

  // Function to cancel the synchronous wait
  void WaitCancel();

private:
  // Conversion activity Status
  ActivityStatusType activity_status_;
  // Dcm session
  SessionControlType active_session_;
  // Dcm Security
  SecurityLevelType active_security_;
  // Reception buffer
  uint32_t rx_buffer_size_;
  // p2 client time
  uint16_t p2_client_max_;
  // p2 star Client time
  uint16_t p2_star_client_max_;
  // logical Source address
  uint16_t source_address_;
  // logical target address
  uint16_t target_address_;
  // port number
  uint16_t port_num_;
  // Vehicle broadcast address
  std::string broadcast_address;
  // remote Ip Address
  std::string remote_address_;
  // conversion name
  std::string conversation_name_;
  // Tp connection
  std::shared_ptr<::uds_transport::Connection> connection_ptr_;
  // timer
  SyncTimer sync_timer_;
  // rx buffer to store the uds response
  ::uds_transport::ByteVector payload_rx_buffer;
  // conversation state
  conversation_state_impl::ConversationStateImpl conversation_state_;
};

/*
 @ Class Name        : DmConversationHandler
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class DmConversationHandler : public ::uds_transport::ConversionHandler {
public:
  // ctor
  DmConversationHandler(::uds_transport::conversion_manager::ConversionHandlerID handler_id,
                        DmConversation &dm_conversion);

  // dtor
  ~DmConversationHandler() = default;

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult, ::uds_transport::UdsMessagePtr> IndicateMessage(
      ::uds_transport::UdsMessage::Address source_addr, ::uds_transport::UdsMessage::Address target_addr,
      ::uds_transport::UdsMessage::TargetAddressType type, ::uds_transport::ChannelID channel_id, std::size_t size,
      ::uds_transport::Priority priority, ::uds_transport::ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo) override;

  // Hands over a valid message to conversion
  void HandleMessage(::uds_transport::UdsMessagePtr message) override;

private:
  DmConversation &dm_conversation_e;
};
}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DMCONVERSATION_H
