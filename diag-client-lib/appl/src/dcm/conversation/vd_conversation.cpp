/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "src/dcm/conversation/vd_conversation.h"

#include <sstream>
#include <string>
#include <utility>

#include "src/common/logger.h"
#include "src/dcm/service/vd_message.h"

namespace diag {
namespace client {
namespace conversation {

std::string ConvertToHexString(std::uint8_t char_start, std::uint8_t char_count,
                               std::vector<std::uint8_t> &input_buffer) {
  std::string hex_string{};
  std::uint8_t total_char_count{static_cast<uint8_t>(char_start + char_count)};

  for (std::uint8_t char_start_count = char_start; char_start_count < total_char_count; char_start_count++) {
    std::stringstream vehicle_info_data_eid{};
    int payload_byte{input_buffer[char_start_count]};
    if ((payload_byte <= 15U)) {
      // "0" appended in case of value upto 15/0xF
      vehicle_info_data_eid << "0";
    }
    vehicle_info_data_eid << std::hex << payload_byte << ":";
    hex_string.append(vehicle_info_data_eid.str());
  }
  hex_string.pop_back();  // remove last ":" appended before
  return hex_string;
}

std::string ConvertToAsciiString(std::uint8_t char_start, std::uint8_t char_count,
                                 std::vector<std::uint8_t> &input_buffer) {
  std::string ascii_string{};
  std::uint8_t total_char_count{static_cast<uint8_t>(char_start + char_count)};

  for (std::uint8_t char_start_count = char_start; char_start_count < total_char_count; char_start_count++) {
    std::stringstream vehicle_info_data_vin{};
    vehicle_info_data_vin << input_buffer[char_start_count];
    ascii_string.append(vehicle_info_data_vin.str());
  }
  return ascii_string;
}

void SerializeEIDGIDFromString(std::string &input_string, std::vector<uint8_t> &output_buffer, std::uint8_t total_size,
                               std::uint8_t substring_range) {

  for (auto char_count = 0U; char_count < total_size; char_count += substring_range) {
    std::string input_string_new{input_string.substr(char_count, static_cast<std::uint8_t>(substring_range))};
    std::stringstream input_string_stream{input_string_new};
    int get_byte;
    input_string_stream >> std::hex >> get_byte;
    output_buffer.emplace_back(static_cast<std::uint8_t>(get_byte));
  }
}

void SerializeVINFromString(std::string &input_string, std::vector<uint8_t> &output_buffer, std::uint8_t total_size,
                            std::uint8_t substring_range) {

  for (auto char_count = 0U; char_count < total_size; char_count += substring_range) {
    std::string input_string_new{input_string.substr(char_count, static_cast<std::uint8_t>(substring_range))};
    std::stringstream input_string_stream{input_string_new};
    int get_byte{input_string_stream.get()};
    output_buffer.emplace_back(static_cast<std::uint8_t>(get_byte));
  }
}

// Vehicle Info Message implementation class
class VehicleInfoMessageImpl final : public vehicle_info::VehicleInfoMessage {
public:
  explicit VehicleInfoMessageImpl(
      std::map<std::uint16_t, vehicle_info::VehicleAddrInfoResponse> &vehicle_info_collection)
      : vehicle_info_messages_{} {
    for (std::pair<std::uint16_t, vehicle_info::VehicleAddrInfoResponse> vehicle_info: vehicle_info_collection) {
      Push(vehicle_info.second);
    }
  }

  ~VehicleInfoMessageImpl() override = default;

  VehicleInfoListResponseType &GetVehicleList() override { return vehicle_info_messages_; }

private:
  // Function to push the vehicle address info received
  void Push(vehicle_info::VehicleAddrInfoResponse &vehicle_addr_info_response) {
    vehicle_info_messages_.emplace_back(vehicle_addr_info_response);
  }

