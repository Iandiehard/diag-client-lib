/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "doip_handler/doip_udp_handler.h"

#include <utility>

#include "doip_handler/common_doip_types.h"

namespace ara {
namespace diag {
namespace doip {

void SerializePayloadFromString(std::string& input_string, std::vector<uint8_t>& output_buffer) {
  std::stringstream input_string_value{input_string};
  for (auto count = 0U; count < input_string.length(); count++) {
    auto get_byte{input_string_value.get()};
    // std::string each_byte{get_byte};
    output_buffer.emplace_back(get_byte);
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
    while (!exit_request_) {
      if (!running_) {
        cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
      }
      if (!exit_request_) {
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
  received_doip_message_.host_ip_address = udp_rx_message->host_ip_address_;
  received_doip_message_.port_num = udp_rx_message->host_port_num_;
  received_doip_message_.protocol_version = udp_rx_message->rx_buffer_[0];
  received_doip_message_.protocol_version_inv = udp_rx_message->rx_buffer_[1];
  received_doip_message_.payload_type = GetDoIPPayloadType(udp_rx_message->rx_buffer_);
  received_doip_message_.payload_length = GetDoIPPayloadLength(udp_rx_message->rx_buffer_);

  if (received_doip_message_.payload_length > 0u) {
    received_doip_message_.payload.clear();
    received_doip_message_.payload.resize(udp_rx_message->rx_buffer_.size() - kDoipheadrSize);
    (void) std::copy(udp_rx_message->rx_buffer_.begin() + kDoipheadrSize,
                     udp_rx_message->rx_buffer_.begin() + kDoipheadrSize + udp_rx_message->rx_buffer_.size(),
                     received_doip_message_.payload.begin());
  }
  // Trigger async transmission
  running_ = true;
  cond_var_.notify_all();
}

auto DoipUdpHandler::GetDoIPPayloadType(std::vector<uint8_t> payload) noexcept -> uint16_t {
  return ((uint16_t) (((payload[BYTE_POS_TWO] & 0xFF) << 8) | (payload[BYTE_POS_THREE] & 0xFF)));
}

auto DoipUdpHandler::GetDoIPPayloadLength(std::vector<uint8_t> payload) noexcept -> uint32_t {
  return ((uint32_t) ((payload[BYTE_POS_FOUR] << 24) & 0xFF000000) |
          (uint32_t) ((payload[BYTE_POS_FIVE] << 16) & 0x00FF0000) |
          (uint32_t) ((payload[BYTE_POS_SIX] << 8) & 0x0000FF00) | (uint32_t) ((payload[BYTE_POS_SEVEN] & 0x000000FF)));
}

void DoipUdpHandler::SetExpectedVehicleIdentificationResponseToBeSent(DoipUdpHandler::VehicleAddrInfo& vehicle_info) {
  expected_vehicle_info_ = vehicle_info;
}

auto DoipUdpHandler::VerifyExpectedVehicleIdentificationRequestReceived(DoipMessage& expected_doip_message) noexcept
    -> bool {
  bool ret_val{false};

  //
  return ret_val;
}

void DoipUdpHandler::Transmit() {
  UdpMessagePtr vehicle_identification_response{std::make_unique<UdpMessage>()};
  // create header
  vehicle_identification_response->tx_buffer_.reserve(kDoipheadrSize + kDoip_VehicleAnnouncement_ResMaxLen);
  CreateDoipGenericHeader(vehicle_identification_response->tx_buffer_, kDoip_VehicleAnnouncement_ResType,
                          kDoip_VehicleAnnouncement_ResMaxLen);
  // vin
  SerializePayloadFromString(expected_vehicle_info_.vin, vehicle_identification_response->tx_buffer_);
  // logical address
  vehicle_identification_response->tx_buffer_.emplace_back(expected_vehicle_info_.logical_address >> 8U);
  vehicle_identification_response->tx_buffer_.emplace_back(expected_vehicle_info_.logical_address & 0xFFU);
  // eid
  SerializePayloadFromString(expected_vehicle_info_.eid, vehicle_identification_response->tx_buffer_);
  // gid
  SerializePayloadFromString(expected_vehicle_info_.gid, vehicle_identification_response->tx_buffer_);
  // set remote ip
  vehicle_identification_response->host_ip_address_ = received_doip_message_.host_ip_address;
  // set remote port
  vehicle_identification_response->host_port_num_ = received_doip_message_.port_num;

  if (udp_socket_handler_unicast_.Transmit(std::move(vehicle_identification_response))) { running_ = false; }
}

void DoipUdpHandler::CreateDoipGenericHeader(std::vector<uint8_t>& doipHeader, std::uint16_t payloadType,
                                             std::uint32_t payloadLen) {
  doipHeader.push_back(kDoip_ProtocolVersion);
  doipHeader.push_back(~((uint8_t) kDoip_ProtocolVersion));
  doipHeader.push_back((uint8_t) ((payloadType & 0xFF00) >> 8));
  doipHeader.push_back((uint8_t) (payloadType & 0x00FF));
  doipHeader.push_back((uint8_t) ((payloadLen & 0xFF000000) >> 24));
  doipHeader.push_back((uint8_t) ((payloadLen & 0x00FF0000) >> 16));
  doipHeader.push_back((uint8_t) ((payloadLen & 0x0000FF00) >> 8));
  doipHeader.push_back((uint8_t) (payloadLen & 0x000000FF));
}

}  // namespace doip
}  // namespace diag
}  // namespace ara