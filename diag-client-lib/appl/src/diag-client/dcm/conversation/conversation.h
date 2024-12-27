/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_H

#include "core/include/result.h"
#include "diag-client/diagnostic_client.h"
#include "diag-client/diagnostic_client_conversation.h"
#include "diag-client/diagnostic_client_vehicle_info_message_type.h"
#include "uds_transport/connection.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief    Interface for diag client conversation
 */
class Conversation {
 public:
  /**
   * @brief         Type alias for Ip address
   */
  using IpAddress = DiagClientConversation::IpAddress;

  /**
   * @brief         Type alias for Connection results
   */
  using ConnectResult = DiagClientConversation::ConnectResult;

  /**
   * @brief         Type alias for Disconnection results
   */
  using DisconnectResult = DiagClientConversation::DisconnectResult;

  /**
   * @brief         Type alias for Diagnostic results
   */
  using DiagError = DiagClientConversation::DiagError;

  /**
   * @brief         Type alias for DoIP protocol version
   */
  using DoIpProtocol = DiagClientConversation::DoIpProtocol;


  /**
   * @brief  Definitions of current activity status
   */
  enum class ActivityStatusType : uint8_t { kActive = 0x00, kInactive = 0x01 };

 public:
  /**
   * @brief         Constructs an instance of Conversation
   */
  Conversation() noexcept = default;

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  Conversation(const Conversation &other) noexcept = delete;
  Conversation &operator=(const Conversation &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  Conversation(Conversation &&other) noexcept = delete;
  Conversation &operator=(Conversation &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of Conversation
   */
  virtual ~Conversation() noexcept = default;

  /**
   * @brief         Function to start the Conversation
   */
  virtual void Startup() noexcept = 0;

  /**
   * @brief         Function to shutdown the Conversation
   */
  virtual void Shutdown() noexcept = 0;

  /**
   * @brief       Function to register the conversation to underlying transport protocol handler
   * @param[in]   connection
   *              The conversation connection object
   */
  virtual void RegisterConnection(
      std::unique_ptr<::uds_transport::Connection> connection) noexcept = 0;

  /**
   * @brief       Function to get the conversation handler from conversation object
   * @return      ConversionHandler &
   *              The reference to conversation handler
   */
  virtual ::uds_transport::ConversionHandler &GetConversationHandler() noexcept = 0;

  /**
   * @brief       Function to connect to Diagnostic Server
   * @param[in]   target_address
   *              Logical address of the Remote server
   * @param[in]   host_ip_addr
   *              IP address of the Remote server
   * @return      ConnectResult
   *              Connection result returned
   */
  virtual ConnectResult ConnectToDiagServer(std::uint16_t, IpAddress) noexcept {
    return ConnectResult::kConnectFailed;
  }

  /**
   * @brief       Function to disconnect from Diagnostic Server
   * @return      DisconnectResult
   *              Disconnection result returned
   */
  virtual DisconnectResult DisconnectFromDiagServer() noexcept {
    return DisconnectResult::kDisconnectFailed;
  }

  /**
   * @brief       Function to set DoIP protocol version for messages
   */
  virtual void SetDoIpProtocol(DoIpProtocol version) noexcept = 0;

  /**
   * @brief       Function to get used DoIP protocol version
   * @return      DoIpProtocol
   *              Currently used protool version
   */
  virtual DoIpProtocol GetDoIpProtocol() const noexcept = 0;

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
  virtual std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult,
                    ::uds_transport::UdsMessagePtr>
  IndicateMessage(::uds_transport::UdsMessage::Address source_addr,
                  ::uds_transport::UdsMessage::Address target_addr,
                  ::uds_transport::UdsMessage::TargetAddressType type,
                  ::uds_transport::ChannelID channel_id, std::size_t size,
                  ::uds_transport::Priority priority, ::uds_transport::ProtocolKind protocol_kind,
                  core_type::Span<std::uint8_t const> payload_info) noexcept = 0;

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  virtual void HandleMessage(::uds_transport::UdsMessagePtr message) noexcept = 0;

  /**
   * @brief       Function to send Diagnostic Request and get Diagnostic Response
   * @param[in]   message
   *              The diagnostic request message wrapped in a unique pointer
   * @return      DiagResult
   *              The Result returned
   * @return      uds_message::UdsResponseMessagePtr
   *              Diagnostic Response message received, null_ptr in case of error
   */
  virtual Result<uds_message::UdsResponseMessagePtr, DiagError> SendDiagnosticRequest(
      uds_message::UdsRequestMessageConstPtr) noexcept {
    return Result<uds_message::UdsResponseMessagePtr, DiagError>::FromError(
        DiagError::kDiagRequestSendFailed);
  }

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   */
  virtual core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                            DiagClient::VehicleInfoResponseError>
  SendVehicleIdentificationRequest(vehicle_info::VehicleInfoListRequestType) noexcept {
    return core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                             DiagClient::VehicleInfoResponseError>::
        FromError(DiagClient::VehicleInfoResponseError::kTransmitFailed);
  }

  /**
   * @brief       Get the current activity status of this conversation
   * @return      The activity status
   */
  auto GetActivityStatus() const noexcept -> ActivityStatusType { return activity_status_; }

 protected:
  /**
   * @brief       Store the conversation activity status
   */
  ActivityStatusType activity_status_{ActivityStatusType::kInactive};
};

}  // namespace conversation
}  // namespace client
}  // namespace diag

#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_H
