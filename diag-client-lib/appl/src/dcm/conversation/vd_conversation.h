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

#include "core/result.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_uds_message_type.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"
#include "src/dcm/conversation/vd_conversation_type.h"
#include "uds_transport/connection.h"
#include "uds_transport/conversion_handler.h"
#include "uds_transport/protocol_types.h"

namespace diag {
namespace client {
namespace conversation {

/**
 * @brief       Class to search for available diagnostic server over a network
 */
class VdConversation {
public:
  /**
   * @brief         Type alias for vehicle identification response result
   */
  using VehicleIdentificationResponseResult =
      core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                        DiagClient::VehicleInfoResponseErrorCode>;

  using IndicationResult = ::uds_transport::UdsTransportProtocolMgr::IndicationResult;

private:
  using PreselectionMode = std::uint8_t;
  using PreselectionValue = std::vector<std::uint8_t>;
  using VehicleAddrInfoResponseStruct = diag::client::vehicle_info::VehicleAddrInfoResponse;

public:
  // ctor
  VdConversation(std::string_view conversion_name, VDConversationType &conversion_identifier);

  // dtor
  ~VdConversation() = default;

  // startup
  void Startup();

  // shutdown
  void Shutdown();

  // Register Connection
  void RegisterConnection(std::shared_ptr<::uds_transport::Connection> connection);

  // Send Vehicle Identification Request and get response
  VehicleIdentificationResponseResult SendVehicleIdentificationRequest(
      vehicle_info::VehicleInfoListRequestType vehicle_info_request);

  // Get the list of available Diagnostic Server
  vehicle_info::VehicleInfoMessageResponseUniquePtr GetDiagnosticServerList();

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<IndicationResult, ::uds_transport::UdsMessagePtr> IndicateMessage(
      ::uds_transport::UdsMessage::Address source_addr, ::uds_transport::UdsMessage::Address target_addr,
      ::uds_transport::UdsMessage::TargetAddressType type, ::uds_transport::ChannelID channel_id, std::size_t size,
      ::uds_transport::Priority priority, ::uds_transport::ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo);

  // Hands over a valid message to conversion
  void HandleMessage(::uds_transport::UdsMessagePtr message);

  // Get Conversation Handlers
  std::shared_ptr<::uds_transport::ConversionHandler> &GetConversationHandler();

private:
  // Function to verify Vehicle Info requests
  bool VerifyVehicleInfoRequest(PreselectionMode preselection_mode, std::uint8_t preselection_value_length);

  // Function to deserialize the received Vehicle Identification Response/ Announcement
  static std::pair<std::uint16_t, VehicleAddrInfoResponseStruct> DeserializeVehicleInfoResponse(
      ::uds_transport::UdsMessagePtr message);

  static std::pair<PreselectionMode, PreselectionValue> DeserializeVehicleInfoRequest(
      vehicle_info::VehicleInfoListRequestType &vehicle_info_request);

  // shared pointer to store the conversion handler
  std::shared_ptr<::uds_transport::ConversionHandler> vd_conversion_handler_;

  // conversation name
  std::string conversation_name_;

  // Vehicle broadcast address
  std::string broadcast_address_;

  // Tp connection
  std::shared_ptr<::uds_transport::Connection> connection_ptr_;

  // container to store the vehicle information
  std::map<std::uint16_t, VehicleAddrInfoResponseStruct> vehicle_info_collection_;

  // mutex to lock the vehicle info collection container
  std::mutex vehicle_info_container_mutex_;
};

/*
 @ Class Name        : DmConversationHandler
 @ Class Description : Class to establish connection with Diagnostic Server
 */
class VdConversationHandler : public ::uds_transport::ConversionHandler {
public:
  // ctor
  VdConversationHandler(::uds_transport::conversion_manager::ConversionHandlerID handler_id,
                        VdConversation &dm_conversion);

  // dtor
  ~VdConversationHandler() = default;

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult, ::uds_transport::UdsMessagePtr> IndicateMessage(
      ::uds_transport::UdsMessage::Address source_addr, ::uds_transport::UdsMessage::Address target_addr,
      ::uds_transport::UdsMessage::TargetAddressType type, ::uds_transport::ChannelID channel_id, std::size_t size,
      ::uds_transport::Priority priority, ::uds_transport::ProtocolKind protocol_kind,
      std::vector<uint8_t> payloadInfo) override;

  // Hands over a valid message to conversion
  void HandleMessage(::uds_transport::UdsMessagePtr message) override;

private:
  VdConversation &vd_conversation_;
};

}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H
