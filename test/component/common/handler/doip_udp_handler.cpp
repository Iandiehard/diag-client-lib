/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "common/handler/doip_udp_handler.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iomanip>

#include "common/message/doip_message.h"

namespace test {
namespace component {
namespace common {
namespace handler {
namespace {

constexpr std::uint8_t kDoipProtocolVersion{0x03};
constexpr std::uint16_t kDoip_VehicleIdentification_ReqType{0x0001};
constexpr std::uint16_t kDoip_VehicleIdentificationEID_ReqType{0x0002};
constexpr std::uint16_t kDoip_VehicleIdentificationVIN_ReqType{0x0003};
constexpr std::uint16_t kDoip_VehicleAnnouncement_ResType{0x0004};

auto CreateDoipGenericHeader(std::uint16_t payload_type, std::uint32_t payload_len) noexcept
    -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> output_buffer{};
  output_buffer.emplace_back(kDoipProtocolVersion);
  output_buffer.emplace_back(~(static_cast<std::uint8_t>(kDoipProtocolVersion)));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_type & 0xFF00) >> 8));
  output_buffer.emplace_back(static_cast<std::uint8_t>(payload_type & 0x00FF));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0xFF000000) >> 24));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x00FF0000) >> 16));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x0000FF00) >> 8));
  output_buffer.emplace_back(static_cast<std::uint8_t>(payload_len & 0x000000FF));
  return output_buffer;
}

template<typename T>
auto SerializeToByteVectorFromString(T input_string, std::uint8_t substring_range) noexcept
    -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> output_buffer{};
  std::uint8_t const string_length{static_cast<std::uint8_t>(input_string.length())};
  output_buffer.reserve(string_length);

  for (std::uint8_t char_count{0u}; char_count < string_length; char_count += substring_range) {
    std::string single_char{input_string.substr(char_count, substring_range)};
    std::stringstream single_char_stream{single_char};
    int single_byte{single_char_stream.get()};
    output_buffer.emplace_back(static_cast<std::uint8_t>(single_byte));
  }
  return output_buffer;
}

template<typename T>
auto SerializeToHexVectorFromString(T input_string, std::uint8_t substring_range) noexcept
    -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> output_buffer{};
  std::uint8_t const string_length{static_cast<std::uint8_t>(input_string.length())};
  output_buffer.reserve(string_length);

  for (std::uint8_t char_count{0u}; char_count < string_length; char_count += substring_range) {
    std::string single_char{input_string.substr(char_count, substring_range)};
    std::stringstream single_char_stream{single_char};
    int single_byte{};
    single_char_stream >> std::hex >> single_byte;
    output_buffer.emplace_back(static_cast<std::uint8_t>(single_byte));
  }
  return output_buffer;
}

template<typename T>
auto RemoveCharFromString(std::string_view input_string, T removal_type) noexcept -> std::string {
  std::string result{input_string};
  result.erase(std::remove(result.begin(), result.end(), removal_type), result.end());
  return result;
}

template<typename T>
auto AddCharToString(std::string_view input_string, T character, std::uint8_t substring_range) noexcept -> std::string {
  std::string result{};
  std::uint8_t const string_length{static_cast<std::uint8_t>(input_string.length())};
  for (std::uint8_t char_count{0u}; char_count < string_length; char_count += substring_range) {
    std::string single_substring{input_string.substr(char_count, substring_range)};
    single_substring += character;
    result += single_substring;
  }
  result.pop_back();  // remove last ":" appended before
  return result;
}

template<typename Container>
auto ConvertToAsciiString(Container container) noexcept -> std::string {
  std::string ascii_string{};
  for (std::uint8_t const byte: container) {
    std::stringstream single_char_stream{};
    single_char_stream << byte;
    ascii_string += single_char_stream.str();
  }
  return ascii_string;
}

template<typename Container>
auto ConvertToHexString(Container container) noexcept -> std::string {
  std::string hex_string{};
  for (std::uint8_t const byte: container) {
    std::stringstream single_char_stream{};
    int payload_byte{byte};
    single_char_stream << std::setw(2) << std::setfill('0') << std::hex << payload_byte;
    hex_string += single_char_stream.str();
  }
  return hex_string;
}
}  // namespace

DoipUdpHandler::DoipUdpHandler(std::string_view broadcast_ip_address, std::string_view unicast_ip_address,
                               std::uint16_t local_port_number)
    : udp_broadcast_server_{broadcast_ip_address, local_port_number},
      udp_unicast_server_{unicast_ip_address, local_port_number} {}

