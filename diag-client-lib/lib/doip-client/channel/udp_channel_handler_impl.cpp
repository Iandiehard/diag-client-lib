/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/udp_channel_handler_impl.h"

#include "channel/udp_channel.h"
#include "core/include/span.h"
#include "handler/udp_transport_handler.h"

namespace doip_client {
namespace udpChannelHandlerImpl {
auto VehicleDiscoveryHandler::ProcessVehicleAnnouncementResponse(DoipMessage &doip_payload) noexcept -> void {
  if (channel_.GetChannelState().GetVehicleDiscoveryStateContext().GetActiveState().GetState() ==
      UdpVehicleDiscoveryState::kWaitForVehicleAnnouncement) {
    // Deserialize and Add to job executor
  } else {
    // ignore
  }
}

auto VehicleDiscoveryHandler::ProcessVehicleIdentificationResponse(DoipMessage &doip_payload) noexcept -> void {
  if (channel_.GetChannelState().GetVehicleIdentificationStateContext().GetActiveState().GetState() ==
      UdpVehicleIdentificationState::kViWaitForVehicleIdentificationRes) {
    // Deserialize data to indicate to upper layer
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, uds_transport::UdsMessagePtr> ret_val{
        udp_transport_handler_.IndicateMessage(
            static_cast<uds_transport::UdsMessage::Address>(0U), static_cast<uds_transport::UdsMessage::Address>(0U),
            uds_transport::UdsMessage::TargetAddressType::kPhysical, 0U, doip_payload.GetPayload().size(), 0U,
            "DoIPUdp", doip_payload.GetPayload())};
    if ((ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
        (ret_val.second != nullptr)) {
      // Add meta info about ip address
      uds_transport::UdsMessage::MetaInfoMap meta_info_map{
          {"kRemoteIpAddress", std::string{doip_payload.GetHostIpAddress()}}};
      ret_val.second->AddMetaInfo(std::make_shared<uds_transport::UdsMessage::MetaInfoMap>(meta_info_map));
      // copy to application buffer
      (void) std::copy(doip_payload.GetPayload().begin(), doip_payload.GetPayload().end(),
                       ret_val.second->GetPayload().begin());
      udp_transport_handler_.HandleMessage(std::move(ret_val.second));
    }
  } else {
    // ignore
  }
}

auto VehicleDiscoveryHandler::SendVehicleIdentificationRequest(uds_transport::UdsMessageConstPtr message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  if (channel_.GetChannelState().GetVehicleIdentificationStateContext().GetActiveState().GetState() ==
      UdpVehicleIdentificationState::kViIdle) {
    if (HandleVehicleIdentificationRequest(std::move(message)) ==
        uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;

      channel_.GetChannelState().GetVehicleIdentificationStateContext().TransitionTo(
          UdpVehicleIdentificationState::kViWaitForVehicleIdentificationRes);
      // Wait for 2 sec to collect all the vehicle identification response
      channel_.GetSyncTimer().WaitForTimeout(
          [&]() {
            channel_.GetChannelState().GetVehicleIdentificationStateContext().TransitionTo(
                UdpVehicleIdentificationState::kViDoIPCtrlTimeout);
          },
          [&]() {
            // do nothing
          },
          std::chrono::milliseconds{kDoIPCtrl});
      channel_.GetChannelState().GetVehicleIdentificationStateContext().TransitionTo(
          UdpVehicleIdentificationState::kViIdle);
    } else {
      // failed, do nothing
    }
  } else {
    // not free
  }
  return ret_val;
}

auto VehicleDiscoveryHandler::HandleVehicleIdentificationRequest(uds_transport::UdsMessageConstPtr message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};

  UdpMessagePtr doip_vehicle_ident_req{std::make_unique<UdpMessage>()};
  // Get preselection mode
  std::uint8_t preselection_mode{message->GetPayload()[BYTE_POS_ONE]};

  // get the payload type & length from preselection mode
  VehiclePayloadType doip_vehicle_payload_type{GetVehicleIdentificationPayloadType(preselection_mode)};

  // create header
  CreateDoipGenericHeader(doip_vehicle_ident_req->tx_buffer_, doip_vehicle_payload_type.first,
                          doip_vehicle_payload_type.second);
  // set remote ip
  doip_vehicle_ident_req->host_ip_address_ = message->GetHostIpAddress();

  // set remote port num
  doip_vehicle_ident_req->host_port_num_ = message->GetHostPortNumber();

  // Copy only if containing VIN / EID
  if (doip_vehicle_payload_type.first != kDoip_VehicleIdentification_ReqType) {
    doip_vehicle_ident_req->tx_buffer_.insert(doip_vehicle_ident_req->tx_buffer_.begin() + kDoipheadrSize,
                                              message->GetPayload().begin() + 2U, message->GetPayload().end());
  }
  if (udp_socket_handler_.Transmit(std::move(doip_vehicle_ident_req))) {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

void VehicleDiscoveryHandler::CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader, std::uint16_t payloadType,
                                                      std::uint32_t payloadLen) {
  doipHeader.emplace_back(kDoip_ProtocolVersion);
  doipHeader.emplace_back(~((uint8_t) kDoip_ProtocolVersion));
  doipHeader.emplace_back((uint8_t) ((payloadType & 0xFF00) >> 8));
  doipHeader.emplace_back((uint8_t) (payloadType & 0x00FF));
  doipHeader.emplace_back((uint8_t) ((payloadLen & 0xFF000000) >> 24));
  doipHeader.emplace_back((uint8_t) ((payloadLen & 0x00FF0000) >> 16));
  doipHeader.emplace_back((uint8_t) ((payloadLen & 0x0000FF00) >> 8));
  doipHeader.emplace_back((uint8_t) (payloadLen & 0x000000FF));
}

auto VehicleDiscoveryHandler::GetVehicleIdentificationPayloadType(std::uint8_t preselection_mode) noexcept
    -> VehiclePayloadType {
  VehiclePayloadType ret_val{0, 0};
  switch (preselection_mode) {
    case 0U:
      ret_val.first = kDoip_VehicleIdentification_ReqType;
      ret_val.second = kDoip_VehicleIdentification_ReqLen;
      break;
    case 1U:
      ret_val.first = kDoip_VehicleIdentificationVIN_ReqType;
      ret_val.second = kDoip_VehicleIdentificationVIN_ReqLen;
      break;
    case 2U:
      ret_val.first = kDoip_VehicleIdentificationEID_ReqType;
      ret_val.second = kDoip_VehicleIdentificationEID_ReqLen;
      break;
    default:
      break;
  }
  return ret_val;
}

auto UdpChannelHandlerImpl::Transmit(uds_transport::UdsMessageConstPtr message) noexcept
    -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  // Get the udp handler type from payload
  std::uint8_t handler_type{message->GetPayload()[BYTE_POS_ZERO]};
  // deserialize and send to proper handler
  switch (handler_type) {
    case 0U:
      // 0U -> Vehicle Identification Req
      ret_val = vehicle_identification_handler_.SendVehicleIdentificationRequest(std::move(message));
      break;
    case 1U:
      // 1U -> Power Mode Req
      break;
  }
  return ret_val;
}

auto UdpChannelHandlerImpl::HandleMessage(UdpMessagePtr udp_rx_message) noexcept -> void {
  std::uint8_t nack_code{};
  DoipMessage doip_rx_message{DoipMessage::MessageType::kUdp, udp_rx_message->host_ip_address_,
                              udp_rx_message->host_port_num_,
                              core_type::Span<std::uint8_t>{udp_rx_message->rx_buffer_}};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    ProcessDoIPPayload(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto UdpChannelHandlerImpl::HandleMessageBroadcast(UdpMessagePtr udp_rx_message) noexcept -> void {
  uint8_t nack_code;
  DoipMessage doip_rx_message{DoipMessage::MessageType::kUdp, udp_rx_message->host_ip_address_,
                              udp_rx_message->host_port_num_,
                              core_type::Span<std::uint8_t>{udp_rx_message->rx_buffer_}};
  // Process the Doip Generic header check
  if (ProcessDoIPHeader(doip_rx_message, nack_code)) {
    vehicle_discovery_handler_.ProcessVehicleAnnouncementResponse(doip_rx_message);
  } else {
    // send NACK or ignore
    (void) nack_code;
  }
}

auto UdpChannelHandlerImpl::ProcessDoIPHeader(DoipMessage &doip_rx_message, uint8_t &nackCode) noexcept -> bool {
  bool ret_val{false};
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion) &&
       (doip_rx_message.GetInverseProtocolVersion() == (uint8_t) (~(kDoip_ProtocolVersion)))) ||
      ((doip_rx_message.GetProtocolVersion() == kDoip_ProtocolVersion_Def) &&
       (doip_rx_message.GetInverseProtocolVersion() == (uint8_t) (~(kDoip_ProtocolVersion_Def))))) {
    /* Check the supported payload type */
    if (doip_rx_message.GetPayloadType() == kDoip_VehicleAnnouncement_ResType) {
      /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
      if (doip_rx_message.GetPayloadLength() <= kDoip_Protocol_MaxPayload) {
        /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
        if (doip_rx_message.GetPayloadLength() <= kUdpChannelLength) {
          /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00019] */
          if (ProcessDoIPPayloadLength(doip_rx_message.GetPayloadLength(), doip_rx_message.GetPayloadType())) {
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

auto UdpChannelHandlerImpl::ProcessDoIPPayloadLength(uint32_t payload_len, uint16_t payload_type) noexcept -> bool {
  bool ret_val{false};
  switch (payload_type) {
    case kDoip_VehicleAnnouncement_ResType: {
      if (payload_len <= kDoip_VehicleAnnouncement_ResMaxLen) ret_val = true;
      break;
    }
    default:
      // do nothing
      break;
  }
  return ret_val;
}

void UdpChannelHandlerImpl::ProcessDoIPPayload(DoipMessage &doip_payload, DoipMessage::RxSocketType socket_type) {
  std::lock_guard<std::mutex> const lck(channel_handler_lock);
  switch (doip_payload.GetPayloadType()) {
    case kDoip_VehicleAnnouncement_ResType: {
      vehicle_identification_handler_.ProcessVehicleIdentificationResponse(doip_payload);
      break;
    }
    default:
      /* do nothing */
      break;
  }
}
}  // namespace udpChannelHandlerImpl
}  // namespace doip_client
