/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include <utility>
#include "src/dcm/conversion/vd_conversation.h"
#include "src/common/logger.h"
#include "src/dcm/service/vd_message.h"

namespace diag {
namespace client {
namespace conversation {


VdConversation::VdConversation(std::string conversion_name,
                               ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier)
: conversation_name_{conversion_name},
  broadcast_address_{conversion_identifier.udp_broadcast_address} {
}

void VdConversation::Startup() {
  // initialize the connection
  connection_ptr_->Initialize();
  // start the connection
  connection_ptr_->Start();
}

void VdConversation::Shutdown() {
  // shutdown connection
  connection_ptr_->Stop();
}

void VdConversation::RegisterConnection(std::shared_ptr<ara::diag::connection::Connection> connection) {
  connection_ptr_ = std::move(connection);
}

vehicle_info::VehicleInfoMessageResponsePtr
VdConversation::SendVehicleIdentificationRequest(vehicle_info::VehicleInfoListRequestType vehicle_info_request) {
  vehicle_info::VehicleInfoMessageResponsePtr response{};

  if(connection_ptr_->Transmit(std::move(
        std::make_unique<diag::client::vd_message::VdMessage>(vehicle_info_request, broadcast_address_)
        )) != ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed) {
    // Vehicle Identification Request Sent & response received

    // Collect the response and create response

  }

  return response;
}

vehicle_info::VehicleInfoMessageResponsePtr VdConversation::GetDiagnosticServerList() {
  return diag::client::vehicle_info::VehicleInfoMessageResponsePtr();
}

std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
  ara::diag::uds_transport::UdsMessagePtr>
VdConversation::IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                                ara::diag::uds_transport::UdsMessage::Address target_addr,
                                ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                ara::diag::uds_transport::ChannelID channel_id, std::size_t size,
                                ara::diag::uds_transport::Priority priority,
                                ara::diag::uds_transport::ProtocolKind protocol_kind,
                                std::vector<uint8_t> payloadInfo) {
  return std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>();
}

void VdConversation::HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) {

}

void VdConversation::WaitForResponse(
  std::function<void()> timeout_func,
  std::function<void()> cancel_func,
  int msec) {

}

void VdConversation::WaitCancel() {

}

VdConversationHandler::VdConversationHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id,
                                             VdConversation &vd_conversion)
  : ara::diag::conversion::ConversionHandler{handler_id},
    vd_conversation_{vd_conversion} {}

std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult,
  ara::diag::uds_transport::UdsMessagePtr>
VdConversationHandler::IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                                       ara::diag::uds_transport::UdsMessage::Address target_addr,
                                       ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                       ara::diag::uds_transport::ChannelID channel_id, std::size_t size,
                                       ara::diag::uds_transport::Priority priority,
                                       ara::diag::uds_transport::ProtocolKind protocol_kind,
                                       std::vector<uint8_t> payloadInfo) {
  return (vd_conversation_.IndicateMessage(
    source_addr,
    target_addr,
    type,
    channel_id,
    size,
    priority,
    protocol_kind,
    payloadInfo));
}

void VdConversationHandler::HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) {
  vd_conversation_.HandleMessage(std::move(message));
}

}  // namespace conversation
}  // namespace client
}  // namespace diag