void DoipUdpHandler::Initialize() {
  udp_broadcast_server_.SetReadHandler(
      [this](UdpServer::MessagePtr udp_message) { ProcessReceivedUdpMessage(std::move(udp_message)); });
  udp_unicast_server_.SetReadHandler(
      [this](UdpServer::MessagePtr udp_message) { ProcessReceivedUdpMessage(std::move(udp_message)); });
  udp_broadcast_server_.Initialize();
  udp_unicast_server_.Initialize();
}

void DoipUdpHandler::DeInitialize() {
  udp_broadcast_server_.DeInitialize();
  udp_unicast_server_.DeInitialize();
}

void DoipUdpHandler::ProcessReceivedUdpMessage(DoipUdpHandler::UdpServer::MessagePtr udp_message) {
  message::DoipMessage doip_message{udp_message->GetHostIpAddress(), udp_message->GetHostPortNumber(),
                                    udp_message->GetPayload()};

  switch (doip_message.GetPayloadType()) {
    case kDoip_VehicleIdentification_ReqType:
      ProcessVehicleIdentificationRequestMessage(doip_message.GetHostIpAddress(), doip_message.GetHostPortNumber(), {},
                                                 {});
      break;
    case kDoip_VehicleIdentificationEID_ReqType: {
      std::string eid{AddCharToString(ConvertToHexString(doip_message.GetPayload()), ':', 2u)};
      ProcessVehicleIdentificationRequestMessage(doip_message.GetHostIpAddress(), doip_message.GetHostPortNumber(), eid,
                                                 {});
    } break;
    case kDoip_VehicleIdentificationVIN_ReqType:
      ProcessVehicleIdentificationRequestMessage(doip_message.GetHostIpAddress(), doip_message.GetHostPortNumber(), {},
                                                 ConvertToAsciiString(doip_message.GetPayload()));
      break;
  }
}

void DoipUdpHandler::SendUdpMessage(DoipUdpHandler::UdpServer::MessageConstPtr udp_message) noexcept {
  EXPECT_TRUE(udp_unicast_server_.Transmit(std::move(udp_message)).HasValue());
}

auto DoipUdpHandler::ComposeVehicleIdentificationResponse(std::string_view remote_ip_address,
                                                          std::uint16_t remote_port_number, std::string_view vin,
                                                          std::uint16_t logical_address, std::string_view eid,
                                                          std::string_view gid, std::uint8_t action_byte,
                                                          std::optional<std::uint8_t> sync_status) noexcept
    -> UdpServer::MessagePtr {
  // Create header
  constexpr std::uint8_t kHeaderSize{8u};
  UdpServer::Message::BufferType response_buffer{CreateDoipGenericHeader(kDoip_VehicleAnnouncement_ResType, 32)};
  // Add VIN
  UdpServer::Message::BufferType const vin_in_bytes{SerializeToByteVectorFromString(vin, 1u)};
  response_buffer.insert(std::next(response_buffer.begin(), kHeaderSize), vin_in_bytes.cbegin(), vin_in_bytes.cend());
  // Add LA
  response_buffer.emplace_back(logical_address >> 8U);
  response_buffer.emplace_back(logical_address & 0xFFU);
  // Add EID
  UdpServer::Message::BufferType const eid_in_bytes{SerializeToHexVectorFromString(RemoveCharFromString(eid, ':'), 2u)};
  constexpr std::uint8_t kEidOffset{kHeaderSize + 19u};
  response_buffer.insert(std::next(response_buffer.begin(), kEidOffset), eid_in_bytes.cbegin(), eid_in_bytes.cend());
  // Add GID
  UdpServer::Message::BufferType const gid_in_bytes{SerializeToHexVectorFromString(RemoveCharFromString(gid, ':'), 2u)};
  constexpr std::uint8_t kGidOffset{kHeaderSize + 25u};
  response_buffer.insert(std::next(response_buffer.begin(), kGidOffset), gid_in_bytes.cbegin(), gid_in_bytes.cend());
  // Set Further action byte
  response_buffer.emplace_back(action_byte);
  // Set sync status
  if (sync_status.has_value()) { response_buffer.emplace_back(sync_status.value()); }
  UdpServer::MessagePtr response{
      std::make_unique<UdpServer::Message>(remote_ip_address, remote_port_number, std::move(response_buffer))};
  return response;
}

}  // namespace handler
}  // namespace common
}  // namespace component
}  // namespace test