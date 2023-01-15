/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_H
//includes
#include <functional>

#include "channel/udp_channel_handler_impl.h"
#include "channel/udp_channel_state_impl.h"
#include "common/common_doip_types.h"
#include "libTimer/oneShotSync/one_shotsync_timer.h"
#include "sockets/udp_socket_handler.h"
#include "utility/executor.h"

namespace ara {
namespace diag {
namespace doip {
// Forward declaration
namespace udpTransport {
class UdpTransportHandler;
}

namespace udpChannel {
//typedefs
using UdpMessagePtr = ara::diag::doip::udpSocket::UdpMessagePtr;
using SyncTimer = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer;
using SyncTimerState = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer::timer_state;
using TaskExecutor = libUtility::executor::Executor<std::function<void(void)>>;

/*
 @ Class Name        : UdpChannel
 @ Class Description : Class used to handle Doip Udp Channel
 */
class UdpChannel {
public:
  //ctor
  UdpChannel(
    kDoip_String &local_ip_address,
    uint16_t port_num,
    ara::diag::doip::udpTransport::UdpTransportHandler &udp_transport_handler);
  //dtor
  ~UdpChannel() = default;
  // Initialize
  ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();
  //Start
  void Start();
  // Stop
  void Stop();
  // function to handle read broadcast
  void HandleMessageBroadcast(UdpMessagePtr udp_rx_message);
  // function to handle read unicast
  void HandleMessageUnicast(UdpMessagePtr udp_rx_message);
  // Function to trigger transmission of vehicle identification request
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
  Transmit(ara::diag::uds_transport::UdsMessageConstPtr message);

  // Function to get the channel context
  auto GetChannelState() noexcept -> udpChannelStateImpl::UdpChannelStateImpl & {
    return udp_channel_state_;
  }

  // Function to add job to executor
  auto SendVehicleInformationToUser() noexcept -> void;
  // Function to wait for response
  void WaitForResponse(std::function<void()> timeout_func,
                       std::function<void()> cancel_func,
                       int msec);
  // Function to cancel the synchronous wait
  void WaitCancel();

private:
  // udp transport handler ref
  ara::diag::doip::udpTransport::UdpTransportHandler &udp_transport_handler_;
  // udp socket handler broadcast
  std::unique_ptr<ara::diag::doip::udpSocket::UdpSocketHandler> udp_socket_handler_bcast_;
  // udp socket handler unicast
  std::unique_ptr<ara::diag::doip::udpSocket::UdpSocketHandler> udp_socket_handler_ucast_;
  // udp channel state
  udpChannelStateImpl::UdpChannelStateImpl udp_channel_state_;
  // tcp channel handler
  udpChannelHandlerImpl::UdpChannelHandlerImpl udp_channel_handler_;
  // Executor
  TaskExecutor task_executor_;
  // sync timer
  SyncTimer sync_timer_;
  // Declare dlt logging context
  DLT_DECLARE_CONTEXT(doip_udp_channel);
};
}  // namespace udpChannel
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_UDP_CHANNEL_H
