/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "sockets/udp_socket_handler.h"

#include "channel/udp_channel/doip_udp_channel.h"
#include "error_domain/doip_error_domain.h"

namespace doip_client {
namespace sockets {
UdpSocketHandler::UdpSocketHandler(std::string_view local_ip_address, std::uint16_t port_num, PortType port_type,
                                   DoipUdpChannel &channel)
    : local_ip_address_{local_ip_address},
      local_port_num_{port_num},
      port_type_{port_type},
      channel_{channel} {
  // create sockets and start receiving
  if (port_type == UdpSocket::PortType::kUdp_Broadcast) {
    udp_socket_ = std::make_unique<UdpSocket>(
        local_ip_address_, local_port_num_, port_type_,
        [this](UdpMessagePtr udp_rx_message) { channel_.ProcessReceivedUdpBroadcast(std::move(udp_rx_message)); });
  } else {
    udp_socket_ = std::make_unique<UdpSocket>(
        local_ip_address_, local_port_num_, port_type_,
        [this](UdpMessagePtr udp_rx_message) { channel_.ProcessReceivedUdpUnicast(std::move(udp_rx_message)); });
  }
}

void UdpSocketHandler::Start() { udp_socket_->Open(); }

void UdpSocketHandler::Stop() { udp_socket_->Destroy(); }

core_type::Result<void> UdpSocketHandler::Transmit(UdpMessageConstPtr udp_message) {
  core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::DoipErrorErrc::kGenericError)};
  if (udp_socket_->Transmit(std::move(udp_message)).HasValue()) { result.EmplaceValue(); }
  return result;
}
}  // namespace sockets
}  // namespace doip_client
