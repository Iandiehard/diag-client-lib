/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_TCP_CHANNEL_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_TCP_CHANNEL_H_

#include <memory>
#include <string_view>

#include "channel/tcp_channel_handler_impl.h"
#include "channel/tcp_channel_state_impl.h"
#include "sockets/tcp_socket_handler.h"
#include "uds_transport/connection.h"
#include "utility/sync_timer.h"

namespace doip_client {
namespace channel {
namespace tcp_channel {

// typedefs
using TcpMessage = tcpSocket::TcpMessage;
using TcpMessagePtr = tcpSocket::TcpMessagePtr;
using TcpMessageConstPtr = tcpSocket::TcpMessageConstPtr;
using TcpRoutingActivationChannelState = tcpChannelStateImpl::routingActivationState;
using TcpDiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;

/**
 * @brief       Class to manage a tcp channel as per DoIP protocol
 */
class DoipTcpChannel final {
 public:
  /**
   * @brief  Type alias for Sync timer
   */
  using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;

  /**
   * @brief         Constructs an instance of TcpChannel
   * @param[in]     tcp_ip_address
   *                The local ip address
   * @param[in]     port_num
   *                The reference to tcp transport handler
   */
  DoipTcpChannel(std::string_view tcp_ip_address, std::uint16_t port_num, uds_transport::Connection &connection);

  /**
   * @brief         Destruct an instance of TcpChannel
   */
  ~DoipTcpChannel() = default;

  /**
   * @brief        Function to start the channel
   */
  void Start();

  /**
   * @brief        Function to stop the channel
   */
  void Stop();

  /**
   * @brief        Function to check if connected to host remote server
   * @return       True if connection, False otherwise
   */
  bool IsConnectToHost();

  /**
   * @brief       Function to establish connection to remote host server
   * @param[in]   message
   *              The connection message
   * @return      Connection result
   */
  uds_transport::UdsTransportProtocolMgr::ConnectionResult ConnectToHost(uds_transport::UdsMessageConstPtr message);

  /**
   * @brief       Function to disconnect from remote host server
   * @return      Disconnection result
   */
  uds_transport::UdsTransportProtocolMgr::DisconnectionResult DisconnectFromHost();

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
   * @brief       Function to transmit a valid Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request.
   */
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(uds_transport::UdsMessageConstPtr message);

  /**
   * @brief       Function to Hands over a valid received Uds message
   * @param[in]   message
   *              The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is given
   *              back to the conversation here
   */
  void HandleMessage(uds_transport::UdsMessagePtr message);

  // Function to get the channel context
  auto GetChannelState() noexcept -> tcpChannelStateImpl::TcpChannelStateImpl & { return tcp_channel_state_; }

  // Function to get the sync timer
  auto GetSyncTimer() noexcept -> SyncTimer & { return sync_timer_; }

  // Function to Hand over all the message received
  void ProcessReceivedTcpMessage(TcpMessagePtr tcp_rx_message);

 private:
  // Function to handle the routing states
  uds_transport::UdsTransportProtocolMgr::ConnectionResult HandleRoutingActivationState(
      uds_transport::UdsMessageConstPtr &message);

  // Function to handle the diagnostic request response state
  uds_transport::UdsTransportProtocolMgr::TransmissionResult HandleDiagnosticRequestState(
      uds_transport::UdsMessageConstPtr &message);

 private:
  // tcp socket handler
  std::unique_ptr<tcpSocket::TcpSocketHandler> tcp_socket_handler_;
  // tcp channel state
  tcpChannelStateImpl::TcpChannelStateImpl tcp_channel_state_;
  // tcp channel handler
  tcpChannelHandlerImpl::TcpChannelHandlerImpl tcp_channel_handler_;
  // sync timer
  SyncTimer sync_timer_;
};

}  // namespace tcp_channel
}  // namespace channel
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_TCP_CHANNEL_TCP_CHANNEL_H_
