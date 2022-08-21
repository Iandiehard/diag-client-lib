/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/udp_Channel.h"
#include "sockets/udp_SocketHandler.h"
#include "handler/udp_TransportHandler.h"

namespace ara{
namespace diag{
namespace doip{
namespace udpChannel{

/*
 @ Class Name        : udpChannel
 @ Class Description : Class used to handle Doip Tcp Channel
 */

//ctor
udpChannel::udpChannel(kDoip_String &localIpaddress, uint16_t portNum, ara::diag::doip::udpTransport::udp_TransportHandler& udpTransport_Handler)
           : udpTransport_Handler_e(udpTransport_Handler),
             udpSocket_Handler_bcast(std::make_unique<ara::diag::doip::udpSocket::udp_SocketHandler>(localIpaddress, portNum, udpSocket::udp_SocketHandler::PortType::kUdp_Broadcast, *this)),
             udpSocket_Handler_ucast(std::make_unique<ara::diag::doip::udpSocket::udp_SocketHandler>(localIpaddress, portNum, udpSocket::udp_SocketHandler::PortType::kUdp_Unicast, *this)),
             timer_bcast(std::make_unique<udpChanlTimer>()),
             timer_ucast(std::make_unique<udpChanlTimer>()) {
}
//dtor
udpChannel::~udpChannel() {
}

// Initialize the udp channel
uds_transport::UdsTransportProtocolHandler::InitializationResult udpChannel::Initialize() {
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult RetVal =                    \
            ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk;
    return RetVal;
}

//start
void udpChannel::Start() {
    udpSocket_Handler_bcast->Start();
    udpSocket_Handler_ucast->Start();
    // change the udp chnnel state here based on checkbox(skip discovery phase)
    udpChannleState_bcast = udpChannelState::kWaitforVehicleAnnouncement;
    //start A_DoIP_Ctrl = 2s timer here
    //timer_bcast->Start(kDoip_A_DoIP_Ctrl, std::bind(&udpChannel::DoIP_CtrlTimeout, this));
}

//stop
void udpChannel::Stop() {
    udpSocket_Handler_bcast->Stop();
    udpSocket_Handler_ucast->Stop();
}

// Function to handle the reception of Vehicle announcement
// @param input  : UdpMessage udpRxMessage
// @return value : void
void udpChannel::HandleMessageBroadcast(UdpMessagePtr udpRxMessage) {
    // apply mutex
    if(udpChannleState_bcast == udpChannelState::kWaitforVehicleAnnouncement)
    {
        ProcessVehicleIdentificationResponse(std::move(udpRxMessage));
    }
    else
    {//ignore
    }
}

// Function to handle the reception of Vehicle Identification response
// @param input  : UdpMessage udpRxMessage
// @return value : void
void udpChannel::HandleMessageUnicast(UdpMessagePtr udpRxMessage) {
    // apply mutex
    if(udpChannleState_ucast == udpChannelState::kWaitforVehicleIdentificationRes)
    {// store the vehicle identification response
        ProcessVehicleIdentificationResponse(std::move(udpRxMessage));
    }
}

// Function to indicate a message start
void udpChannel::IndicateMessage(UdpMessage &udpRxMessage) {
}

// Function to notify failure
void udpChannel::NotifyMessageFailure() {
}

// Function to trigger transmission
bool udpChannel::Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) {
    //check the type of request
    Std_ReturnType retval   = E_NOT_OK;
    bool           retval_b = false;
    // Check if the channel is free
   if(udpChannleState_ucast == udpChannelState::kIdle)
   {
        udpChannleState_ucast = udpChannelState::kSendVehicleIdentificationReq;
        switch (vehicleInfo_Ref.vehInfoType) {
            case ara::diag::doip::VehicleInfo::VehicleInfoType::kVehicleIdentificationReq :
                {
                    retval = SendVehicleIdentificationRequest(vehicleInfo_Ref);
                    break;
                }
            case ara::diag::doip::VehicleInfo::VehicleInfoType::kVehicleIdentificationReqEID :
                {
                    retval = SendVehicleIdentificationRequest_EID(vehicleInfo_Ref);
                    break;
                }
            case ara::diag::doip::VehicleInfo::VehicleInfoType::kVehicleIdentificationReqVIN :
                {
                    retval = SendVehicleIdentificationRequest_VIN(vehicleInfo_Ref);
                    break;
                }
            default:
                {// nothing
                    break;
                }
        }
        if(retval == E_NOT_OK) {
            // transmission failed , free the channel
            udpChannleState_ucast = udpChannelState::kIdle;
        }
        else if(retval == E_OK) {
            retval_b = true;
        }
   }
   else {
       // do nothing , channel not free      
   }
   return retval_b;
}

// Function to confirm transmission of Vehicle Identification request
// @param input  : void
// @return value : void
void udpChannel::TransmitConfirmation(bool result) {
    if(udpChannleState_ucast == udpChannelState::kSendVehicleIdentificationReq) {
        if(result != false) {
            // success
            udpChannleState_ucast = udpChannelState::kWaitforVehicleIdentificationRes;
            //start A_DoIP_Ctrl = 2s timer here
            //timer_ucast->Start(kDoip_A_DoIP_Ctrl, std::bind(&udpChannel::DoIP_CtrlTimeout, this));
        }
        else {
            // failure
            udpChannleState_ucast = udpChannelState::kIdle;
        }
        udpTransport_Handler_e.TransmitConfirmation(result);
    }
}

// Function to trigger Vehicle Identification req
// @param input  : doipHeader, payloadType, payloadLen
// @return value : void
Std_ReturnType udpChannel::SendVehicleIdentificationRequest(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) {
    Std_ReturnType      retval = E_NOT_OK;
    UdpMessagePtr       doipVehIdenReq = std::make_unique<UdpMessage>(); 
    // to remove warnings
    (void)vehicleInfo_Ref;
    //reserve 8 bytes in vector
    doipVehIdenReq->txBuffer.reserve(kDoipheadrSize + kDoip_VehicleIdentification_ReqLen);
    // create header
    CreateDoIPGenericHeader(doipVehIdenReq->txBuffer, kDoip_VehicleIdentification_ReqType, kDoip_VehicleIdentification_ReqLen);
    //
    doipVehIdenReq->hostIpAddress = vehicleInfo_Ref.hostIpAddress;
    //
    doipVehIdenReq->hostportNum = vehicleInfo_Ref.hostPortNum;
    // transmit
    if(udpSocket_Handler_ucast->Transmit(std::move(doipVehIdenReq))) {
        // success
        retval = E_OK;
    }
    return retval;
}

// Function to trigger Vehicle Identification req with EID
// @param input  : doipHeader, payloadType, payloadLen
// @return value : void
Std_ReturnType udpChannel::SendVehicleIdentificationRequest_EID(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) {
    Std_ReturnType      retval = E_NOT_OK;
    UdpMessagePtr       doipVehIdenEIDReq = std::make_unique<UdpMessage>();
    //reserve 9 bytes in vector
    doipVehIdenEIDReq->txBuffer.reserve(kDoipheadrSize + kDoip_VehicleIdentificationEID_ReqLen);
    // create header
    CreateDoIPGenericHeader(doipVehIdenEIDReq->txBuffer, kDoip_VehicleIdentificationEID_ReqType, kDoip_VehicleIdentificationEID_ReqLen);
    // fill the eid
    doipVehIdenEIDReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.eid[0]);
    doipVehIdenEIDReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.eid[1]);
    doipVehIdenEIDReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.eid[2]);
    doipVehIdenEIDReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.eid[3]);
    doipVehIdenEIDReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.eid[4]);
    doipVehIdenEIDReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.eid[5]);
    // transmit
    if(udpSocket_Handler_ucast->Transmit(std::move(doipVehIdenEIDReq))) {
        // success
        retval = E_OK;
    }
    return retval;
}