  // store the vehicle info message list
  VehicleInfoListResponseType vehicle_info_messages_;
};

// Conversation class
VdConversation::VdConversation(std::string_view conversion_name, VDConversationType &conversion_identifier)
    : vd_conversion_handler_{std::make_shared<VdConversationHandler>(conversion_identifier.handler_id, *this)},
      conversation_name_{conversion_name},
      broadcast_address_{conversion_identifier.udp_broadcast_address},
      connection_ptr_{},
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

void VdConversation::RegisterConnection(std::shared_ptr<uds_transport::Connection> connection) {
  connection_ptr_ = std::move(connection);
}

VdConversation::VehicleIdentificationResponseResult VdConversation::SendVehicleIdentificationRequest(
    vehicle_info::VehicleInfoListRequestType vehicle_info_request) {
  VehicleIdentificationResponseResult result{
      VehicleIdentificationResponseResult::FromError(DiagClient::VehicleInfoResponseErrorCode::kTransmitFailed)};
  // Deserialize first , Todo: Add optional when deserialize fails
  std::pair<PreselectionMode, PreselectionValue> vehicle_info_request_deserialized_value{
      DeserializeVehicleInfoRequest(vehicle_info_request)};

  if (VerifyVehicleInfoRequest(vehicle_info_request_deserialized_value.first,
                               vehicle_info_request_deserialized_value.second.size())) {
    if (connection_ptr_->Transmit(std::move(std::make_unique<diag::client::vd_message::VdMessage>(
            vehicle_info_request_deserialized_value.first, vehicle_info_request_deserialized_value.second,
            broadcast_address_))) != uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed) {
      // Check if any response received
      if (vehicle_info_collection_.empty()) {
        // no response received
        result.EmplaceError(DiagClient::VehicleInfoResponseErrorCode::kNoResponseReceived);
      } else {
        result.EmplaceValue(std::move(std::make_unique<VehicleInfoMessageImpl>(vehicle_info_collection_)));
        // all the responses are copied, now clear the map
        vehicle_info_collection_.clear();
      }
    }
  } else {
    result.EmplaceError(DiagClient::VehicleInfoResponseErrorCode::kInvalidParameters);
  }
  return result;
}

vehicle_info::VehicleInfoMessageResponseUniquePtr VdConversation::GetDiagnosticServerList() { return nullptr; }

std::pair<VdConversation::IndicationResult, uds_transport::UdsMessagePtr> VdConversation::IndicateMessage(
    uds_transport::UdsMessage::Address /* source_addr */, uds_transport::UdsMessage::Address /* target_addr */,
    uds_transport::UdsMessage::TargetAddressType /* type */, uds_transport::ChannelID channel_id, std::size_t size,
    uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind, std::vector<uint8_t> payloadInfo) {
  std::pair<IndicationResult, uds_transport::UdsMessagePtr> ret_val{IndicationResult::kIndicationNOk, nullptr};
  if (!payloadInfo.empty()) {
    ret_val.first = IndicationResult::kIndicationOk;
    ret_val.second = std::move(std::make_unique<diag::client::vd_message::VdMessage>());
    ret_val.second->GetPayload().resize(size);
  }
  return ret_val;
}

void VdConversation::HandleMessage(uds_transport::UdsMessagePtr message) {
  if (message != nullptr) {
    std::lock_guard<std::mutex> const lock{vehicle_info_container_mutex_};
    std::pair<std::uint16_t, VehicleAddrInfoResponseStruct> vehicle_info_request{
        DeserializeVehicleInfoResponse(std::move(message))};

    vehicle_info_collection_.emplace(vehicle_info_request.first, vehicle_info_request.second);
  }
}

bool VdConversation::VerifyVehicleInfoRequest(PreselectionMode preselection_mode,
                                              std::uint8_t preselection_value_length) {
  bool is_veh_info_valid{false};
  if ((preselection_mode != 0U) && (preselection_value_length != 0U)) {
    // 1U : DoIP Entities with given VIN
    if (preselection_mode == 1U && (preselection_value_length == 17U)) {
      is_veh_info_valid = true;
    }
    // 2U : DoIP Entities with given EID
    else if (preselection_mode == 2U && (preselection_value_length == 6U)) {
      is_veh_info_valid = true;
    } else {
    }
  }
  // 0U : No preselection
  else if (preselection_mode == 0U && (preselection_value_length == 0U)) {
    is_veh_info_valid = true;
  } else {
  }

  return is_veh_info_valid;
}

std::pair<std::uint16_t, VdConversation::VehicleAddrInfoResponseStruct> VdConversation::DeserializeVehicleInfoResponse(
    uds_transport::UdsMessagePtr message) {
  constexpr std::uint8_t start_index_vin{0U};
  constexpr std::uint8_t total_vin_length{17U};
  constexpr std::uint8_t start_index_eid{19U};
  constexpr std::uint8_t start_index_gid{25U};
  constexpr std::uint8_t total_eid_gid_length{6U};

  std::string vehicle_info_data_vin{ConvertToAsciiString(start_index_vin, total_vin_length, message->GetPayload())};
  std::string vehicle_info_data_eid{ConvertToHexString(start_index_eid, total_eid_gid_length, message->GetPayload())};
  std::string vehicle_info_data_gid{ConvertToHexString(start_index_gid, total_eid_gid_length, message->GetPayload())};

  std::uint16_t logical_address{
      (static_cast<std::uint16_t>(((message->GetPayload()[17U] & 0xFF) << 8) | (message->GetPayload()[18U] & 0xFF)))};

  // Create the structure out of the extracted string
  VehicleAddrInfoResponseStruct vehicle_addr_info{std::string{message->GetHostIpAddress()},  // remote ip address
                                                  logical_address,                           // logical address
                                                  vehicle_info_data_vin,                     // vin
                                                  vehicle_info_data_eid,                     // eid
                                                  vehicle_info_data_gid};                    // gid

  return {logical_address, vehicle_addr_info};
}

std::shared_ptr<uds_transport::ConversionHandler> &VdConversation::GetConversationHandler() {
  return vd_conversion_handler_;
}

std::pair<VdConversation::PreselectionMode, VdConversation::PreselectionValue>
VdConversation::DeserializeVehicleInfoRequest(vehicle_info::VehicleInfoListRequestType &vehicle_info_request) {

  std::pair<VdConversation::PreselectionMode, VdConversation::PreselectionValue> ret_val{};
  ret_val.first = vehicle_info_request.preselection_mode;

  if (ret_val.first == 1U) {
    // 1U : DoIP Entities with given VIN
    SerializeVINFromString(vehicle_info_request.preselection_value, ret_val.second,
                           vehicle_info_request.preselection_value.length(), 1U);
  } else if (ret_val.first == 2U) {
    // 2U : DoIP Entities with given EID
    vehicle_info_request.preselection_value.erase(
        remove(vehicle_info_request.preselection_value.begin(), vehicle_info_request.preselection_value.end(), ':'),
        vehicle_info_request.preselection_value.end());
    SerializeEIDGIDFromString(vehicle_info_request.preselection_value, ret_val.second,
                              vehicle_info_request.preselection_value.length(), 2U);
  }
  return ret_val;
}

VdConversationHandler::VdConversationHandler(uds_transport::conversion_manager::ConversionHandlerID handler_id,
                                             VdConversation &vd_conversion)
    : uds_transport::ConversionHandler{handler_id},
      vd_conversation_{vd_conversion} {}

std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr>
VdConversationHandler::IndicateMessage(uds_transport::UdsMessage::Address source_addr,
                                       uds_transport::UdsMessage::Address target_addr,
                                       uds_transport::UdsMessage::TargetAddressType type,
                                       uds_transport::ChannelID channel_id, std::size_t size,
                                       uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
                                       std::vector<uint8_t> payloadInfo) {
  return (vd_conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                           payloadInfo));
}

void VdConversationHandler::HandleMessage(uds_transport::UdsMessagePtr message) {
  vd_conversation_.HandleMessage(std::move(message));
}

}  // namespace conversation
}  // namespace client
}  // namespace diag
