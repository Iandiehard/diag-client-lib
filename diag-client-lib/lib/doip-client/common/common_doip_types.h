/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_COMMON_COMMON_DOIP_TYPES_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_COMMON_COMMON_DOIP_TYPES_H

#include <cstdint>

namespace doip_client {

/* DoIP Port Number - Unsecured */
constexpr std::uint16_t kDoipPort = 13400U;
/* Udp Channel Length */
constexpr std::uint32_t kUdpChannelLength = 41U;
/* Tcp Channel Length */
constexpr std::uint32_t kTcpChannelLength = 4096U;
/* DoIP Header */
constexpr std::uint8_t kDoipheadrSize = 0x8;
constexpr std::uint8_t kDoip_ProtocolVersion_2012 = 0x2;  // ISO 13400-2012
constexpr std::uint8_t kDoip_ProtocolVersion_2019 = 0x3;  // ISO 13400-2019
constexpr std::uint8_t kDoip_ProtocolVersion = kDoip_ProtocolVersion_2012;
constexpr std::uint8_t kDoip_ProtocolVersion_Def = 0xFF;
constexpr std::uint32_t kDoip_Protocol_MaxPayload = 0xFFFFFFFF;  // 4294967295 bytes
/* Payload Types */
constexpr std::uint16_t kDoip_GenericHeadr_NackType = 0x0000;
constexpr std::uint16_t kDoip_VehicleIdentification_ReqType = 0x0001;
constexpr std::uint16_t kDoip_VehicleIdentificationEID_ReqType = 0x0002;
constexpr std::uint16_t kDoip_VehicleIdentificationVIN_ReqType = 0x0003;
constexpr std::uint16_t kDoip_VehicleAnnouncement_ResType = 0x0004;

//constexpr std::uint16_t kDoipENTITY_STATUS_REQ_TYPE                             0x4001
//constexpr std::uint16_t kDoipENTITY_STATUS_RES_TYPE                             0x4002
//constexpr std::uint16_t kDoipDIAG_POWER_MODEINFO_REQ_TYPE                       0x4003
//constexpr std::uint16_t kDoipDIAG_POWER_MODEINFO_RES_TYPE                       0x4004

constexpr std::uint16_t kDoip_InvalidPayload_Type = 0xFFFF;
/* Payload length excluding header */
constexpr std::uint32_t kDoip_VehicleIdentification_ReqLen = 0;
constexpr std::uint32_t kDoip_VehicleIdentificationEID_ReqLen = 6;
constexpr std::uint32_t kDoip_VehicleIdentificationVIN_ReqLen = 17;
constexpr std::uint32_t kDoip_VehicleAnnouncement_ResMaxLen = 33;
constexpr std::uint32_t kDoip_GenericHeader_NackLen = 1;

//constexpr std::uint8_t kDoipALIVE_CHECK_RES_LEN							1

/* Further action code values */
//constexpr std::uint8_t kDoipNO_FURTHER_ACTION                              0x00
//constexpr std::uint8_t kDoipFURTHER_ACTION_CENTRAL_SEC                     0x10
//constexpr std::uint8_t kDoipFURTHER_VM_SPECIFIC_MIN                        0x11
//constexpr std::uint8_t kDoipFURTHER_VM_SPECIFIC_MAX                        0xFF
/* VIN/GID Sync status Code values */
//constexpr std::uint8_t kDoipVIN_GID_SYNC                                   0x00
//constexpr std::uint8_t kDoipVIN_GID_NOT_SYNC                               0x10
/* Vehicle identification parameter (invalidity pattern) */
constexpr std::uint8_t kDoip_VIN_Invalid_FF = 0xFF;
constexpr std::uint8_t kDoip_VIN_Invalid_00 = 0x00;
constexpr std::uint16_t kDoip_LogAddress_Invalid = 0xFFFF;
constexpr std::uint8_t kDoip_EID_Invalid_FF = 0xFF;
constexpr std::uint8_t kDoip_EID_Invalid_00 = 0x00;
constexpr std::uint8_t kDoip_GID_Invalid_FF = 0xFF;
constexpr std::uint8_t kDoip_GID_Invalid_00 = 0x00;
/* DoIP timing and communication parameter (in millisecond) */
/* Description: This timeout specifies the maximum time that the
                client waits for response to a previously sent UDP message.
                This includes max time to wait and collect multiple responses
                to previous broadcast(UDP only)
 * */
constexpr std::uint32_t kDoIPCtrl = 2000U;  // 2 sec

}  // namespace doip_client

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_DOIP_CLIENT_COMMON_COMMON_DOIP_TYPES_H