// Function to trigger Vehicle Identification req with VIN
// @param input  : doipHeader, payloadType, payloadLen
// @return value : void
Std_ReturnType udpChannel::SendVehicleIdentificationRequest_VIN(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) {
    Std_ReturnType      retval = E_NOT_OK;
    UdpMessagePtr       doipVehIdenVINReq = std::make_unique<UdpMessage>();
    //reserve 9 bytes in vector
    doipVehIdenVINReq->txBuffer.reserve(kDoipheadrSize + kDoip_VehicleIdentificationVIN_ReqLen);
    // create header
    CreateDoIPGenericHeader(doipVehIdenVINReq->txBuffer, kDoip_VehicleIdentificationVIN_ReqType, kDoip_VehicleIdentificationVIN_ReqLen);
    // set host ipadrress
    doipVehIdenVINReq->hostIpAddress = vehicleInfo_Ref.hostIpAddress;
    // set host port number
    doipVehIdenVINReq->hostportNum = vehicleInfo_Ref.hostPortNum;
    // fill the VIN
    for(uint8_t i = 0; i < vehicleInfo_Ref.vin.size(); i ++) {
        doipVehIdenVINReq->txBuffer.push_back((uint8_t)vehicleInfo_Ref.vin[i]);
    }
    // transmit
    if(udpSocket_Handler_ucast->Transmit(std::move(doipVehIdenVINReq))) {
        // success
        retval = E_OK;
    }
    return retval;
}

