/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "doip_handler/doip_udp_handler.h"

#include <algorithm>
#include <utility>

#include "doip_handler/common_doip_types.h"

namespace doip_handler {

std::string ConvertToHexString(std::uint8_t char_start, std::uint8_t char_count,
                               std::vector<std::uint8_t>& input_buffer) {
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
                                 std::vector<std::uint8_t>& input_buffer) {
  std::string ascii_string{};
  std::uint8_t total_char_count{static_cast<uint8_t>(char_start + char_count)};

  for (std::uint8_t char_start_count = char_start; char_start_count < total_char_count; char_start_count++) {
    std::stringstream vehicle_info_data_vin{};
    vehicle_info_data_vin << input_buffer[char_start_count];
    ascii_string.append(vehicle_info_data_vin.str());
  }
  return ascii_string;
}

void SerializeEIDGIDFromString(std::string& input_string, std::vector<uint8_t>& output_buffer, std::uint8_t total_size,
                               std::uint8_t substring_range) {

  for (auto char_count = 0U; char_count < total_size; char_count += substring_range) {
    std::string input_string_new{input_string.substr(char_count, static_cast<std::uint8_t>(substring_range))};
    std::stringstream input_string_stream{input_string_new};
    int get_byte;
    input_string_stream >> std::hex >> get_byte;
    output_buffer.emplace_back(static_cast<std::uint8_t>(get_byte));
  }
}

void SerializeVINFromString(std::string& input_string, std::vector<uint8_t>& output_buffer, std::uint8_t total_size,
                            std::uint8_t substring_range) {

  for (auto char_count = 0U; char_count < total_size; char_count += substring_range) {
    std::string input_string_new{input_string.substr(char_count, static_cast<std::uint8_t>(substring_range))};
    std::stringstream input_string_stream{input_string_new};
    int get_byte{input_string_stream.get()};
    output_buffer.emplace_back(static_cast<std::uint8_t>(get_byte));
  }
}

DoipUdpHandler::DoipUdpHandler(ip_address local_udp_address, uint16_t udp_port_num)
    : udp_socket_handler_unicast_{local_udp_address, udp_port_num,
                                  udpSocket::DoipUdpSocketHandler::PortType::kUdp_Unicast,
                                  [this](UdpMessagePtr udp_rx_message) {
                                    ProcessUdpUnicastMessage(std::move(udp_rx_message));
                                  }},
      udp_socket_handler_broadcast_{
          local_udp_address, udp_port_num, udpSocket::DoipUdpSocketHandler::PortType::kUdp_Broadcast,
          [this](UdpMessagePtr udp_rx_message) { ProcessUdpUnicastMessage(std::move(udp_rx_message)); }} {
  // Start thread to receive messages
  thread_ = std::thread([&]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_.load()) {
      if (!running_.load()) {
        cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
      }
      if (!exit_request_.load()) {
        if (running_) { Transmit(); }
      }
    }
  });
}

DoipUdpHandler::~DoipUdpHandler() {
  exit_request_ = true;
  running_ = false;
  cond_var_.notify_all();
  thread_.join();
};

void DoipUdpHandler::Initialize() {
  udp_socket_handler_unicast_.Start();
  udp_socket_handler_broadcast_.Start();
}

void DoipUdpHandler::DeInitialize() {
  udp_socket_handler_unicast_.Stop();
  udp_socket_handler_broadcast_.Stop();
}

void DoipUdpHandler::ProcessUdpUnicastMessage(UdpMessagePtr udp_rx_message) {
  received_doip_message_.host_ip_address = udp_rx_message->GetHostIpAddress();
  received_doip_message_.port_num = udp_rx_message->GetHostPortNumber();
  received_doip_message_.protocol_version = udp_rx_message->GetRxBuffer()[0];
  received_doip_message_.protocol_version_inv = udp_rx_message->GetRxBuffer()[1];
  received_doip_message_.payload_type = GetDoIPPayloadType(udp_rx_message->GetRxBuffer());
  received_doip_message_.payload_length = GetDoIPPayloadLength(udp_rx_message->GetRxBuffer());

  if (received_doip_message_.payload_length > 0U) {
    received_doip_message_.payload.insert(received_doip_message_.payload.begin(),
                                          udp_rx_message->GetRxBuffer().begin() + kDoipheadrSize,
                                          udp_rx_message->GetRxBuffer().end());
  }
  // Trigger async transmission
  running_ = true;
  cond_var_.notify_all();
}

auto DoipUdpHandler::GetDoIPPayloadType(core_type::Span<uint8_t> payload) noexcept -> uint16_t {
  return ((uint16_t) (((payload[BYTE_POS_TWO] & 0xFF) << 8) | (payload[BYTE_POS_THREE] & 0xFF)));
}

