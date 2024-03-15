/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
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
    if ((payload_byte <= 15)) {
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

/**
 * @brief    Class to manage reception from transport protocol handler to vd connection handler
 */
class VdConversationHandler final : public ::uds_transport::ConversionHandler {
 public:
  /**
   * @brief         Constructs an instance of VdConversationHandler
   * @param[in]     handler_id
   *                The handle id of conversation
   * @param[in]     vd_conversion
   *                The reference of vd conversation
   */
  VdConversationHandler(::uds_transport::conversion_manager::ConversionHandlerID handler_id,
                        VdConversation &vd_conversion)
      : ::uds_transport::ConversionHandler{handler_id},
        vd_conversation_{vd_conversion} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  VdConversationHandler(const VdConversationHandler &other) noexcept = delete;
  VdConversationHandler &operator=(const VdConversationHandler &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  VdConversationHandler(VdConversationHandler &&other) noexcept = delete;
  VdConversationHandler &operator=(VdConversationHandler &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of DmConversationHandler
   */
  ~VdConversationHandler() override = default;

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
      core_type::Span<std::uint8_t const> payloadInfo) const noexcept override {
    return (vd_conversation_.IndicateMessage(source_addr, target_addr, type, channel_id, size, priority, protocol_kind,
                                             payloadInfo));
  }

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(::uds_transport::UdsMessagePtr message) const noexcept override {
    vd_conversation_.HandleMessage(std::move(message));
  }

 private:
  /**
   * @brief         Store the reference of vd conversation
   */
  VdConversation &vd_conversation_;
};

// Conversation class
VdConversation::VdConversation(std::string_view conversion_name, VDConversationType &conversion_identifier)
    : vd_conversion_handler_{std::make_unique<VdConversationHandler>(conversion_identifier.handler_id, *this)},
      conversation_name_{conversion_name},
      broadcast_address_{conversion_identifier.udp_broadcast_address},
      connection_ptr_{},
      vehicle_info_collection_{},
      vehicle_info_container_mutex_{} {}

VdConversation::~VdConversation() = default;

void VdConversation::Startup() noexcept {
  // initialize the connection
  static_cast<void>(connection_ptr_->Initialize());
  // start the connection
  connection_ptr_->Start();
  // Change the state to Active
  activity_status_ = ActivityStatusType::kActive;
}

void VdConversation::Shutdown() noexcept {
  if (GetActivityStatus() == ActivityStatusType::kActive) {
    // shutdown connection
    connection_ptr_->Stop();
    // Change the state to InActive
    activity_status_ = ActivityStatusType::kInactive;
  }
}

void VdConversation::RegisterConnection(std::unique_ptr<uds_transport::Connection> connection) noexcept {
  connection_ptr_ = std::move(connection);
}

core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr, DiagClient::VehicleInfoResponseError>
VdConversation::SendVehicleIdentificationRequest(
    vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept {
  using VehicleIdentificationResponseResult =
      core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                        DiagClient::VehicleInfoResponseError>;

  VehicleIdentificationResponseResult result{
      VehicleIdentificationResponseResult::FromError(DiagClient::VehicleInfoResponseError::kTransmitFailed)};

  // Deserialize first , Todo: Add optional when deserialize fails
  std::pair<PreselectionMode, PreselectionValue> vehicle_info_request_deserialized_value{
      DeserializeVehicleInfoRequest(vehicle_info_request)};

  if (VerifyVehicleInfoRequest(vehicle_info_request_deserialized_value.first,
                               static_cast<uint8_t>(vehicle_info_request_deserialized_value.second.size()))) {
    if (connection_ptr_->Transmit(std::make_unique<diag::client::vd_message::VdMessage>(
            vehicle_info_request_deserialized_value.first, vehicle_info_request_deserialized_value.second,
            broadcast_address_)) != uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed) {
      // Check if any response received
      if (vehicle_info_collection_.empty()) {
        // no response received
        result.EmplaceError(DiagClient::VehicleInfoResponseError::kNoResponseReceived);
        logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogWarn(
            __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
              msg << "'" << conversation_name_ << "'"
                  << "-> "
                  << "No vehicle identification response received, timed out "
                     "without response";
            });
      } else {
        result.EmplaceValue(std::make_unique<VehicleInfoMessageImpl>(vehicle_info_collection_));
        // all the responses are copied, now clear the map
        vehicle_info_collection_.clear();
      }
    }
  } else {
    result.EmplaceError(DiagClient::VehicleInfoResponseError::kInvalidParameters);
  }
  return result;
}

vehicle_info::VehicleInfoMessageResponseUniquePtr VdConversation::GetDiagnosticServerList() { return nullptr; }

std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult, ::uds_transport::UdsMessagePtr>
VdConversation::IndicateMessage(uds_transport::UdsMessage::Address /* source_addr */,
                                uds_transport::UdsMessage::Address /* target_addr */,
                                uds_transport::UdsMessage::TargetAddressType /* type */, uds_transport::ChannelID,
                                std::size_t size, uds_transport::Priority, uds_transport::ProtocolKind,
                                core_type::Span<std::uint8_t const> payload_info) noexcept {
  using IndicationResult =
      std::pair<::uds_transport::UdsTransportProtocolMgr::IndicationResult, ::uds_transport::UdsMessagePtr>;
  IndicationResult ret_val{::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationNOk, nullptr};
  if (!payload_info.empty()) {
    ret_val.first = ::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk;
    ret_val.second = std::make_unique<diag::client::vd_message::VdMessage>();
    ret_val.second->GetPayload().resize(size);
  }
  return ret_val;
}

void VdConversation::HandleMessage(uds_transport::UdsMessagePtr message) noexcept {
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
  // 0U : No preselection
  if (preselection_mode == 0U && (preselection_value_length == 0U)) {
    is_veh_info_valid = true;
  }
  // 1U : DoIP Entities with given VIN
  else if (preselection_mode == 1U && (preselection_value_length == 17U)) {
    is_veh_info_valid = true;
  }
  // 2U : DoIP Entities with given EID
  else if (preselection_mode == 2U && (preselection_value_length == 6U)) {
    is_veh_info_valid = true;
  } else {
    // do nothing
  }

  return is_veh_info_valid;
}

std::pair<VdConversation::LogicalAddress, VdConversation::VehicleAddrInfoResponseStruct>
VdConversation::DeserializeVehicleInfoResponse(uds_transport::UdsMessagePtr message) {
  constexpr std::uint8_t start_index_vin{0U};
  constexpr std::uint8_t total_vin_length{17U};
  constexpr std::uint8_t start_index_eid{19U};
  constexpr std::uint8_t start_index_gid{25U};
  constexpr std::uint8_t total_eid_gid_length{6U};

  std::string const vehicle_info_data_vin{
      ConvertToAsciiString(start_index_vin, total_vin_length, message->GetPayload())};
  std::string const vehicle_info_data_eid{
      ConvertToHexString(start_index_eid, total_eid_gid_length, message->GetPayload())};
  std::string const vehicle_info_data_gid{
      ConvertToHexString(start_index_gid, total_eid_gid_length, message->GetPayload())};

  LogicalAddress const logical_address{
      (static_cast<std::uint16_t>(((message->GetPayload()[17U] & 0xFF) << 8) | (message->GetPayload()[18U] & 0xFF)))};

  // Create the structure out of the extracted string
  VehicleAddrInfoResponseStruct const vehicle_addr_info{std::string{message->GetHostIpAddress()},  // remote ip address
                                                        logical_address,                           // logical address
                                                        vehicle_info_data_vin,                     // vin
                                                        vehicle_info_data_eid,                     // eid
                                                        vehicle_info_data_gid};                    // gid

  return std::pair<std::uint16_t, VdConversation::VehicleAddrInfoResponseStruct>{logical_address, vehicle_addr_info};
}

::uds_transport::ConversionHandler &VdConversation::GetConversationHandler() noexcept {
  return *vd_conversion_handler_;
}

std::pair<VdConversation::PreselectionMode, VdConversation::PreselectionValue>
VdConversation::DeserializeVehicleInfoRequest(vehicle_info::VehicleInfoListRequestType &vehicle_info_request) {

  std::pair<VdConversation::PreselectionMode, VdConversation::PreselectionValue> ret_val{};
  ret_val.first = vehicle_info_request.preselection_mode;

  if (ret_val.first == 1U) {
    // 1U : DoIP Entities with given VIN
    SerializeVINFromString(vehicle_info_request.preselection_value, ret_val.second,
                           static_cast<uint8_t>(vehicle_info_request.preselection_value.length()), 1U);
  } else if (ret_val.first == 2U) {
    // 2U : DoIP Entities with given EID
    vehicle_info_request.preselection_value.erase(
        remove(vehicle_info_request.preselection_value.begin(), vehicle_info_request.preselection_value.end(), ':'),
        vehicle_info_request.preselection_value.end());
    SerializeEIDGIDFromString(vehicle_info_request.preselection_value, ret_val.second,
                              static_cast<uint8_t>(vehicle_info_request.preselection_value.length()), 2U);
  } else {
    // log failure
  }
  return ret_val;
}

}  // namespace conversation
}  // namespace client
}  // namespace diag