// Function to process Vehicle Identification response 
// @param input  : UdpMessage udpRxMessage
// @return value : void
void udpChannel::ProcessVehicleIdentificationResponse(UdpMessagePtr udpRxMessage) {
    uint8_t nackCode;
    ara::diag::doip::VehicleInfo vehInfo;
    // Process the Doip header check
    if(ProcessDoIPHeader(udpRxMessage->rxBuffer, nackCode)) {
        // Copy the VIN
        for(uint8_t i = 0; i < vehInfo.vin.size(); i ++)
        {
            vehInfo.vin[i] = (uint8_t)(udpRxMessage->rxBuffer[kDoipheadrSize + i]);
        }
        // Copy the Logical address
        vehInfo.logicalAddress = (uint16_t)(((uint8_t)(udpRxMessage->rxBuffer.at(25)) << 8) | ((uint8_t)(udpRxMessage->rxBuffer.at(26)))) ;
        // Copy the eid
        for(uint8_t i = 0; i < vehInfo.eid.size(); i ++)
        {
            vehInfo.eid[i] = (uint8_t)(udpRxMessage->rxBuffer[27 + i]);
        }
        // Copy the gid
        for(uint8_t i = 0; i < vehInfo.gid.size(); i ++)
        {
            vehInfo.eid[i] = (uint8_t)(udpRxMessage->rxBuffer[33 + i]);
        }
        // Copy the further action byte
        vehInfo.furtherActionByte = udpRxMessage->rxBuffer.at(39);
        // Copy the sender address
        vehInfo.hostIpAddress = udpRxMessage->hostIpAddress;
        // push to vector after checking duplicacy
        vehicle_info.push_back(vehInfo);
    }
    else {
        // send NACK or ignore
        SendDoIPNACKMessage(nackCode);
    }
}

// Function triggered after Doip Ctrl timeout
// @param input  : void
// @return value : void
void udpChannel::DoIP_CtrlTimeout(void) {
    // for broadcast
    switch(udpChannleState_bcast) {
        case udpChannelState::kWaitforVehicleAnnouncement:
        {//change the state to timeout
            udpChannleState_bcast = udpChannelState::kA_DoIP_Ctrl_Timeout;
            if(vehicle_info.size() != 0)
            {
                // Check for VIN/GID sync status

                // send info to upper layer
                udpTransport_Handler_e.Indicate(vehicle_info);
            }
            break;
        }
        default:
            // do nothing
            break;
    }
    // for unicast
    switch(udpChannleState_ucast) {
        case udpChannelState::kWaitforVehicleIdentificationRes:
        {
            udpChannleState_ucast = udpChannelState::kA_DoIP_Ctrl_Timeout;
            if(vehicle_info.size() != 0)
            {
                // Check for VIN/GID sync status

                // send info to upper layer
                udpTransport_Handler_e.Indicate(vehicle_info);
            }
            break;
        }
        default:
            // do nothing
            break;
    }
}

// Description   : Function to create Generic Header
// @param input  : doipHeader, payloadType, payloadLen
// @return value : void
void udpChannel::CreateDoIPGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType, uint32_t payloadLen) {
    doipHeader.push_back(kDoip_ProtocolVersion);
    doipHeader.push_back(~((uint8_t)kDoip_ProtocolVersion));
    doipHeader.push_back((uint8_t)((payloadType & 0xFF00) >> 8));
    doipHeader.push_back((uint8_t)(payloadType & 0x00FF));
    doipHeader.push_back((uint8_t)((payloadLen & 0xFF000000) >> 24));
    doipHeader.push_back((uint8_t)((payloadLen & 0x00FF0000) >> 16));
    doipHeader.push_back((uint8_t)((payloadLen & 0x0000FF00) >> 8));
    doipHeader.push_back((uint8_t)(payloadLen & 0x000000FF));
}

// Description   : Function to send NACK
// @param input  : nackType
// @return value : boolean true/false
bool udpChannel::SendDoIPNACKMessage(uint8_t nackType) {
    UdpMessagePtr       doipNackMessage = std::make_unique<UdpMessage>();
    //reserve 9 bytes in vector
    doipNackMessage->txBuffer.reserve(kDoipheadrSize + kDoip_GenericHeader_NackLen);
    // create header
    CreateDoIPGenericHeader(doipNackMessage->txBuffer, kDoip_GenericHeadr_NackType, kDoip_GenericHeader_NackLen);
    // fill the nack code
    doipNackMessage->txBuffer.push_back(nackType);
    // transmit
    if(!(udpSocket_Handler_ucast->Transmit(std::move(doipNackMessage)))) {
        // error
    }
    return true;
}

