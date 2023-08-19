/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_H
//includes
#include <functional>
#include <string_view>

#include "channel/udp_channel_handler_impl.h"
#include "channel/udp_channel_state_impl.h"
#include "common/common_doip_types.h"
#include "sockets/udp_socket_handler.h"
#include "utility/executor.h"
#include "utility/sync_timer.h"

namespace doip_client {
// Forward declaration
namespace udp_transport {
class UdpTransportHandler;
}

namespace udpChannel {
//typedefs
using UdpMessagePtr = udpSocket::UdpMessagePtr;
using SyncTimer = utility::sync_timer::SyncTimer<std::chrono::steady_clock>;
using TaskExecutor = utility::executor::Executor<std::function<void(void)>>;

/*
 @ Class Name        : UdpChannel
 @ Class Description : Class used to handle Doip Udp Channel
 */
class UdpChannel {
 public:
  //ctor
  UdpChannel(std::string_view local_ip_address, uint16_t port_num,
             udp_transport::UdpTransportHandler &udp_transport_handler);

  //dtor
  ~UdpChannel() = default;

  // Initialize
  uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();

  //Start
  void Start();

  // Stop
  void Stop();

  // function to handle read broadcast
  void HandleMessageBroadcast(UdpMessagePtr udp_rx_message);

  // function to handle read unicast
  void HandleMessageUnicast(UdpMessagePtr udp_rx_message);

  // Function to trigger transmission of vehicle identification request
  uds_transport::UdsTransportProtocolMgr::TransmissionResult Transmit(uds_transport::UdsMessageConstPtr message);

  // Function to get the channel context
  auto GetChannelState() noexcept -> udpChannelStateImpl::UdpChannelStateImpl & { return udp_channel_state_; }

  // Function to add job to executor
  void SendVehicleInformationToUser() noexcept;

  // Function to get the sync timer
  auto GetSyncTimer() noexcept -> SyncTimer & { return sync_timer_; }

 private:
  // udp transport handler ref
  udp_transport::UdpTransportHandler &udp_transport_handler_;
  // udp socket handler broadcast
  std::unique_ptr<udpSocket::UdpSocketHandler> udp_socket_handler_bcast_;
  // udp socket handler unicast
  std::unique_ptr<udpSocket::UdpSocketHandler> udp_socket_handler_ucast_;
  // udp channel state
  udpChannelStateImpl::UdpChannelStateImpl udp_channel_state_;
  // tcp channel handler
  udpChannelHandlerImpl::UdpChannelHandlerImpl udp_channel_handler_;
  // Executor
  TaskExecutor task_executor_;
  // sync timer
  SyncTimer sync_timer_;
};
}  // namespace udpChannel
}  // namespace doip_client

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_CHANNEL_UDP_CHANNEL_H
