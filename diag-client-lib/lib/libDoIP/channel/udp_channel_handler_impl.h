/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_HANDLER_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_HANDLER_IMPL_H
//includes
#include <mutex>
#include "channel/udp_channel_state_impl.h"
#include "common/common_doip_types.h"
#include "common/doip_payload_type.h"
#include "common_Header.h"
#include "sockets/udp_socket_handler.h"

namespace ara {
namespace diag {
namespace doip {
// forward declaration
namespace udpChannel {
class UdpChannel;
}

namespace udpTransport {
class UdpTransportHandler;
}

namespace udpChannelHandlerImpl {

using UdpMessage = ara::diag::doip::udpSocket::UdpMessage;
using UdpMessagePtr = ara::diag::doip::udpSocket::UdpMessagePtr;
using UdpVehicleIdentificationState = udpChannelStateImpl::VehicleIdentificationState;
using UdpVehicleDiscoveryState = udpChannelStateImpl::VehicleDiscoveryState;
using VehiclePayloadType = std::pair<std::uint16_t, std::uint8_t>;

/*
 @ Class Name        : VehicleDiscoveryHandler
 @ Class Description : Class used as a handler to process vehicle announcement messages
 */
class VehicleDiscoveryHandler {
public:
  // ctor
  VehicleDiscoveryHandler(
    udpSocket::UdpSocketHandler &udp_socket_handler,
    udpTransport::UdpTransportHandler &udp_transport_handler,
    udpChannel::UdpChannel &channel) : udp_socket_handler_{udp_socket_handler},
                                       udp_transport_handler_{udp_transport_handler},
                                       channel_{channel} {}
  
  // dtor
  ~VehicleDiscoveryHandler() = default;
  
  // Function to send vehicle identification request
  auto SendVehicleIdentificationRequest(
    uds_transport::UdsMessageConstPtr message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;
  
  // Function to process Vehicle announcement response
  auto ProcessVehicleAnnouncementResponse(DoipMessage &doip_payload) noexcept -> void;
  
  // Function to process Vehicle identification response
  auto ProcessVehicleIdentificationResponse(DoipMessage &doip_payload) noexcept -> void;

private:
  // Function to handle Vehicle Identification Request
  auto HandleVehicleIdentificationRequest(
    uds_transport::UdsMessageConstPtr message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;
  
  static auto CreateDoipGenericHeader(
    std::vector<uint8_t> &doipHeader,
    std::uint16_t payloadType,
    std::uint32_t payloadLen) noexcept -> void;
  
  static auto GetVehicleIdentificationPayloadType(
    std::uint8_t preselection_mode) noexcept -> const VehiclePayloadType;
  
  // socket reference
  udpSocket::UdpSocketHandler &udp_socket_handler_;
  // transport handler reference
  udpTransport::UdpTransportHandler &udp_transport_handler_;
  // channel reference
  udpChannel::UdpChannel &channel_;
};

/*
 @ Class Name        : UdpChannelHandlerImpl
 @ Class Description : Class to handle received messages from lower layer
 */
class UdpChannelHandlerImpl {
public:
  // ctor
  UdpChannelHandlerImpl(
    udpSocket::UdpSocketHandler &udp_socket_handler_bcast,
    udpSocket::UdpSocketHandler &udp_socket_handler_ucast,
    udpTransport::UdpTransportHandler &udp_transport_handler,
    udpChannel::UdpChannel &channel)
    : vehicle_discovery_handler_{udp_socket_handler_bcast, udp_transport_handler, channel},
      vehicle_identification_handler_{udp_socket_handler_ucast, udp_transport_handler, channel} {}
  
  // dtor
  ~UdpChannelHandlerImpl() = default;
  
  // Function to trigger transmission
  auto Transmit(
    ara::diag::uds_transport::UdsMessageConstPtr message)
      noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;
  
  // process message unicast
  auto HandleMessage(UdpMessagePtr udp_rx_message) noexcept -> void;
  
  // process message broadcast
  auto HandleMessageBroadcast(UdpMessagePtr udp_rx_message) noexcept -> void;

private:
  // Function to process DoIP Header
  static auto ProcessDoIPHeader(DoipMessage &doip_rx_message, uint8_t &nackCode) noexcept -> bool;
  
  // Function to verify payload length of various payload type
  static auto ProcessDoIPPayloadLength(uint32_t payload_len, uint16_t payload_type) noexcept -> bool;
  
  // Function to get payload type
  static auto GetDoIPPayloadType(std::vector<uint8_t> payload) noexcept -> uint16_t;
  
  // Function to get payload length
  static auto GetDoIPPayloadLength(std::vector<uint8_t> payload) noexcept -> uint32_t;
  
  // Function to process DoIP payload responses
  auto ProcessDoIPPayload(DoipMessage &doip_payload,
                          DoipMessage::rx_socket_type socket_type = DoipMessage::rx_socket_type::kUnicast) noexcept -> void;
  
  // handler to process vehicle announcement
  VehicleDiscoveryHandler vehicle_discovery_handler_;
  // handler to process vehicle identification req/ res
  VehicleDiscoveryHandler vehicle_identification_handler_;
  // mutex to protect critical section
  std::mutex channel_handler_lock;
};
}  // namespace udpChannelHandlerImpl
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_HANDLER_IMPL_H
