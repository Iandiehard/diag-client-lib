/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_VEHICLE_DISCOVERY_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_VEHICLE_DISCOVERY_HANDLER_H_

#include "common/doip_message.h"
#include "sockets/socket_handler.h"
#include "uds_transport/protocol_mgr.h"
#include "uds_transport/uds_message.h"

namespace doip_client {
namespace channel {
namespace udp_channel {

// Forward declaration
class DoipUdpChannel;

/**
 * @brief       Class used as a handler to process vehicle announcement messages
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
  VehicleDiscoveryHandler(sockets::UdpSocketHandler &udp_socket_handler, DoipUdpChannel &channel);

  /**
   * @brief         Destruct an instance of VehicleDiscoveryHandler
   */
  ~VehicleDiscoveryHandler();

  /**
   * @brief       Function to process received vehicle announcement response
   * @param[in]   doip_payload
   *              The doip message received
   */
  void ProcessVehicleAnnouncementResponse(DoipMessage &doip_payload) noexcept;

 private:
  /**
   * @brief  Forward declaration Handler implementation
   */
  class VehicleDiscoveryHandlerImpl;

  /**
   * @brief  Stores the Handler implementation
   */
  std::unique_ptr<VehicleDiscoveryHandlerImpl> handler_impl_;
};

}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client
#endif  //DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_VEHICLE_DISCOVERY_HANDLER_H_
