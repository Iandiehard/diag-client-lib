/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _PROTOCOL_TYPES_H_
#define _PROTOCOL_TYPES_H_

#include <vector>
#include <functional>
#include <tuple>
#include <map>
#include <string>
#include <memory>

namespace ara{
namespace diag{

namespace doip{

constexpr uint8_t kDoip_VIN_Size            = 17;
constexpr uint8_t kDoip_EID_GID_Size        = 6;

// vin type
using VINArray = std::array<uint8_t, kDoip_VIN_Size>;
// eid/gid type
using EID_GID_Array = std::array<uint8_t, kDoip_EID_GID_Size>;

class VehicleInfo {
public:
  // Vehicle Info type
  enum class VehicleInfoType : std::uint8_t {
      kVehicleAnnouncementRes         = 0,
      kVehicleIdentificationReq       = 1,
      kVehicleIdentificationReqEID    = 2,
      kVehicleIdentificationReqVIN    = 3
  };
  
  //VIN
  VINArray vin;
  
  //EID
  EID_GID_Array eid;
  
  //GID
  EID_GID_Array gid;
  
  // Logical address of DOIP entity
  uint16_t logicalAddress;
  
  // Host port number
  uint16_t hostPortNum;
  
  // Further action bytes received
  uint8_t furtherActionByte;
  
  // Synchronisation status
  uint8_t vingidSyncStatus;
  
  // Ip address of Doip Entity
  std::string hostIpAddress; // IPV4 supported
  
  // Type
  VehicleInfoType vehInfoType;
};
} //doip

namespace uds_transport {
// This is the type of ByteVector
using ByteVector = std::vector<uint8_t>;
// This is the type of Channel Id
using ChannelID = uint32_t;
// This is the type of Priority
using Priority = uint8_t;
// This is the type of Protocol Kind
using ProtocolKind = std::string;
// UdsTransportProtocolHandler are flexible "plugins", which need an identification
using UdsTransportProtocolHandlerID = uint8_t;
} // uds_transport

namespace conversion_manager {
// Conversion identification needed by user
using ConversionHandlerID = uint8_t;
// Conversion identifier Type can be used by user 
struct ConversionIdentifierType {
  // Transmission buffer
  uint32_t tx_buffer_size;
  
  // Reception buffer
  uint32_t rx_buffer_size;
  
  // p2 client time
  uint16_t p2_client_max;
  
  // p2 star Client time 
  uint16_t p2_star_client_max;
  
  // source address of client
  uint16_t source_address;
  
  // target address of server
  uint16_t target_address;
  
  // self tcp address
  std::string tcp_address;
  
  // self udp address
  std::string udp_address;
  
  // Vehicle broadcast address
  std::string udp_broadcast_address;
  
  // Port Number
  uint16_t port_num;
  
  // conversion handler ID   
  ConversionHandlerID handler_id;
};

} // conversion manager

namespace connection{
// connection identification needed by user
using ConnectionId = uint8_t;

} // connection

} // diag
} // ara


#endif // _PROTOCOL_TYPES_H_
