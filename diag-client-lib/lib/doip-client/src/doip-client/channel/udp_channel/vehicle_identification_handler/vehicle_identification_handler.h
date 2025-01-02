/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DOIP_CLIENT_CHANNEL_UDP_CHANNEL_VEHICLE_IDENTIFICATION_HANDLER_VEHICLE_IDENTIFICATION_HANDLER_H_
#define DOIP_CLIENT_CHANNEL_UDP_CHANNEL_VEHICLE_IDENTIFICATION_HANDLER_VEHICLE_IDENTIFICATION_HANDLER_H_

#include "doip-client/message/doip_message.h"
#include "doip-client/sockets/udp_socket_handler.h"
#include "uds_transport-layer-api/protocol_mgr.h"
#include "uds_transport-layer-api/uds_message.h"

namespace doip_client {
namespace channel {
namespace udp_channel {
// Forward declaration
class DoipUdpChannel;

namespace vehicle_identification_handler {

/**
 * @brief       Class used as a handler to process vehicle identification req/ res messages
 */
class VehicleIdentificationHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using UdpMessagePtr = sockets::UdpSocketHandler::MessagePtr;

  /**
   * @brief  Type alias for Udp message
   */
  using UdpMessage = sockets::UdpSocketHandler::Message;

  /**
  * @brief  Type alias for Doip message
  */
  using DoipMessage = message::DoipMessage;

 public:
  /**
   * @brief         Constructs an instance of VehicleIdentificationHandler
   * @param[in]     udp_socket_handler
   *                The reference to socket handler
   * @param[in]     channel
   *                The reference to doip udp channel
   */
  VehicleIdentificationHandler(sockets::UdpSocketHandler &udp_socket_handler,
                               DoipUdpChannel &channel);

  /**
   * @brief         Destruct an instance of VehicleIdentificationHandler
   */
  ~VehicleIdentificationHandler();

  /**
   * @brief       Function to handle sending of vehicle identification request
   * @param[in]   vehicle_identification_request
   *              The vehicle identification request
   * @return      Transmission result
   */
  auto HandleVehicleIdentificationRequest(
      uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  /**
   * @brief       Function to process received vehicle identification response
   * @param[in]   doip_payload
   *              The doip message received
   */
  void ProcessVehicleIdentificationResponse(DoipMessage &doip_payload) noexcept;

 private:
  /**
   * @brief       Function to send vehicle identification request
   * @param[in]   vehicle_identification_request
   *              The vehicle identification request
   * @return      Transmission result
   */
  auto SendVehicleIdentificationRequest(
      uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

 private:
  /**
   * @brief  Forward declaration Handler implementation
   */
  class VehicleIdentificationHandlerImpl;

  /**
   * @brief  Stores the Handler implementation
   */
  std::unique_ptr<VehicleIdentificationHandlerImpl> handler_impl_;
};

}  // namespace vehicle_identification_handler
}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client
#endif  //DOIP_CLIENT_CHANNEL_UDP_CHANNEL_VEHICLE_IDENTIFICATION_HANDLER_VEHICLE_IDENTIFICATION_HANDLER_H_
