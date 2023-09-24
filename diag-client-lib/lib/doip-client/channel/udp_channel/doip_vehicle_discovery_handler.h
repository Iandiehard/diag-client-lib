/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_VEHICLE_DISCOVERY_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_VEHICLE_DISCOVERY_HANDLER_H_

namespace doip_client {
namespace channel {
namespace udp_channel {

/**
 * @brief       Class used as a handler to process vehicle discovery messages
 */
class VehicleDiscoveryHandler final {
 public:
  /**
   * @brief         Constructs an instance of VehicleDiscoveryHandler
   * @param[in]     udp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to doip udp channel
   */
  VehicleDiscoveryHandler(sockets::UdpSocketHandler &udp_socket_handler, DoipUdpChannel &channel)
      : udp_socket_handler_{udp_socket_handler},
        channel_{channel} {}

  // dtor
  ~VehicleDiscoveryHandler() = default;

  // Function to send vehicle identification request
  auto SendVehicleIdentificationRequest(uds_transport::UdsMessageConstPtr message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  // Function to process Vehicle announcement response
  auto ProcessVehicleAnnouncementResponse(DoipMessage &doip_payload) noexcept -> void;

  // Function to process Vehicle identification response
  auto ProcessVehicleIdentificationResponse(DoipMessage &doip_payload) noexcept -> void;

 private:
  // Function to handle Vehicle Identification Request
  auto HandleVehicleIdentificationRequest(uds_transport::UdsMessageConstPtr message) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  static void CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, std::uint16_t payloadType,
                                      std::uint32_t payloadLen);

  static auto GetVehicleIdentificationPayloadType(std::uint8_t preselection_mode) noexcept -> VehiclePayloadType;

  // socket reference
  udpSocket::UdpSocketHandler &udp_socket_handler_;
  // transport handler reference
  udp_transport::UdpTransportHandler &udp_transport_handler_;
  // channel reference
  udpChannel::UdpChannel &channel_;
};


}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  //DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_VEHICLE_DISCOVERY_HANDLER_H_
