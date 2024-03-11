/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_UDS_MESSAGE_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_UDS_MESSAGE_H

#include <cstdint>
#include <map>
#include <string_view>

#include "uds_transport/protocol_types.h"

namespace uds_transport {

class UdsMessage {
 public:
  // type for UDS source and target addresses
  using Address = std::uint16_t;
  // Ip address
  using IpAddress = std::string_view;
  // Port Number
  using PortNumber = std::uint16_t;
  // Type for the meta information attached to a UdsMessage
  using MetaInfoMap = std::map<std::string, std::string>;
  // type of target address in UdsMessage
  enum class TargetAddressType : std::uint8_t { kPhysical = 0U, kFunctional = 1U };

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
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UDS_TRANSPORT_LAYER_API_UDS_TRANSPORT_UDS_MESSAGE_H