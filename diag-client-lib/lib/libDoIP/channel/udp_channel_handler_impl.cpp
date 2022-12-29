/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/udp_channel_handler_impl.h"
#include "channel/udp_channel.h"
#include "handler/udp_transport_handler.h"

namespace ara {
namespace diag {
namespace doip {
namespace udpChannelHandlerImpl {

auto VehicleDiscoveryHandler::ProcessVehicleAnnouncementResponse(
  DoipMessage &doip_payload) noexcept -> void {
  
  if(channel_
      .GetChannelState()
      .GetVehicleDiscoveryStateContext()
      .GetActiveState()
      .GetState() == UdpVehicleDiscoveryState::kWaitForVehicleAnnouncement) {
    // Deserialize and Add to job executor
    
  }
  else {
    // ignore
  }
}

auto VehicleDiscoveryHandler::ProcessVehicleIdentificationResponse(
  DoipMessage &doip_payload) noexcept -> void {
  
  if(channel_
       .GetChannelState()
       .GetVehicleIdentificationStateContext()
       .GetActiveState()
       .GetState() == UdpVehicleIdentificationState::kViWaitForVehicleIdentificationRes) {
    
    // Deserialize data to indicate to upper layer
    std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>
      ret_val{udp_transport_handler_.IndicateMessage(
      ara::diag::uds_transport::UdsMessage::Address(0U),
      ara::diag::uds_transport::UdsMessage::Address(0U),
      ara::diag::uds_transport::UdsMessage::TargetAddressType::kPhysical,
      0,
      std::size_t(doip_payload.payload.size()),
      0,
      "DoIPUdp",
      doip_payload.payload)};
    
    if((ret_val.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
      (ret_val.second != nullptr)) {
      // Add meta info about ip address
      uds_transport::UdsMessage::MetaInfoMap  meta_info_map{
        {"kRemoteIpAddress", doip_payload.host_ip_address}
      };
      ret_val.second->AddMetaInfo(std::move(
        std::make_shared<uds_transport::UdsMessage::MetaInfoMap>(meta_info_map)));
      
      // copy to application buffer
      std::copy(doip_payload.payload.begin(),doip_payload.payload.end(),
                ret_val.second->GetPayload().begin());
      udp_transport_handler_.HandleMessage(std::move(ret_val.second));
    }
  }
  else {
    // ignore
  }
}

auto VehicleDiscoveryHandler::SendVehicleIdentificationRequest(
  uds_transport::UdsMessageConstPtr &message)
  noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult
    ret_val{uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  if(channel_
    .GetChannelState()
    .GetVehicleIdentificationStateContext()
    .GetActiveState()
    .GetState() == UdpVehicleIdentificationState::kViIdle) {
    if(HandleVehicleIdentificationRequest(message) ==
       ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      
      channel_.GetChannelState().GetVehicleIdentificationStateContext()
        .TransitionTo(UdpVehicleIdentificationState::kViWaitForVehicleIdentificationRes);
      
      // Wait for 2 sec to collect all the vehicle identification response
      channel_.WaitForResponse(
        [&]() {
          channel_.GetChannelState().GetVehicleIdentificationStateContext()
            .TransitionTo(UdpVehicleIdentificationState::kViDoIPCtrlTimeout);
        },
        [&](){
          // do nothing
        },
        kDoIPCtrl
        );
      channel_.GetChannelState().GetVehicleIdentificationStateContext()
        .TransitionTo(UdpVehicleIdentificationState::kViIdle);
    }
    else {
      // failed, do nothing
    }
  }
  else {
    // not free
  }
  return ret_val;
}

auto VehicleDiscoveryHandler::HandleVehicleIdentificationRequest(
  uds_transport::UdsMessageConstPtr &message) noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult
    ret_val{uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  
  UdpMessagePtr doip_vehicle_ident_req{std::make_unique<UdpMessage>()};
  
  // get the payload type & length
  auto doip_vehicle_payload_type{
    GetVehicleIdentificationPayloadType(message->GetPayload()[BYTE_POS_ONE])};
  
  // create header
  doip_vehicle_ident_req->tx_buffer_.reserve(kDoipheadrSize);
  CreateDoipGenericHeader(doip_vehicle_ident_req->tx_buffer_,
                          doip_vehicle_payload_type.first,
                          doip_vehicle_payload_type.second);
  
  // Copy if containing VIN / EID
  if(doip_vehicle_payload_type.first != kDoip_VehicleIdentification_ReqType) {
    doip_vehicle_ident_req->tx_buffer_.insert(
      doip_vehicle_ident_req->tx_buffer_.end(),
      doip_vehicle_payload_type.second,
      message->GetPayload().at(BYTE_POS_TWO));
  }
  
  if(udp_socket_handler_.Transmit(std::move(doip_vehicle_ident_req))) {
    ret_val = uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
  }
  return ret_val;
}

auto VehicleDiscoveryHandler::CreateDoipGenericHeader(
  std::vector<uint8_t> &doipHeader,
  std::uint16_t payloadType,
  std::uint32_t payloadLen) noexcept -> void {
  doipHeader.push_back(kDoip_ProtocolVersion);
  doipHeader.push_back(~((uint8_t)kDoip_ProtocolVersion));
  doipHeader.push_back((uint8_t)((payloadType & 0xFF00) >> 8));
  doipHeader.push_back((uint8_t)(payloadType & 0x00FF));
  doipHeader.push_back((uint8_t)((payloadLen & 0xFF000000) >> 24));
  doipHeader.push_back((uint8_t)((payloadLen & 0x00FF0000) >> 16));
  doipHeader.push_back((uint8_t)((payloadLen & 0x0000FF00) >> 8));
  doipHeader.push_back((uint8_t)(payloadLen & 0x000000FF));
}

auto VehicleDiscoveryHandler::GetVehicleIdentificationPayloadType(
  std::uint8_t preselection_mode ) noexcept -> const std::pair<std::uint16_t, std::uint8_t > {
  std::pair<std::uint16_t, std::uint8_t > ret_val{0,0};
  switch (preselection_mode) {
    case 0U: {
      ret_val.first = kDoip_VehicleIdentification_ReqType;
      ret_val.second = kDoip_VehicleIdentification_ReqLen;
    }
    break;
    case 1U: {
      ret_val.first = kDoip_VehicleIdentificationVIN_ReqType;
      ret_val.second = kDoip_VehicleIdentificationVIN_ReqLen;
    }
    break;
    case 2U: {
      ret_val.first = kDoip_VehicleIdentificationEID_ReqType;
      ret_val.second = kDoip_VehicleIdentificationEID_ReqLen;
    }
    break;
    default:
      break;
  }
  return ret_val;
}

auto UdpChannelHandlerImpl::Transmit(
  uds_transport::UdsMessageConstPtr &message)
  noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult {
  uds_transport::UdsTransportProtocolMgr::TransmissionResult
    ret_val{uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  
  // deserialize and send to proper handler
  switch(message->GetPayload()[BYTE_POS_ZERO]) {
    case 0U: {
      // 0U -> Vehicle Identification Req
      ret_val = vehicle_identification_handler_.SendVehicleIdentificationRequest(message);
    }
    break;
    
    case 1U: {
      // 1U -> Power Mode Req
    }
    break;
    
    default:
      // nothing
      break;
  }
  return ret_val;
}

auto UdpChannelHandlerImpl::HandleMessage(
  UdpMessagePtr udp_rx_message) noexcept -> void {
  uint8_t nack_code;
  DoipMessage doip_rx_message;
  
  doip_rx_message.host_ip_address = udp_rx_message->host_ip_address_;
  doip_rx_message.protocol_version = udp_rx_message->rx_buffer_[0];
  doip_rx_message.protocol_version_inv = udp_rx_message->rx_buffer_[1];
  doip_rx_message.payload_type = GetDoIPPayloadType(udp_rx_message->rx_buffer_);
  doip_rx_message.payload_length = GetDoIPPayloadLength(udp_rx_message->rx_buffer_);
  
  // Process the Doip Generic header check
  if(ProcessDoIPHeader(doip_rx_message, nack_code)) {
    doip_rx_message.payload.resize(udp_rx_message->rx_buffer_.size() - kDoipheadrSize);
    // copy payload locally
    std::copy(udp_rx_message->rx_buffer_.begin() + kDoipheadrSize,
              udp_rx_message->rx_buffer_.begin() + kDoipheadrSize + udp_rx_message->rx_buffer_.size(),
              doip_rx_message.payload.begin());
    ProcessDoIPPayload(doip_rx_message);
  }
  else {
    // send NACK or ignore
    // SendDoIPNACKMessage(nack_code);
    (void)nack_code;
  }
}

auto UdpChannelHandlerImpl::HandleMessageBroadcast(
  UdpMessagePtr udp_rx_message) noexcept -> void {
  uint8_t nack_code;
  DoipMessage doip_rx_message;

  doip_rx_message.protocol_version = udp_rx_message->rx_buffer_[0];
  doip_rx_message.protocol_version_inv = udp_rx_message->rx_buffer_[1];
  doip_rx_message.payload_type = GetDoIPPayloadType(udp_rx_message->rx_buffer_);
  doip_rx_message.payload_length = GetDoIPPayloadLength(udp_rx_message->rx_buffer_);

  // Process the Doip Generic header check
  if(ProcessDoIPHeader(doip_rx_message, nack_code)) {
    doip_rx_message.payload.resize(udp_rx_message->rx_buffer_.size() - kDoipheadrSize);
    // copy payload locally
    std::copy(udp_rx_message->rx_buffer_.begin() + kDoipheadrSize,
      udp_rx_message->rx_buffer_.begin() + kDoipheadrSize + udp_rx_message->rx_buffer_.size(),
      doip_rx_message.payload.begin());
    vehicle_discovery_handler_.ProcessVehicleAnnouncementResponse(doip_rx_message);
  }
  else {
    // send NACK or ignore
    // SendDoIPNACKMessage(nack_code);
    (void)nack_code;
  }
}

auto UdpChannelHandlerImpl::ProcessDoIPHeader(
  DoipMessage &doip_rx_message,
  uint8_t &nackCode) noexcept -> bool {
  bool ret_val{false};
  /* Check the header synchronisation pattern */
  if (((doip_rx_message.protocol_version == kDoip_ProtocolVersion)
       && (doip_rx_message.protocol_version_inv == (uint8_t)(~(kDoip_ProtocolVersion)))) ||
      ((doip_rx_message.protocol_version == kDoip_ProtocolVersion_Def)
       && (doip_rx_message.protocol_version_inv == (uint8_t)(~(kDoip_ProtocolVersion_Def))))) {
    
    /* Check the supported payload type */
    if ((doip_rx_message.payload_type == kDoip_RoutingActivation_ResType) ||
        (doip_rx_message.payload_type == kDoip_DiagMessagePosAck_Type)    ||
        (doip_rx_message.payload_type == kDoip_DiagMessageNegAck_Type)    ||
        (doip_rx_message.payload_type == kDoip_DiagMessage_Type)          ||
        (doip_rx_message.payload_type == kDoip_AliveCheck_ReqType)) {
      
      /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
      if (doip_rx_message.payload_length <= kDoip_Protocol_MaxPayload) {
        /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
        if (doip_rx_message.payload_length <= kUdpChannelLength) {
          /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00019] */
          if (ProcessDoIPPayloadLength(
            doip_rx_message.payload_length, doip_rx_message.payload_type)) {
            ret_val = true;
          }
          else  {
            // Send NACK code 0x04, close the socket
            nackCode = kDoip_GenericHeader_InvalidPayloadLen;
            // socket closure ??
          }
        }
        else {
          // Send NACK code 0x03, discard message
          nackCode = kDoip_GenericHeader_OutOfMemory;
        }
      }
      else {
        // Send NACK code 0x02, discard message
        nackCode = kDoip_GenericHeader_MessageTooLarge;
      }
    }
    else {// Send NACK code 0x01, discard message
      nackCode = kDoip_GenericHeader_UnknownPayload;
    }
  }
  else {// Send NACK code 0x00, close the socket
    nackCode = kDoip_GenericHeader_IncorrectPattern;
    // socket closure
  }
  return ret_val;
}

auto UdpChannelHandlerImpl::ProcessDoIPPayloadLength(
  uint32_t payload_len,
  uint16_t payload_type) noexcept -> bool {
  bool ret_val{false};
  switch(payload_type) {
    case kDoip_VehicleAnnouncement_ResType:
    {
      if(payload_len <= (uint32_t)kDoip_VehicleAnnouncement_ResMaxLen)
        ret_val = true;
      break;
    }
    default:
      // do nothing
      break;
  }
  return ret_val;
}

auto UdpChannelHandlerImpl::GetDoIPPayloadType(
  std::vector<uint8_t> payload) noexcept -> uint16_t {
  return ((uint16_t)(((payload[BYTE_POS_TWO] & 0xFF) << 8) | (payload[BYTE_POS_THREE] & 0xFF)));
}

auto UdpChannelHandlerImpl::GetDoIPPayloadLength(
  std::vector<uint8_t> payload) noexcept -> uint32_t {
  return ((uint32_t)((payload[BYTE_POS_FOUR] << 24) & 0xFF000000) |
    (uint32_t)((payload[BYTE_POS_FIVE] << 16) & 0x00FF0000) |
    (uint32_t)((payload[BYTE_POS_SIX] <<  8) & 0x0000FF00) |
    (uint32_t)((payload[BYTE_POS_SEVEN] & 0x000000FF)));
}

auto UdpChannelHandlerImpl::ProcessDoIPPayload(
  DoipMessage &doip_payload,
  DoipMessage::rx_socket_type socket_type) noexcept -> void {
  std::unique_lock<std::mutex> lck(channel_handler_lock);
  switch(doip_payload.payload_type) {
    case kDoip_VehicleAnnouncement_ResType: {
      vehicle_identification_handler_.ProcessVehicleIdentificationResponse(doip_payload);
      break;
    }
    
    default:
      /* do nothing */
      break;
  }
}
} // udpChannelHandlerImpl
} // doip
} // diag
} // ara