/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "channel/udp_channel.h"

#include "handler/udp_transport_handler.h"
#include "sockets/udp_socket_handler.h"

namespace ara {
namespace diag {
namespace doip {
namespace udpChannel {
/*
 @ Class Name        : udpChannel
 @ Class Description : Class used to handle Doip Tcp Channel
 */
//ctor
UdpChannel::UdpChannel(
  kDoip_String &local_ip_address,
  uint16_t port_num,
  ara::diag::doip::udpTransport::UdpTransportHandler &udp_transport_handler)
    : udp_transport_handler_{udp_transport_handler},
      udp_socket_handler_bcast_{
        std::make_unique<ara::diag::doip::udpSocket::UdpSocketHandler>(
          local_ip_address,
          port_num,
          udpSocket::UdpSocketHandler::PortType::kUdp_Broadcast,
          *this)},
      udp_socket_handler_ucast_{
        std::make_unique<ara::diag::doip::udpSocket::UdpSocketHandler>(
          local_ip_address,
          port_num,
          udpSocket::UdpSocketHandler::PortType::kUdp_Unicast,
          *this)},
      udp_channel_handler_{*(udp_socket_handler_bcast_),
                           *(udp_socket_handler_ucast_),
                           udp_transport_handler_, *this} {
  DLT_REGISTER_CONTEXT(doip_udp_channel, "dudp", "DoipClient Udp Channel Context");
}

// Initialize the udp channel
uds_transport::UdsTransportProtocolHandler::InitializationResult
UdpChannel::Initialize() {
  ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult ret_val =
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk;
  return ret_val;
}

// start
void UdpChannel::Start() {
  udp_socket_handler_bcast_->Start();
  udp_socket_handler_ucast_->Start();
}

// stop
void UdpChannel::Stop() {
  udp_socket_handler_bcast_->Stop();
  udp_socket_handler_ucast_->Stop();
}

void UdpChannel::HandleMessageBroadcast(UdpMessagePtr udp_rx_message) {
  udp_channel_handler_.HandleMessageBroadcast(std::move(udp_rx_message));
}

void UdpChannel::HandleMessageUnicast(UdpMessagePtr udp_rx_message) {
  udp_channel_handler_.HandleMessage(std::move(udp_rx_message));
}

ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
UdpChannel::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) {
  return udp_channel_handler_.Transmit(message);
}

void UdpChannel::WaitForResponse(
  std::function<void()> timeout_func,
  std::function<void()> cancel_func,
  int msec) {
  if (sync_timer_.Start(msec) == SyncTimerState::kTimeout) {
    timeout_func();
  } else {
    cancel_func();
  }
}

void UdpChannel::WaitCancel() {
  sync_timer_.Stop();
}
}  // namespace udpChannel
}  // namespace doip
}  // namespace diag
}  // namespace ara
