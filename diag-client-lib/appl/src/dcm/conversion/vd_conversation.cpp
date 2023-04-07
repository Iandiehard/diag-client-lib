/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "src/dcm/conversion/vd_conversation.h"

#include <sstream>
#include <string>
#include <utility>

#include "src/common/logger.h"
#include "src/dcm/service/vd_message.h"

namespace diag {
namespace client {
namespace conversation {

class VehicleInfoMessageImpl final : public vehicle_info::VehicleInfoMessage {
public:
  explicit VehicleInfoMessageImpl(
      std::map<std::uint16_t, vehicle_info::VehicleAddrInfoResponse> &vehicle_info_collection)
      : vehicle_info_messages_{} {
    for (auto &vehicle_info: vehicle_info_collection) { Push(vehicle_info.second); }
  }

  ~VehicleInfoMessageImpl() override = default;

  VehicleInfoListResponseType &GetVehicleList() override { return vehicle_info_messages_; }

private:
  // Function to push the vehicle address info received
  void Push(vehicle_info::VehicleAddrInfoResponse &vehicle_addr_info_response) {
    vehicle_info_messages_.emplace_back(vehicle_addr_info_response);
  }

  VehicleInfoListResponseType vehicle_info_messages_;
};

// Conversation class
VdConversation::VdConversation(std::string conversion_name,
                               ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier)
    : vd_conversion_handler_{std::make_shared<VdConversationHandler>(conversion_identifier.handler_id, *this)},
      conversation_name_{conversion_name},
      broadcast_address_{conversion_identifier.udp_broadcast_address},
      connection_ptr_{},
      sync_timer_{},
      vehicle_info_collection_{},
      vehicle_info_container_mutex_{} {}

void VdConversation::Startup() {
  // initialize the connection
  static_cast<void>(connection_ptr_->Initialize());
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

VdConversation::VehicleIdentificationResponseResult VdConversation::SendVehicleIdentificationRequest(
    vehicle_info::VehicleInfoListRequestType vehicle_info_request) {
  VehicleIdentificationResponseResult ret_val{VehicleResponseResult::kTransmitFailed, nullptr};

  if (VerifyVehicleInfoRequest(vehicle_info_request)) {
    if (connection_ptr_->Transmit(std::move(
            std::make_unique<diag::client::vd_message::VdMessage>(vehicle_info_request, broadcast_address_))) !=
        ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed) {
      // Check if any response received
      if (vehicle_info_collection_.empty()) {
        // no response received
        ret_val.first = VehicleResponseResult::kNoResponseReceived;
      } else {
        ret_val.first = VehicleResponseResult::kStatusOk;
        ret_val.second = std::move(std::make_unique<VehicleInfoMessageImpl>(vehicle_info_collection_));
        // all the responses are copied, now clear the map
        vehicle_info_collection_.clear();
      }
    }
  } else {
    ret_val.first = VehicleResponseResult::kInvalidParameters;
  }
  return ret_val;
}

vehicle_info::VehicleInfoMessageResponsePtr VdConversation::GetDiagnosticServerList() { return nullptr; }

std::pair<VdConversation::IndicationResult, ara::diag::uds_transport::UdsMessagePtr> VdConversation::IndicateMessage(
    ara::diag::uds_transport::UdsMessage::Address /* source_addr */,
    ara::diag::uds_transport::UdsMessage::Address /* target_addr */,
    ara::diag::uds_transport::UdsMessage::TargetAddressType /* type */, ara::diag::uds_transport::ChannelID channel_id,
    std::size_t size, ara::diag::uds_transport::Priority priority, ara::diag::uds_transport::ProtocolKind protocol_kind,
    std::vector<uint8_t> payloadInfo) {
  std::pair<IndicationResult, ara::diag::uds_transport::UdsMessagePtr> ret_val{IndicationResult::kIndicationNOk,
                                                                               nullptr};
  if (!payloadInfo.empty()) {
    ret_val.first = IndicationResult::kIndicationOk;
    ret_val.second = std::move(std::make_unique<diag::client::vd_message::VdMessage>());
    ret_val.second->GetPayload().resize(size);
  }
  return ret_val;
}

void VdConversation::HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) {
  if (message != nullptr) {
    std::lock_guard<std::mutex> const lock{vehicle_info_container_mutex_};
    std::pair<std::uint16_t, VehicleAddrInfoResponseStruct> vehicle_info_request{
        DeserializeVehicleInfoResponse(std::move(message))};

    vehicle_info_collection_.emplace(vehicle_info_request.first, vehicle_info_request.second);
  }
}

bool VdConversation::VerifyVehicleInfoRequest(vehicle_info::VehicleInfoListRequestType &vehicle_info_request) {
  return true;
}

std::pair<std::uint16_t, VdConversation::VehicleAddrInfoResponseStruct> VdConversation::DeserializeVehicleInfoResponse(
    ara::diag::uds_transport::UdsMessagePtr message) {
  // Copy to stream
  std::ostringstream vehicle_info_data_os;
  for (std::uint8_t &byte: message->GetPayload()) { vehicle_info_data_os << byte; }
  std::string vehicle_info_data{vehicle_info_data_os.str()};

  std::uint16_t logical_address{
      (static_cast<std::uint16_t>(((message->GetPayload()[17] & 0xFF) << 8) | (message->GetPayload()[18] & 0xFF)))};

  VehicleAddrInfoResponseStruct vehicle_addr_info{message->GetHostIpAddress(),         // remote ip address
                                                  logical_address,                     // logical address
                                                  vehicle_info_data.substr(0u, 17u),   // vin
                                                  vehicle_info_data.substr(19u, 6u),   // eid
                                                  vehicle_info_data.substr(25u, 6u)};  // gid

  return {logical_address, vehicle_addr_info};
}

std::shared_ptr<ara::diag::conversion::ConversionHandler> &VdConversation::GetConversationHandler() {
  return vd_conversion_handler_;
}

VdConversationHandler::VdConversationHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id,
                                             VdConversation &vd_conversion)
    : ara::diag::conversion::ConversionHandler{handler_id},
      vd_conversation_{vd_conversion} {}

std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>
VdConversationHandler::IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                                       ara::diag::uds_transport::UdsMessage::Address target_addr,
                                       ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                       ara::diag::uds_transport::ChannelID channel_id, std::size_t size,
                                       ara::diag::uds_transport::Priority priority,
                                       ara::diag::uds_transport::ProtocolKind protocol_kind,
                                       std::vector<uint8_t> payloadInfo) {
  return (vd_conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                           payloadInfo));
}

void VdConversationHandler::HandleMessage(ara::diag::uds_transport::UdsMessagePtr message) {
  vd_conversation_.HandleMessage(std::move(message));
}

}  // namespace conversation
}  // namespace client
}  // namespace diag