// Description   : Function to process Generic Doip Header received
// @param input  : doipHeader , nackCode
// @return value : boolean true/false
bool udpChannel::ProcessDoIPHeader(std::vector<uint8_t> &doipHeader, uint8_t &nackCode) {
    uint16_t PayloadType;
    uint32_t PayloadLength;
    bool RetVal = false;
    /* Check the header synchronisation pattern */
    if (((doipHeader[0] == kDoip_ProtocolVersion) && (doipHeader[1] == (uint8_t)(~(kDoip_ProtocolVersion)))) ||
        ((doipHeader[0] == kDoip_ProtocolVersion_Def) && (doipHeader[1] == (uint8_t)(~(kDoip_ProtocolVersion_Def))))) {
        /* get the payload type */
        PayloadType = (uint16_t)(((doipHeader[2] & 0xFF) << 8) | (doipHeader[3] & 0xFF));
        /* Check the supported payload type */
        if (PayloadType == kDoip_VehicleAnnouncement_ResType) {
            PayloadLength = (uint32_t)((doipHeader[4] << 24) & 0xFF000000) | (uint32_t)((doipHeader[5] << 16) & 0x00FF0000) | \
                            (uint32_t)((doipHeader[6] <<  8) & 0x0000FF00) | (uint32_t)((doipHeader[7] & 0x000000FF));

            /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00017] */
            if (PayloadLength <= kDoip_Protocol_MaxPayload) {
                /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00018] */
                if (PayloadLength <= kUdpChannelLength) {
                    /* Req-[AUTOSAR_SWS_DiagnosticOverIP][SWS_DoIP_00019] */
                    if (PayloadLength <= (uint32_t)kDoip_VehicleAnnouncement_ResMaxLen) {
                        // allow 32 and 33 bytes payload, considering VIN/GID sync status optional
                        RetVal = true;
                    }
                    else {// Send NACK code 0x04, close the socket
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
        else {
            // Send NACK code 0x01, discard message
            nackCode = kDoip_GenericHeader_UnknownPayload;
        }
    }
    else {
        // Send NACK code 0x00, close the socket
        nackCode = kDoip_GenericHeader_IncorrectPattern;
        // socket closure
    }
    return RetVal;
}

// Check for duplicate VehInfo, if matches return true else false
bool udpChannel::CheckForDuplicateVehicleInfo(VehicleInfo &vehicleInfo_Ref)
{
    bool retval = false;
    //vin invalidity
    std::array<uint8_t, kDoip_VIN_Size> vin_invalid_FF{kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, \
                                                       kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, \
                                                       kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, \
                                                       kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, \
                                                       kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF, \
                                                       kDoip_VIN_Invalid_FF, kDoip_VIN_Invalid_FF};
    std::array<uint8_t, kDoip_VIN_Size> vin_invalid_00{kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, \
                                                       kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, \
                                                       kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, \
                                                       kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, \
                                                       kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00, \
                                                       kDoip_VIN_Invalid_00, kDoip_VIN_Invalid_00};

    //GID invalidity
    std::array<uint8_t, kDoip_EID_GID_Size> gid_invalid_FF{kDoip_GID_Invalid_FF, kDoip_GID_Invalid_FF, kDoip_GID_Invalid_FF,
                                                           kDoip_GID_Invalid_FF, kDoip_GID_Invalid_FF, kDoip_GID_Invalid_FF};
    std::array<uint8_t, kDoip_EID_GID_Size> gid_invalid_00{kDoip_GID_Invalid_00, kDoip_GID_Invalid_00, kDoip_GID_Invalid_00,
                                                           kDoip_GID_Invalid_00, kDoip_GID_Invalid_00, kDoip_GID_Invalid_00};
    //EID invalidity
    std::array<uint8_t, kDoip_EID_GID_Size> eid_invalid_FF{kDoip_EID_Invalid_FF, kDoip_EID_Invalid_FF, kDoip_EID_Invalid_FF,
                                                           kDoip_EID_Invalid_FF, kDoip_EID_Invalid_FF, kDoip_EID_Invalid_FF};
    std::array<uint8_t, kDoip_EID_GID_Size> eid_invalid_00{kDoip_EID_Invalid_00, kDoip_EID_Invalid_00, kDoip_EID_Invalid_00,
                                                           kDoip_EID_Invalid_00, kDoip_EID_Invalid_00, kDoip_EID_Invalid_00};

    //check for invalid vin
    if(!((vehicleInfo_Ref.vin == vin_invalid_FF) || (vehicleInfo_Ref.vin == vin_invalid_00))) {
        // is not invalid
        //search for duplicate vin in vector veh info
    }
    else if(!((vehicleInfo_Ref.gid == gid_invalid_FF) || (vehicleInfo_Ref.gid == gid_invalid_00))) {

    }
    else if(!((vehicleInfo_Ref.eid == eid_invalid_FF) || (vehicleInfo_Ref.eid == eid_invalid_00))) {

    }
    else {
        //vin, eid, gid all are invalid
    }
    return retval;
}

} // udpChannel
} // doip
} // diag
} // ara
