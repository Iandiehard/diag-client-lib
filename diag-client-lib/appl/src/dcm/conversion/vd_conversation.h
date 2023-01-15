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
#include "ara/diag/uds_transport/connection.h"
#include "ara/diag/uds_transport/conversion_handler.h"
#include "ara/diag/uds_transport/protocol_types.h"
#include "common_Header.h"
#include "libTimer/oneShotSync/one_shotsync_timer.h"
#include "include/diagnostic_client_message_type.h"

namespace diag {
namespace client {
namespace conversation {

using SyncTimer = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer;
using SyncTimerState = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer::timer_state;

/*
 @ Class Name        : VdConversation
 @ Class Description : Class to query Diagnostic Server list
 */
class VdConversation {
public:
  // ctor
  VdConversation(
    std::string conversion_name,
    ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier);
  
  // dtor
  ~VdConversation() = default;
  
  // startup
  void Startup();
  
  // shutdown
  void Shutdown();
  
  // Register Connection
  void RegisterConnection(std::shared_ptr<ara::diag::connection::Connection> connection);
  
  // Send Vehicle Identification Request and get response
  vehicle_info::VehicleInfoMessageResponsePtr
  SendVehicleIdentificationRequest(
    vehicle_info::VehicleInfoListRequestType vehicle_info_request);
  
  // Get the list of available Diagnostic Server
  vehicle_info::VehicleInfoMessageResponsePtr
  GetDiagnosticServerList();
  
  // Indicate message Diagnostic message reception over TCP to user
  std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
    ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(
    ara::diag::uds_transport::UdsMessage::Address source_addr,
    ara::diag::uds_transport::UdsMessage::Address target_addr,
    ara::diag::uds_transport::UdsMessage::TargetAddressType type,
    ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size,
    ara::diag::uds_transport::Priority priority,
    ara::diag::uds_transport::ProtocolKind protocol_kind,
    std::vector<uint8_t> payloadInfo);
  
  // Hands over a valid message to conversion
  void HandleMessage(ara::diag::uds_transport::UdsMessagePtr message);
  
  // shared pointer to store the conversion handler
  std::shared_ptr<ara::diag::conversion::ConversionHandler> vd_conversion_handler;
private:
  // Function to wait for response
  void WaitForResponse(std::function<void()> timeout_func, std::function<void()> cancel_func, int msec);
  
  // Function to cancel the synchronous wait
  void WaitCancel();
  
  // conversation name
  std::string conversation_name_;
  
  // Tp connection
  std::shared_ptr<ara::diag::connection::Connection> connection_ptr_;
  
  // timer
  SyncTimer sync_timer_;
};

/*
 @ Class Name        : DmConversationHandler
 @ Class Description : Class to establish connection with Diagnostic Server
 */
class VdConversationHandler : public ara::diag::conversion::ConversionHandler {
public:
  // ctor
  VdConversationHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id,
                        VdConversation &dm_conversion);
  
  // dtor
  ~VdConversationHandler() = default;
  
  // Indicate message Diagnostic message reception over TCP to user
  std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
    ara::diag::uds_transport::UdsMessagePtr>
  IndicateMessage(
    ara::diag::uds_transport::UdsMessage::Address source_addr,
    ara::diag::uds_transport::UdsMessage::Address target_addr,
    ara::diag::uds_transport::UdsMessage::TargetAddressType type,
    ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size,
    ara::diag::uds_transport::Priority priority,
    ara::diag::uds_transport::ProtocolKind protocol_kind,
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
