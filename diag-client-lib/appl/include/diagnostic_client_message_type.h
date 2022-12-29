/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_UDS_MESSAGE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_UDS_MESSAGE_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace diag {
namespace client {

// Ip address
using IpAddress = std::string;
using ByteVector = std::vector<std::uint8_t>;

namespace uds_message {

class UdsRequestMessage {
public:
  // ctor
  UdsRequestMessage() = default;

  UdsRequestMessage (const UdsRequestMessage &other)=default;
  UdsRequestMessage (UdsRequestMessage &&other) noexcept=default;
  UdsRequestMessage& operator= (const UdsRequestMessage &other)=default;
  UdsRequestMessage& operator= (UdsRequestMessage &&other) noexcept=default;

  // Dtor
  virtual ~UdsRequestMessage()=default;

  // Get the UDS message data starting with the SID (A_Data as per ISO)
  virtual const ByteVector& GetPayload () const = 0;

  // Return the underlying buffer for write access
  virtual ByteVector& GetPayload () = 0;

  // Get Host Ip address
  virtual IpAddress GetHostIpAddress() const noexcept = 0;
};

// This is the unique_ptr for constant UdsRequestMessage 
using UdsRequestMessageConstPtr = std::unique_ptr<const UdsRequestMessage>;

// This is the unique_ptr for Request Message
using UdsRequestMessagePtr = std::unique_ptr<UdsRequestMessage>;

// This is the unique_ptr for Response Message
using UdsResponseMessagePtr = std::unique_ptr<UdsRequestMessage>;

} // uds_message

namespace vehicle_info {

struct VehicleAddrInfoResponse {
  IpAddress ip_address;
  std::uint16_t logical_address;
  std::string vin;
  std::string eid;
  std::string gid;
};

struct VehicleAddrInfoRequest {
  /*
    0U : No preselection
    1U : DoIP Entities with given VIN
    2U : DoIP Entities with given EID
   */
  std::uint8_t preselection_mode {0U};
  /*
    VIN when preselection_mode = 1U
    EID when preselection_mode = 2U
    Empty when preselection_mode = 0U
   */
  std::string preselection_value;
};

using VehicleInfoListResponseType = std::vector<VehicleAddrInfoResponse>;
using VehicleInfoListRequestType = VehicleAddrInfoRequest;

// Message type for vehicle identification request/ response / announcement
class VehicleInfoMessage {
public:
  // ctor
  VehicleInfoMessage() = default;
  VehicleInfoMessage (const VehicleInfoMessage &other)=default;
  VehicleInfoMessage (VehicleInfoMessage &&other) noexcept=default;
  VehicleInfoMessage& operator= (const VehicleInfoMessage &other)=default;
  VehicleInfoMessage& operator= (VehicleInfoMessage &&other) noexcept=default;

  // dtor
  virtual ~VehicleInfoMessage() = default;

  // Get the list of vehicle available in the network
  virtual VehicleInfoListResponseType& GetVehicleList() = 0;
};

// This is the unique_ptr for Response Message
using VehicleInfoMessageResponsePtr = std::unique_ptr<VehicleInfoMessage>;

} // vehicle_info

} // client
} // diag

#endif // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_UDS_MESSAGE_H
