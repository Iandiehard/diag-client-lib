/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H
#define DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H

/* includes */
#include <string_view>

#include "ara/diag/uds_transport/connection.h"
#include "ara/diag/uds_transport/conversion_handler.h"
#include "ara/diag/uds_transport/protocol_types.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_uds_message_type.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"
#include "libTimer/oneShotSync/one_shotsync_timer.h"

namespace diag {
namespace client {
namespace conversation {

using SyncTimer = libBoost::libTimer::oneShot::oneShotSyncTimer;
using SyncTimerState = libBoost::libTimer::oneShot::oneShotSyncTimer::timer_state;

/*
 @ Class Name        : VdConversation
 @ Class Description : Class to query Diagnostic Server list
 */
class VdConversation {
public:
  using VehicleIdentificationResponseResult =
      std::pair<diag::client::DiagClient::VehicleResponseResult,
                diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr>;

  using IndicationResult = ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult;

private:
  using PreselectionMode = std::uint8_t;
  using PreselectionValue = std::vector<std::uint8_t>;
  using VehicleResponseResult = diag::client::DiagClient::VehicleResponseResult;
  using VehicleAddrInfoResponseStruct = diag::client::vehicle_info::VehicleAddrInfoResponse;

public:
  // ctor
  VdConversation(std::string_view conversion_name,
                 ara::diag::conversion_manager::ConversionIdentifierType &conversion_identifier);

  // dtor
  ~VdConversation() = default;

  // startup
  void Startup();

  // shutdown
  void Shutdown();

  // Register Connection
  void RegisterConnection(std::shared_ptr<ara::diag::connection::Connection> connection);

  // Send Vehicle Identification Request and get response
  VehicleIdentificationResponseResult SendVehicleIdentificationRequest(
      vehicle_info::VehicleInfoListRequestType vehicle_info_request);

  // Get the list of available Diagnostic Server
  vehicle_info::VehicleInfoMessageResponseUniquePtr GetDiagnosticServerList();

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<IndicationResult, ara::diag::uds_transport::UdsMessagePtr> IndicateMessage(
      ara::diag::uds_transport::UdsMessage::Address source_addr,
      ara::diag::uds_transport::UdsMessage::Address target_addr,
      ara::diag::uds_transport::UdsMessage::TargetAddressType type, ara::diag::uds_transport::ChannelID channel_id,
      std::size_t size, ara::diag::uds_transport::Priority priority,
      ara::diag::uds_transport::ProtocolKind protocol_kind, std::vector<uint8_t> payloadInfo);

  // Hands over a valid message to conversion
  void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message);

  // Get Conversation Handlers
  std::shared_ptr<ara::diag::conversion::ConversionHandler> &GetConversationHandler();

private:
  // Function to verify Vehicle Info requests
  bool VerifyVehicleInfoRequest(PreselectionMode preselection_mode, std::uint8_t preselection_value_length);

  // Function to deserialize the received Vehicle Identification Response/ Announcement
  static std::pair<std::uint16_t, VehicleAddrInfoResponseStruct> DeserializeVehicleInfoResponse(
      ara::diag::uds_transport::UdsMessagePtr message);

  static std::pair<PreselectionMode, PreselectionValue> DeserializeVehicleInfoRequest(
      vehicle_info::VehicleInfoListRequestType &vehicle_info_request);

  // shared pointer to store the conversion handler
  std::shared_ptr<ara::diag::conversion::ConversionHandler> vd_conversion_handler_;

  // conversation name
  std::string conversation_name_;

  // Vehicle broadcast address
  std::string broadcast_address_;

  // Tp connection
  std::shared_ptr<ara::diag::connection::Connection> connection_ptr_;

  // sync timer
  SyncTimer sync_timer_;

  // container to store the vehicle information
  std::map<std::uint16_t, VehicleAddrInfoResponseStruct> vehicle_info_collection_;

  // mutex to lock the vehicle info collection container
  std::mutex vehicle_info_container_mutex_;
};

/*
 @ Class Name        : DmConversationHandler
 @ Class Description : Class to establish connection with Diagnostic Server
 */
class VdConversationHandler : public ara::diag::conversion::ConversionHandler {
public:
  // ctor
  VdConversationHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id, VdConversation &dm_conversion);

  // dtor
  ~VdConversationHandler() = default;

  // Indicate message Diagnostic message reception over TCP to user
  std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
            ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                  ara::diag::uds_transport::UdsMessage::Address target_addr,
                  ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                  ara::diag::uds_transport::ChannelID channel_id, std::size_t size,
                  ara::diag::uds_transport::Priority priority, ara::diag::uds_transport::ProtocolKind protocol_kind,
                  std::vector<uint8_t> payloadInfo) override;

  // Hands over a valid message to conversion
  void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) override;

private:
  VdConversation &vd_conversation_;
};

}  // namespace conversation
}  // namespace client
}  // namespace diag
#endif  // DIAG_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_VD_CONVERSATION_H
