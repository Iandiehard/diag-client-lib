/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "doip_handler/doip_udp_handler.h"
#include "doip_handler/common_doip_types.h"

namespace ara {
namespace diag {
namespace doip {

DoipUdpHandler::DoipUdpHandler(ip_address local_udp_address, uint16_t udp_port_num)
    : udp_socket_handler_unicast_{
        local_udp_address,
        udp_port_num,
        udpSocket::DoipUdpSocketHandler::PortType::kUdp_Unicast,
        [this](UdpMessagePtr udp_rx_message) {
          ProcessUdpUnicastMessage(std::move(udp_rx_message));
        }},
      udp_socket_handler_broadcast_{
        local_udp_address,
        udp_port_num,
        udpSocket::DoipUdpSocketHandler::PortType::kUdp_Broadcast,
        [this](UdpMessagePtr udp_rx_message) {
          ProcessUdpUnicastMessage(std::move(udp_rx_message));
        }} {}

void DoipUdpHandler::Initialize() {
  udp_socket_handler_unicast_.Start();
  udp_socket_handler_broadcast_.Start();
}

void DoipUdpHandler::DeInitialize() {
  udp_socket_handler_unicast_.Stop();
  udp_socket_handler_broadcast_.Stop();
}

void DoipUdpHandler::ProcessUdpUnicastMessage(UdpMessagePtr udp_rx_message) {
  (void)(udp_rx_message);
}

auto DoipUdpHandler::ProcessDoIPHeader(
  DoipMessage &doip_rx_message,
  uint8_t &nackCode) noexcept -> bool {
  bool ret_val{false};
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.protocol_version == kDoip_ProtocolVersion) &&
       (doip_rx_message.protocol_version_inv == (uint8_t) (~(kDoip_ProtocolVersion)))) ||
      ((doip_rx_message.protocol_version == kDoip_ProtocolVersion_Def) &&
       (doip_rx_message.protocol_version_inv == (uint8_t) (~(kDoip_ProtocolVersion_Def))))) {
    /* Check the supported payload type */
    if ((doip_rx_message.payload_type == kDoip_RoutingActivation_ResType) ||
        (doip_rx_message.payload_type == kDoip_DiagMessagePosAck_Type) ||
        (doip_rx_message.payload_type == kDoip_DiagMessageNegAck_Type) ||
        (doip_rx_message.payload_type == kDoip_DiagMessage_Type) ||
        (doip_rx_message.payload_type == kDoip_AliveCheck_ReqType)) {
      if (doip_rx_message.payload_length <= kDoip_Protocol_MaxPayload) {
        if (doip_rx_message.payload_length <= kUdpChannelLength) {
          if (ProcessDoIPPayloadLength(
                doip_rx_message.payload_length, doip_rx_message.payload_type)) {
            ret_val = true;
          } else {
            // Send NACK code 0x04, close the socket
            nackCode = kDoip_GenericHeader_InvalidPayloadLen;
            // socket closure ??
          }
        } else {
          // Send NACK code 0x03, discard message
          nackCode = kDoip_GenericHeader_OutOfMemory;
        }
      } else {
        // Send NACK code 0x02, discard message
        nackCode = kDoip_GenericHeader_MessageTooLarge;
      }
    } else {  // Send NACK code 0x01, discard message
      nackCode = kDoip_GenericHeader_UnknownPayload;
    }
  } else {  // Send NACK code 0x00, close the socket
    nackCode = kDoip_GenericHeader_IncorrectPattern;
    // socket closure
  }
  return ret_val;
}

auto DoipUdpHandler::ProcessDoIPPayloadLength(
  uint32_t payload_len,
  uint16_t payload_type) noexcept -> bool {
  bool ret_val{false};
  switch (payload_type) {
    case kDoip_VehicleAnnouncement_ResType: {
      if (payload_len <= (uint32_t) kDoip_VehicleAnnouncement_ResMaxLen)
        ret_val = true;
      break;
    }
    default:
      // do nothing
      break;
  }
  return ret_val;
}

auto DoipUdpHandler::GetDoIPPayloadType(
  std::vector<uint8_t> payload) noexcept -> uint16_t {
  return ((uint16_t) (((payload[BYTE_POS_TWO] & 0xFF) << 8) | (payload[BYTE_POS_THREE] & 0xFF)));
}

auto DoipUdpHandler::GetDoIPPayloadLength(
  std::vector<uint8_t> payload) noexcept -> uint32_t {
  return ((uint32_t) ((payload[BYTE_POS_FOUR] << 24) & 0xFF000000) |
          (uint32_t) ((payload[BYTE_POS_FIVE] << 16) & 0x00FF0000) |
          (uint32_t) ((payload[BYTE_POS_SIX] << 8) & 0x0000FF00) |
          (uint32_t) ((payload[BYTE_POS_SEVEN] & 0x000000FF)));
}

}  // namespace doip
}  // namespace diag
}  // namespace ara