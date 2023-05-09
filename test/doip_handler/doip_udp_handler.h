/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_DOIP_UDP_HANDLER_H
#define DIAG_CLIENT_DOIP_UDP_HANDLER_H

#include <string>

#include "doip_handler/doip_payload_type.h"
#include "doip_handler/udp_socket_handler.h"

namespace doip_handler {

// typedef
using ip_address = std::string;
using UdpMessagePtr = udpSocket::UdpMessagePtr;
using UdpMessage = udpSocket::UdpMessage;

class DoipUdpHandler {
public:
  struct VehicleAddrInfo {
    std::uint16_t logical_address;
    std::string vin;
    std::string eid;
    std::string gid;
  };

  // ctor
  DoipUdpHandler(ip_address local_udp_address, uint16_t udp_port_num);

  // dtor
  ~DoipUdpHandler();

  // function to perform initialization
  void Initialize();

  // function to perform de-initialization
  void DeInitialize();

  // function to create the expected VehicleIdentification Response
  void SetExpectedVehicleIdentificationResponseToBeSent(VehicleAddrInfo &vehicle_info);

  // function to set the expectation of VIN on the request received
  auto VerifyVehicleIdentificationRequestWithExpectedVIN(std::string_view vin) noexcept -> bool;

  // function to set the expectation of EID on the request received
  auto VerifyVehicleIdentificationRequestWithExpectedEID(std::string_view eid) noexcept -> bool;

private:
  // udp socket handler unicast
  udpSocket::DoipUdpSocketHandler udp_socket_handler_unicast_;

  // udp socket handler broadcast
  udpSocket::DoipUdpSocketHandler udp_socket_handler_broadcast_;

  // Expected Vehicle info
  VehicleAddrInfo expected_vehicle_info_{};

  // Received doip message
  DoipMessage received_doip_message_{};

  // flag to terminate the thread
  std::atomic_bool exit_request_{false};

  // flag th start the thread
  std::atomic_bool running_{false};

  // conditional variable to block the thread
  std::condition_variable cond_var_;

  // threading var
  std::thread thread_;

  // locking critical section
  std::mutex mutex_;

private:
  // function to process udp unicast message received
  void ProcessUdpUnicastMessage(UdpMessagePtr udp_rx_message);

  // Function to get payload type
  static auto GetDoIPPayloadType(std::vector<uint8_t> payload) noexcept -> uint16_t;

  // Function to get payload length
  static auto GetDoIPPayloadLength(std::vector<uint8_t> payload) noexcept -> uint32_t;

  // Function to create the generic header
  static void CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, std::uint16_t payload_type,
                                      std::uint32_t payload_len);

  // Function to trigger transmission of udp messages
  void Transmit();
};

}  // namespace doip_handler

#endif  //DIAG_CLIENT_DOIP_UDP_HANDLER_H
