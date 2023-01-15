/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LIBDOIP_COMMON_COMMON_DOIP_TYPES_H_
#define LIB_LIBDOIP_COMMON_COMMON_DOIP_TYPES_H_
#include "common_doip_header.h"

namespace ara {
namespace diag {
namespace doip {
/* DoIP Port Number - Unsecured */
constexpr uint16_t kDoipPort = 13400U;
/* Udp Channel Length */
constexpr uint32_t kUdpChannelLength = 41U;
/* Tcp Channel Length */
constexpr uint32_t kTcpChannelLength = 4096U;
/* DoIP Header */
constexpr uint8_t kDoipheadrSize = 0x8;
constexpr uint8_t kDoip_ProtocolVersion_2012 = 0x2;  // ISO 13400-2012
constexpr uint8_t kDoip_ProtocolVersion_2019 = 0x3;  // ISO 13400-2019
constexpr uint8_t kDoip_ProtocolVersion = kDoip_ProtocolVersion_2012;
constexpr uint8_t kDoip_ProtocolVersion_Def = 0xFF;
constexpr uint32_t kDoip_Protocol_MaxPayload = 0xFFFFFFFF;  // 4294967295 bytes
/* Payload Types */
constexpr uint16_t kDoip_GenericHeadr_NackType = 0x0000;
constexpr uint16_t kDoip_VehicleIdentification_ReqType = 0x0001;
constexpr uint16_t kDoip_VehicleIdentificationEID_ReqType = 0x0002;
constexpr uint16_t kDoip_VehicleIdentificationVIN_ReqType = 0x0003;
constexpr uint16_t kDoip_VehicleAnnouncement_ResType = 0x0004;
constexpr uint16_t kDoip_RoutingActivation_ReqType = 0x0005;
constexpr uint16_t kDoip_RoutingActivation_ResType = 0x0006;
constexpr uint16_t kDoip_AliveCheck_ReqType = 0x0007;
constexpr uint16_t kDoip_AliveCheck_ResType = 0x0008;
//constexpr uint16_t kDoipENTITY_STATUS_REQ_TYPE                             0x4001
//constexpr uint16_t kDoipENTITY_STATUS_RES_TYPE                             0x4002
//constexpr uint16_t kDoipDIAG_POWER_MODEINFO_REQ_TYPE                       0x4003
//constexpr uint16_t kDoipDIAG_POWER_MODEINFO_RES_TYPE                       0x4004
constexpr uint16_t kDoip_DiagMessage_Type = 0x8001;
constexpr uint16_t kDoip_DiagMessagePosAck_Type = 0x8002;
constexpr uint16_t kDoip_DiagMessageNegAck_Type = 0x8003;
constexpr uint16_t kDoip_InvalidPayload_Type = 0xFFFF;
/* Payload length excluding header */
constexpr uint8_t kDoip_VehicleIdentification_ReqLen = 0;
constexpr uint8_t kDoip_VehicleIdentificationEID_ReqLen = 6;
constexpr uint8_t kDoip_VehicleIdentificationVIN_ReqLen = 17;
constexpr uint8_t kDoip_VehicleAnnouncement_ResMaxLen = 33;
constexpr uint8_t kDoip_GenericHeader_NackLen = 1;
constexpr uint8_t kDoip_RoutingActivation_ReqMinLen = 7;   //without OEM specific use byte
constexpr uint8_t kDoip_RoutingActivation_ResMinLen = 9;   //without OEM specific use byte
constexpr uint8_t kDoip_RoutingActivation_ReqMaxLen = 11;  //with OEM specific use byte
constexpr uint8_t kDoip_RoutingActivation_ResMaxLen = 13;  //with OEM specific use byte
//constexpr uint8_t kDoipALIVE_CHECK_RES_LEN							1
constexpr uint8_t kDoip_DiagMessage_ReqResMinLen = 4;  // considering SA and TA
constexpr uint8_t kDoip_DiagMessageAck_ResMinLen = 5;  // considering SA, TA, Ack code
/* Generic DoIP Header NACK codes */
constexpr uint8_t kDoip_GenericHeader_IncorrectPattern = 0x00;
constexpr uint8_t kDoip_GenericHeader_UnknownPayload = 0x01;
constexpr uint8_t kDoip_GenericHeader_MessageTooLarge = 0x02;
constexpr uint8_t kDoip_GenericHeader_OutOfMemory = 0x03;
constexpr uint8_t kDoip_GenericHeader_InvalidPayloadLen = 0x04;
/* Routing Activation request activation types */
constexpr uint8_t kDoip_RoutingActivation_ReqActType_Default = 0x00;
constexpr uint8_t kDoip_RoutingActivation_ReqActType_WWHOBD = 0x01;
constexpr uint8_t kDoip_RoutingActivation_ReqActType_CentralSec = 0xE0;
/* Routing Activation response code values */
constexpr uint8_t kDoip_RoutingActivation_ResCode_UnknownSA = 0x00;
constexpr uint8_t kDoip_RoutingActivation_ResCode_AllSocktActive = 0x01;
constexpr uint8_t kDoip_RoutingActivation_ResCode_DifferentSA = 0x02;
constexpr uint8_t kDoip_RoutingActivation_ResCode_ActiveSA = 0x03;
constexpr uint8_t kDoip_RoutingActivation_ResCode_AuthentnMissng = 0x04;
constexpr uint8_t kDoip_RoutingActivation_ResCode_ConfirmtnRejectd = 0x05;
constexpr uint8_t kDoip_RoutingActivation_ResCode_UnsupportdActType = 0x06;
constexpr uint8_t kDoip_RoutingActivation_ResCode_TLSRequired = 0x07;
constexpr uint8_t kDoip_RoutingActivation_ResCode_RoutingSuccessful = 0x10;
constexpr uint8_t kDoip_RoutingActivation_ResCode_ConfirmtnRequired = 0x11;
/* Diagnostic Message positive acknowledgement code */
constexpr uint8_t kDoip_DiagnosticMessage_PosAckCode_Confirm = 0x00;
/* Diagnostic Message negative acknowledgement code */
constexpr uint8_t kDoip_DiagnosticMessage_NegAckCode_InvalidSA = 0x02;
/* Further action code values */
//constexpr uint8_t kDoipNO_FURTHER_ACTION                              0x00
//constexpr uint8_t kDoipFURTHER_ACTION_CENTRAL_SEC                     0x10
//constexpr uint8_t kDoipFURTHER_VM_SPECIFIC_MIN                        0x11
//constexpr uint8_t kDoipFURTHER_VM_SPECIFIC_MAX                        0xFF
/* VIN/GID Sync status Code values */
//constexpr uint8_t kDoipVIN_GID_SYNC                                   0x00
//constexpr uint8_t kDoipVIN_GID_NOT_SYNC                               0x10
/* Vehicle identification parameter (invalidity pattern) */
constexpr uint8_t kDoip_VIN_Invalid_FF = 0xFF;
constexpr uint8_t kDoip_VIN_Invalid_00 = 0x00;
constexpr uint16_t kDoip_LogAddress_Invalid = 0xFFFF;
constexpr uint8_t kDoip_EID_Invalid_FF = 0xFF;
constexpr uint8_t kDoip_EID_Invalid_00 = 0x00;
constexpr uint8_t kDoip_GID_Invalid_FF = 0xFF;
constexpr uint8_t kDoip_GID_Invalid_00 = 0x00;
/* DoIP timing and communication parameter (in millisecond) */
/* Description: This is used to configure the
                timeout value for a DoIP Routing Activation request */
constexpr uint32_t kDoIPRoutingActivationTimeout = 1000;  // 1 sec
/* Description: This timeout specifies the maximum time that
                the test equipment waits for a confirmation ACK or NACK
                from the DoIP entity after the last byte of a DoIP Diagnostic
                request message has been sent
*/
constexpr uint32_t kDoIPDiagnosticAckTimeout = 2000;  // 2 sec
/* Description: This timeout specifies the maximum time that the
                client waits for response to a previously sent UDP message.
                This includes max time to wait and collect multiple responses
                to previous broadcast(UDP only)
 * */
constexpr uint32_t kDoIPCtrl = 2000;  // 2 sec
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // LIB_LIBDOIP_COMMON_COMMON_DOIP_TYPES_H_
