/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_UDP_CHANNEL_HANDLER_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_UDP_CHANNEL_HANDLER_H_

#include <mutex>

#include "channel/udp_channel/doip_vehicle_discovery_handler.h"
#include "channel/udp_channel/doip_vehicle_identification_handler.h"
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
 * @brief       Class to handle udp received messages from lower layer
 */
class DoipUdpChannelHandler final {
 public:
  /**
   * @brief  Type alias for Tcp message pointer
   */
  using UdpMessagePtr = sockets::UdpSocketHandler::MessagePtr;

  /**
   * @brief         Constructs an instance of DoipUdpChannelHandler
   * @param[in]     udp_socket_handler_broadcast
   *                The reference to socket handler with broadcast mechanism
   * @param[in]     udp_socket_handler_unicast
   *                The reference to socket handler with unicast mechanism
   * @param[in]     channel
   *                The reference to tcp transport handler
   */
  DoipUdpChannelHandler(sockets::UdpSocketHandler &udp_socket_handler_broadcast,
                        sockets::UdpSocketHandler &udp_socket_handler_unicast, DoipUdpChannel &channel);

  /**
   * @brief         Function to vehicle identification request to the connected network
   * @param[in]     vehicle_identification_request
   *                The vehicle identification request
   * @return        TransmissionResult
   *                The transmission result
   */
  auto SendVehicleIdentificationRequest(uds_transport::UdsMessageConstPtr vehicle_identification_request) noexcept
      -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

  /**
   * @brief         Function to process the received unicast udp message
   * @param[in]     udp_rx_message
   *                The message received
   */
  void HandleMessageUnicast(UdpMessagePtr udp_rx_message) noexcept;

  /**
   * @brief         Function to process the received broadcast udp message
   * @param[in]     udp_rx_message
   *                The message received
   */
  void HandleMessageBroadcast(UdpMessagePtr udp_rx_message) noexcept;

 private:
  /**
   * @brief         Function to process doip header in received response
   * @param[in]     doip_rx_message
   *                The received doip rx message
   * @param[in]     nack_code
   *                The negative ack code
   */
  auto ProcessDoIPHeader(DoipMessage &doip_rx_message, std::uint8_t &nack_code) noexcept -> bool;

  /**
   * @brief         Function to verify payload length of various payload type
   * @param[in]     payload_len
   *                The length of payload received
   * @param[in]     payload_type
   *                The type of payload
   */
  static auto ProcessDoIPPayloadLength(std::uint32_t payload_len, std::uint16_t payload_type) noexcept -> bool;

  /**
   * @brief         Function to process the doip payload
   * @param[in]     doip_payload
   *                The reference to received payload
   */
  void ProcessDoIPPayload(DoipMessage &doip_payload,
                          DoipMessage::RxSocketType socket_type = DoipMessage::RxSocketType::kUnicast);

  /**
   * @brief         Handler to process vehicle discovery messages
   */
  VehicleDiscoveryHandler vehicle_discovery_handler_;

  /**
   * @brief         Handler to process vehicle identification req/res messages
   */
  VehicleIdentificationHandler vehicle_identification_handler_;

  /**
   * @brief         Mutex to protect critical section
   */
  std::mutex channel_handler_lock;
};
}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client

#endif  //DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_UDP_CHANNEL_HANDLER_H_
