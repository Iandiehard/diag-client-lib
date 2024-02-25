/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_UDP_CHANNEL_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_UDP_CHANNEL_H_

#include <memory>
#include <string_view>
#include <utility>

#include "channel/udp_channel/doip_udp_channel_handler.h"
#include "sockets/socket_handler.h"
#include "uds_transport/connection.h"

namespace doip_client {
namespace channel {
namespace udp_channel {

/**
 * @brief       Class to manage a udp channel as per DoIP protocol
 */
class DoipUdpChannel final {
 public:
  /**
   * @brief  Type alias for Tcp socket handler
   */
  using UdpSocketHandler = sockets::UdpSocketHandler;

  /**
   * @brief  Type alias for Udp message pointer
   */
  using UdpMessagePtr = sockets::UdpSocketHandler::MessagePtr;

  /**
   * @brief         Constructs an instance of UdpChannel
   * @param[in]     udp_ip_address
   *                The local ip address
   * @param[in]     port_num
   *                The reference to tcp transport handler
   * @param[in]     connection
   *                The reference to tcp transport handler
   */
  DoipUdpChannel(UdpSocketHandler udp_socket_handler_broadcast, UdpSocketHandler udp_socket_handler_unicast,
                 uds_transport::Connection &connection);

  /**
   * @brief         Destruct an instance of UdpChannel
   */
  ~DoipUdpChannel() = default;

  /**
   * @brief        Function to start the channel
   */
  void Start();

  /**
   * @brief        Function to stop the channel
   */
  void Stop();

  /**
   * @brief       Function to indicate a start of reception of message
   * @details     This is called to indicate the reception of new message by underlying transport protocol handler
   * @param[in]   source_addr
   *              The UDS source address of message
   * @param[in]   target_addr
   *              The UDS target address of message
   * @param[in]   type
   *              The indication whether its is phys/func request
   * @param[in]   channel_id
   *              The transport protocol channel on which message start happened
   * @param[in]   size
   *              The size in bytes of the UdsMessage starting from SID
   * @param[in]   priority
   *              The priority of the given message, used for prioritization of conversations
   * @param[in]   protocol_kind
   *              The identifier of protocol kind associated to message
   * @param[in]   payload_info
   *              The view onto the first received payload bytes, if any. This view shall be used only within this function call.
   *              It is recommended that the TP provides at least the first two bytes of the request message,
   *              so the DM can identify a functional TesterPresent
   * @return      std::pair< IndicationResult, UdsMessagePtr >
   *              The pair of IndicationResult and a pointer to UdsMessage owned/created by DM core and returned
   *              to the handler to get filled
   */
  std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> IndicateMessage(
      uds_transport::UdsMessage::Address source_addr, uds_transport::UdsMessage::Address target_addr,
      uds_transport::UdsMessage::TargetAddressType type, uds_transport::ChannelID channel_id, std::size_t size,
      uds_transport::Priority priority, uds_transport::ProtocolKind protocol_kind,
      core_type::Span<std::uint8_t> payload_info);

  /**
   * @brief       Function to Hands over a valid received Uds message to upper layer
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(uds_transport::UdsMessagePtr message);

  /**
   * @brief       Function to process the received Udp broadcast message from socket layer
   * @param[in]   udp_rx_message
   *              The Udp message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the channel here
   */
  void ProcessReceivedUdpBroadcast(UdpMessagePtr udp_rx_message);

  /**
   * @brief       Function to process the received Udp unicast message from socket layer
   * @param[in]   udp_rx_message
   *              The Udp message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the channel here
   */
  void ProcessReceivedUdpUnicast(UdpMessagePtr udp_rx_message);

  /**
   * @brief       Function to transmit a Vehicle Identification request
   * @param[in]   message
   *              The vehicle identification message
   */
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(uds_transport::UdsMessageConstPtr message);

 private:
  /**
   * @brief  Store the udp socket handler for broadcast messages
   */
  UdpSocketHandler udp_socket_handler_broadcast_;

  /**
   * @brief  Store the udp socket handler for unicast messages
   */
  UdpSocketHandler udp_socket_handler_unicast_;

  /**
   * @brief  Store the doip channel handler
   */
  DoipUdpChannelHandler udp_channel_handler_;

  /**
   * @brief  Store the reference to doip connection
   */
  uds_transport::Connection &connection_;
};
}  // namespace udp_channel
}  // namespace channel
}  // namespace doip_client

#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_DOIP_UDP_CHANNEL_H_
