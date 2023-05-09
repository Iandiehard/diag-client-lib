/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/udp_channel.h"

#include "common/logger.h"
#include "handler/udp_transport_handler.h"
#include "sockets/udp_socket_handler.h"

namespace doip_client {
namespace udpChannel {
/*
 @ Class Name        : udpChannel
 @ Class Description : Class used to handle Doip Tcp Channel
 */
//ctor
UdpChannel::UdpChannel(std::string_view local_ip_address, uint16_t port_num,
                       udpTransport::UdpTransportHandler &udp_transport_handler)
    : udp_transport_handler_{udp_transport_handler},
      udp_socket_handler_bcast_{std::make_unique<udpSocket::UdpSocketHandler>(
          local_ip_address, port_num, udpSocket::UdpSocketHandler::PortType::kUdp_Broadcast, *this)},
      udp_socket_handler_ucast_{std::make_unique<udpSocket::UdpSocketHandler>(
          local_ip_address, port_num, udpSocket::UdpSocketHandler::PortType::kUdp_Unicast, *this)},
      udp_channel_handler_{*(udp_socket_handler_bcast_), *(udp_socket_handler_ucast_), udp_transport_handler_, *this} {}

// Initialize the udp channel
uds_transport::UdsTransportProtocolHandler::InitializationResult UdpChannel::Initialize() {
  uds_transport::UdsTransportProtocolHandler::InitializationResult ret_val =
      uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk;
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

uds_transport::UdsTransportProtocolMgr::TransmissionResult UdpChannel::Transmit(
    uds_transport::UdsMessageConstPtr message) {
  return udp_channel_handler_.Transmit(std::move(message));
}

void UdpChannel::WaitForResponse(std::function<void()> timeout_func, std::function<void()> cancel_func, int msec) {
  if (sync_timer_.Start(std::chrono::milliseconds(msec)) == SyncTimerState::kTimeout) {
    timeout_func();
  } else {
    cancel_func();
  }
}

void UdpChannel::WaitCancel() { sync_timer_.Stop(); }
}  // namespace udpChannel
}  // namespace doip_client
