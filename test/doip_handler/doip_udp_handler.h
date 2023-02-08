/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_DOIP_UDP_HANDLER_H
#define DIAG_CLIENT_DOIP_UDP_HANDLER_H

#include <string>

#include "doip_handler/udp_socket_handler.h"
#include "doip_handler/doip_payload_type.h"

namespace ara {
namespace diag {
namespace doip {

// typedef
using ip_address = std::string;
using UdpMessagePtr = udpSocket::UdpMessagePtr;

class DoipUdpHandler {
public:
  struct VehicleAddrInfo {
    std::uint16_t logical_address;
    std::string vin;
    std::string eid;
    std::string gid;
  };

  // ctor
  DoipUdpHandler(ip_address local_udp_address,
                 uint16_t udp_port_num);

  ~DoipUdpHandler() noexcept = default;

  // function to perform initialization
  void Initialize();

  // function to perform de-initialization
  void DeInitialize();

  // function to create the VehicleIdentification Response
  void CreateVehicleIdentificationResponse(VehicleAddrInfo vehicle_info);

  // function to send out Vehicle Announcement Message
  void SendVehicleAnnouncementMessage();

private:
  // udp socket handler unicast
  udpSocket::DoipUdpSocketHandler udp_socket_handler_unicast_;

  // udp socket handler broadcast
  udpSocket::DoipUdpSocketHandler udp_socket_handler_broadcast_;

  // Vehicle info
  VehicleAddrInfo vehicle_info_;

  // function to process udp unicast message received
  void ProcessUdpUnicastMessage(UdpMessagePtr udp_rx_message);

  // Function to process DoIP Header
  static auto ProcessDoIPHeader(DoipMessage &doip_rx_message, uint8_t &nackCode) noexcept -> bool;

  // Function to verify payload length of various payload type
  static auto ProcessDoIPPayloadLength(uint32_t payload_len, uint16_t payload_type) noexcept -> bool;

  // Function to get payload type
  static auto GetDoIPPayloadType(std::vector<uint8_t> payload) noexcept -> uint16_t;

  // Function to get payload length
  static auto GetDoIPPayloadLength(std::vector<uint8_t> payload) noexcept -> uint32_t;
};

}  // namespace doip
}  // namespace diag
}  // namespace ara

#endif  //DIAG_CLIENT_DOIP_UDP_HANDLER_H
