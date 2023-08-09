/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_H
/* includes */
#include <string_view>

#include "include/diagnostic_client_conversation.h"
#include "src/dcm/conversation/conversation.h"
#include "src/dcm/conversation/dm_conversation_state_impl.h"
#include "src/dcm/conversation/dm_conversation_type.h"
#include "uds_transport/connection.h"
#include "uds_transport/protocol_types.h"
#include "utility/sync_timer.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief    Class to establish connection with Diagnostic Server
 */
class DmConversation final : public Conversation {
public:
  /**
   * @brief         Type alias for conversation internal state
   */
  using ConversationState = conversation_state_impl::ConversationState;
  /**
   * @brief         Type alias for synchronous timer
   */
  using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;

  /**
   * @brief         Constructs an instance of DmConversation
   * @param[in]     conversion_name
   *                The name of conversation
   * @param[in]     conversion_identifier
   *                The identifier consisting of conversation settings
   */
  DmConversation(std::string_view conversion_name, DMConversationType &conversion_identifier);

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  DmConversation(const DmConversation &other) noexcept = delete;
  DmConversation &operator=(const DmConversation &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  DmConversation(DmConversation &&other) noexcept = delete;
  DmConversation &operator=(DmConversation &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of DmConversation
   */
  ~DmConversation() override;

  /**
   * @brief         Function to start the DmConversation
   */
  void Startup() noexcept override;

  /**
   * @brief         Function to shutdown the DmConversation
   */
  void Shutdown() noexcept override;

  /**
   * @brief       Function to register the conversation to underlying transport protocol handler
   * @param[in]   connection
   *              The conversation connection object
   */
  void RegisterConnection(std::shared_ptr<::uds_transport::Connection> connection) noexcept override;

  /**
   * @brief       Function to get the conversation handler from conversation object
   * @return      ConversionHandler &
   *              The reference to conversation handler
   */
  ::uds_transport::ConversionHandler &GetConversationHandler() noexcept override;

  /**
   * @brief       Function to connect to Diagnostic Server
   * @param[in]   target_address
   *              Logical address of the Remote server
   * @param[in]   host_ip_addr
   *              IP address of the Remote server
   * @return      ConnectResult
   *              Connection result returned
   */
  ConnectResult ConnectToDiagServer(std::uint16_t target_address, IpAddress host_ip_addr) noexcept override;

  /**
   * @brief       Function to disconnect from Diagnostic Server
   * @return      DisconnectResult
   *              Disconnection result returned
   */
  DisconnectResult DisconnectFromDiagServer() noexcept override;

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
      core_type::Span<uint8_t> payload_info) noexcept override;

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(::uds_transport::UdsMessagePtr message) noexcept override;

  /**
   * @brief       Function to send Diagnostic Request and get Diagnostic Response
   * @param[in]   message
   *              The diagnostic request message wrapped in a unique pointer
   * @return      DiagResult
   *              The Result returned
   * @return      uds_message::UdsResponseMessagePtr
   *              Diagnostic Response message received, null_ptr in case of error
   */
  Result<uds_message::UdsResponseMessagePtr, DiagError> SendDiagnosticRequest(
      uds_message::UdsRequestMessageConstPtr message) noexcept override;

private:
  /**
   * @brief  Definitions of active diagnostic session
   */
  enum class SessionControlType : std::uint8_t {
    kDefaultSession = 0x01,
    kProgrammingSession = 0x02,
    kExtendedSession = 0x03,
    kSystemSafetySession = 0x04
  };

  /**
   * @brief  Definitions of active security level
   */
  enum class SecurityLevelType : std::uint8_t {
    kLocked = 0x00,
    kUnLocked = 0x01,
  };

  /**
   * @brief  Definitions of current activity status
   */
  enum class ActivityStatusType : uint8_t { kActive = 0x00, kInactive = 0x01 };

private:
  /**
   * @brief       Helper function to convert response type
   * @param[in]   result_type
   *              The transmission result type
   * @return      DiagResult
   *              The diagnostic result type
   */
  static DiagClientConversation::DiagError ConvertResponseType(
      ::uds_transport::UdsTransportProtocolMgr::TransmissionResult result_type);

  /**
   * @brief       Store the conversation activity status
   */
  ActivityStatusType activity_status_;

  /**
   * @brief       Store the active diagnostic session
   */
  SessionControlType active_session_;

  /**
   * @brief       Store the active diagnostic security level
   */
  SecurityLevelType active_security_level_;

  /**
   * @brief       Store the size of reception buffer size setting
   */
  std::uint32_t rx_buffer_size_;

  /**
   * @brief       Store the maximum p2 client time
   */
  std::uint16_t p2_client_max_;

  /**
   * @brief       Store the maximum p2 star client time
   */
  std::uint16_t p2_star_client_max_;

  /**
   * @brief       Store the logical source address of conversation
   */
  std::uint16_t source_address_;

  /**
   * @brief       Store the logical target address of remote server
   */
  std::uint16_t target_address_;

  /**
   * @brief       Store the remote IP address of remote server
   */
  std::string remote_address_;

  /**
   * @brief       Store the conversation name
   */
  std::string conversation_name_;

  /**
   * @brief       Store the dm conversation handler
   */
  std::unique_ptr<::uds_transport::ConversionHandler> dm_conversion_handler_;

  /**
   * @brief       Store the underlying transport protocol connection object
   */
  std::shared_ptr<::uds_transport::Connection> connection_ptr_;

  /**
   * @brief       Store the synchronous timer
   */
  SyncTimer sync_timer_;

  /**
   * @brief       Store the received uds response
   */
  ::uds_transport::ByteVector payload_rx_buffer_;

  /**
   * @brief       Store the conversation state
   */
  conversation_state_impl::ConversationStateImpl conversation_state_;
};

}  // namespace conversation
}  // namespace client
}  // namespace diag

#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_DM_CONVERSATION_H
