/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _UDS_MESSAGE_H_
#define _UDS_MESSAGE_H_

#include <map>

#include "protocol_types.h"

namespace ara {
namespace diag {
namespace uds_transport {
class UdsMessage {
public:
  // type for UDS source and target addresses
  using Address = uint16_t;
  // Ip address
  using IpAddress = std::string;
  // Port Number
  using PortNumber = uint16_t;
  // Type for the meta information attached to a UdsMessage
  using MetaInfoMap = std::map<std::string, std::string>;
  // type of target address in UdsMessage
  enum class TargetAddressType : std::uint8_t {
    kPhysical = 0,
    kFunctional = 1
  };
  
  // ctor
  UdsMessage() = default;
  
  UdsMessage(const UdsMessage &other) = default;
  
  UdsMessage(UdsMessage &&other) noexcept = default;
  
  UdsMessage &operator=(const UdsMessage &other) = default;
  
  UdsMessage &operator=(UdsMessage &&other) noexcept = default;
  
  // dtor
  inline virtual ~UdsMessage() = default;
  
  // add new metaInfo to this message.
  virtual void AddMetaInfo(std::shared_ptr<const MetaInfoMap> meta_info) = 0;
  
  // Get the UDS message data starting with the SID (A_Data as per ISO)
  virtual const ByteVector &GetPayload() const = 0;
  
  // return the underlying buffer for write access
  virtual ByteVector &GetPayload() = 0;
  
  // Get the source address of the uds message.
  virtual Address GetSa() const noexcept = 0;
  
  // Get the target address of the uds message.
  virtual Address GetTa() const noexcept = 0;
  
  // Get the target address type (phys/func) of the uds message.
  virtual TargetAddressType GetTaType() const noexcept = 0;
  
  // Get Host Ip address
  virtual IpAddress GetHostIpAddress() const noexcept = 0;
  
  // Get Host port number
  virtual PortNumber GetHostPortNumber() const noexcept = 0;
};

// This is the unique_ptr for constant UdsMessages
using UdsMessageConstPtr = std::unique_ptr<const UdsMessage>;
// This is the unique_ptr for UdsMessages
using UdsMessagePtr = std::unique_ptr<UdsMessage>;
}  // namespace uds_transport
}  // namespace diag
}  // namespace ara
#endif  // _UDS_MESSAGE_H_