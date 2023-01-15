/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip_handler/udp_socket_handler.h"

namespace ara {
namespace diag {
namespace doip {
namespace udpSocket {

UdpSocketHandler::UdpSocketHandler(
  kDoip_String &local_ip_address,
  uint16_t port_num,
  PortType port_type,
  ara::diag::doip::udpChannel::UdpChannel &channel)
  : local_ip_address_{local_ip_address},
    port_num_{port_num},
    port_type_{port_type},
    channel_{channel} {
  // create sockets and start receiving
  if (port_type == UdpSocket::PortType::kUdp_Broadcast) {
    udp_socket_ = std::make_unique<UdpSocket>(
      local_ip_address_,
      port_num_,
      port_type_,
      [this](UdpMessagePtr udp_rx_message) {
        //channel_.HandleMessageBroadcast(std::move(udp_rx_message));
      });
  } else {
    udp_socket_ = std::make_unique<UdpSocket>(
      local_ip_address_,
      port_num_,
      port_type_,
      [this](UdpMessagePtr udp_rx_message) {
        //channel_.HandleMessageUnicast(std::move(udp_rx_message));
      });
  }
}

void UdpSocketHandler::Start() {
  udp_socket_->Open();
}

void UdpSocketHandler::Stop() {
  udp_socket_->Destroy();
}

bool UdpSocketHandler::Transmit(UdpMessageConstPtr udpTxMessage) {
  return (udp_socket_->Transmit(std::move(udpTxMessage)));
}

} // udpSocket
} // doip
} // diag
} // ara
