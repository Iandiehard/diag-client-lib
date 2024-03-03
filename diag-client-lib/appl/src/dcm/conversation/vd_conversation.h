/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H
#define DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H

/* includes */
#include <chrono>
#include <mutex>
#include <string_view>

#include "core/include/result.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_uds_message_type.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"
#include "src/dcm/conversation/conversation.h"
#include "src/dcm/conversation/vd_conversation_type.h"
#include "uds_transport/connection.h"
#include "uds_transport/conversation_handler.h"
#include "uds_transport/protocol_types.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief       Class to search for available diagnostic server over a network
 */
class VdConversation final : public Conversation {
 private:
  /**
   * @brief         Type alias of pre-selection mode
   */
  using PreselectionMode = std::uint8_t;

  /**
   * @brief         Type alias of pre-selection value in terms of vector
   */
  using PreselectionValue = std::vector<std::uint8_t>;

  /**
   * @brief         Type alias of vehicle address info response
   */
  using VehicleAddrInfoResponseStruct = diag::client::vehicle_info::VehicleAddrInfoResponse;

  /**
   * @brief         Type alias of logical address
   */
  using LogicalAddress = std::uint16_t;

 public:
  /**
   * @brief         Constructs an instance of VdConversation
   * @param[in]     conversion_name
   *                The name of conversation
   * @param[in]     conversion_identifier
   *                The identifier consisting of conversation settings
   */
  VdConversation(std::string_view conversion_name, VDConversationType &conversion_identifier);

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  VdConversation(const VdConversation &other) noexcept = delete;
  VdConversation &operator=(const VdConversation &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  VdConversation(VdConversation &&other) noexcept = delete;
  VdConversation &operator=(VdConversation &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of DmConversation
   */
  ~VdConversation() override;

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
  void RegisterConnection(std::unique_ptr<::uds_transport::Connection> connection) noexcept override;

  /**
   * @brief       Function to get the conversation handler from conversation object
   * @return      ConversionHandler &
   *              The reference to conversation handler
   */
  ::uds_transport::ConversionHandler &GetConversationHandler() noexcept override;

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
      core_type::Span<std::uint8_t const> payloadInfo) noexcept override;

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(::uds_transport::UdsMessagePtr message) noexcept override;

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   */
  core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                    DiagClient::VehicleInfoResponseError>
  SendVehicleIdentificationRequest(vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept override;

  /**
   * @brief       Function to get the list of available diagnostic server
   * @return      The Vehicle info message containing available diagnostic server information
   */
  vehicle_info::VehicleInfoMessageResponseUniquePtr GetDiagnosticServerList();

 private:
  /**
   * @brief       Function to verify the received vehicle info request
   * @param[in]   preselection_mode
   *              The preselection mode
   * @param[in]   preselection_value_length
   *              The length of preselection value
   * @return      True on success, else false
   */
  bool VerifyVehicleInfoRequest(PreselectionMode preselection_mode, std::uint8_t preselection_value_length);

  /**
   * @brief       Function to deserialize the received Vehicle Identification Response/ Announcement
   * @param[in]   message
   *              The message to deserialize
   * @return      The pair with logical address of server and Vehicle address information
   */
  static std::pair<LogicalAddress, VehicleAddrInfoResponseStruct> DeserializeVehicleInfoResponse(
      ::uds_transport::UdsMessagePtr message);

  /**
   * @brief       Function to deserialize the Vehicle Information request from user
   * @param[in]   vehicle_info_request
   *              The vehicle info request
   * @return      The pair with preselection mode along with its preselection value
   */
  static std::pair<PreselectionMode, PreselectionValue> DeserializeVehicleInfoRequest(
      vehicle_info::VehicleInfoListRequestType &vehicle_info_request);

  /**
   * @brief       Store the vd conversation handler
   */
  std::unique_ptr<::uds_transport::ConversionHandler> vd_conversion_handler_;

  /**
   * @brief       Store the conversation name
   */
  std::string conversation_name_;

  /**
   * @brief       Store the broadcast Ip address of the conversation
   */
  std::string broadcast_address_;

  /**
   * @brief       Store the underlying transport protocol connection object
   */
  std::unique_ptr<::uds_transport::Connection> connection_ptr_;

  /**
   * @brief       Store the vehicle info collection received till now
   */
  std::map<std::uint16_t, VehicleAddrInfoResponseStruct> vehicle_info_collection_;

  /**
   * @brief       Mutex to lock the vehicle info collection container
   */
  std::mutex vehicle_info_container_mutex_;
};

}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H