auto DoipUdpHandler::GetDoIPPayloadLength(core_type::Span<uint8_t> payload) noexcept -> uint32_t {
  return ((uint32_t) ((payload[BYTE_POS_FOUR] << 24U) & 0xFF000000) |
          (uint32_t) ((payload[BYTE_POS_FIVE] << 16U) & 0x00FF0000) |
          (uint32_t) ((payload[BYTE_POS_SIX] << 8U) & 0x0000FF00) |
          (uint32_t) ((payload[BYTE_POS_SEVEN] & 0x000000FF)));
}

void DoipUdpHandler::SetExpectedVehicleIdentificationResponseToBeSent(DoipUdpHandler::VehicleAddrInfo& vehicle_info) {
  DoipUdpHandler::VehicleAddrInfo create_info{vehicle_info};
  create_info.eid.erase(remove(create_info.eid.begin(), create_info.eid.end(), ':'), create_info.eid.end());
  create_info.gid.erase(remove(create_info.gid.begin(), create_info.gid.end(), ':'), create_info.gid.end());
  expected_vehicle_info_ = create_info;
}

auto DoipUdpHandler::VerifyVehicleIdentificationRequestWithExpectedVIN(std::string_view vin) noexcept -> bool {
  constexpr std::uint8_t start_index_vin{0U};
  std::string vehicle_info_data_vin{
      ConvertToAsciiString(start_index_vin, received_doip_message_.payload.size(), received_doip_message_.payload)};
  return (vehicle_info_data_vin == vin);
}

auto DoipUdpHandler::VerifyVehicleIdentificationRequestWithExpectedEID(std::string_view eid) noexcept -> bool {
  constexpr std::uint8_t start_index_eid{0U};
  std::string vehicle_info_data_vin{
      ConvertToHexString(start_index_eid, received_doip_message_.payload.size(), received_doip_message_.payload)};
  return (vehicle_info_data_vin == eid);
}

void DoipUdpHandler::Transmit() {
  UdpMessagePtr vehicle_identification_response{
      std::make_unique<UdpMessage>(received_doip_message_.host_ip_address, received_doip_message_.port_num)};
  // create header
  vehicle_identification_response->GetTxBuffer().reserve(kDoipheadrSize + kDoip_VehicleAnnouncement_ResMaxLen);
  CreateDoipGenericHeader(vehicle_identification_response->GetTxBuffer(), kDoip_VehicleAnnouncement_ResType,
                          kDoip_VehicleAnnouncement_ResMaxLen);
  // vin
  SerializeVINFromString(expected_vehicle_info_.vin, vehicle_identification_response->GetTxBuffer(),
                         expected_vehicle_info_.vin.length(), 1U);
  // logical address
  vehicle_identification_response->GetTxBuffer().emplace_back(expected_vehicle_info_.logical_address >> 8U);
  vehicle_identification_response->GetTxBuffer().emplace_back(expected_vehicle_info_.logical_address & 0xFFU);
  // eid
  SerializeEIDGIDFromString(expected_vehicle_info_.eid, vehicle_identification_response->GetTxBuffer(),
                            expected_vehicle_info_.eid.length(), 2U);
  // gid
  SerializeEIDGIDFromString(expected_vehicle_info_.gid, vehicle_identification_response->GetTxBuffer(),
                            expected_vehicle_info_.eid.length(), 2U);
  //  set further action required
  vehicle_identification_response->GetTxBuffer().emplace_back(0U);

  if (udp_socket_handler_unicast_.Transmit(std::move(vehicle_identification_response))) { running_ = false; }
}

void DoipUdpHandler::CreateDoipGenericHeader(std::vector<uint8_t>& doipHeader, std::uint16_t payload_type,
                                             std::uint32_t payload_len) {
  doipHeader.push_back(kDoip_ProtocolVersion);
  doipHeader.push_back(~((uint8_t) kDoip_ProtocolVersion));
  doipHeader.push_back((uint8_t) ((payload_type & 0xFF00) >> 8));
  doipHeader.push_back((uint8_t) (payload_type & 0x00FF));
  doipHeader.push_back((uint8_t) ((payload_len & 0xFF000000) >> 24));
  doipHeader.push_back((uint8_t) ((payload_len & 0x00FF0000) >> 16));
  doipHeader.push_back((uint8_t) ((payload_len & 0x0000FF00) >> 8));
  doipHeader.push_back((uint8_t) (payload_len & 0x000000FF));
}

}  // namespace doip_handler