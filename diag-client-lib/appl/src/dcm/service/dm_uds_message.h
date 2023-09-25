/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_DM_UDS_MESSAGE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_DM_UDS_MESSAGE_H
/* includes */
#include "include/diagnostic_client_uds_message_type.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"
#include "uds_transport/uds_message.h"

namespace diag {
namespace client {
namespace uds_message {

class DmUdsMessage final : public uds_transport::UdsMessage {
 public:
  // ctor
  DmUdsMessage(Address sa, Address ta, IpAddress host_ip_address, uds_transport::ByteVector &payload);

  // dtor
  ~DmUdsMessage() noexcept override = default;

 private:
  // SA
  Address source_address_;

  // TA
  Address target_address_;

  // TA type
  TargetAddressType target_address_type_;

  // Host Ip Address
  std::string host_ip_address_;

  // store only UDS payload to be sent
  uds_transport::ByteVector &uds_payload_;

  // add new metaInfo to this message.
  void AddMetaInfo(std::shared_ptr<const MetaInfoMap>) override {
    // Todo [Add meta info information]
  }

  // Get the UDS message data starting with the SID (A_Data as per ISO)
  const uds_transport::ByteVector &GetPayload() const override { return uds_payload_; }

  // return the underlying buffer for write access
  uds_transport::ByteVector &GetPayload() override { return uds_payload_; }

  // Get the source address of the uds message.
  Address GetSa() const noexcept override { return source_address_; }

  // Get the target address of the uds message.
  Address GetTa() const noexcept override { return target_address_; }

  // Get the target address type (phys/func) of the uds message.
  TargetAddressType GetTaType() const noexcept override { return target_address_type_; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address_; }

  // Get Host port number
  PortNumber GetHostPortNumber() const noexcept override { return 13400U; }
};

class DmUdsResponse final : public UdsMessage {
 public:
  explicit DmUdsResponse(ByteVector &payload);

  ~DmUdsResponse() noexcept override = default;

 private:
  // store only UDS payload to be sent
  ByteVector &uds_payload_;
  // Host Ip Address
  IpAddress host_ip_address_;

  // Get the UDS message data starting with the SID (A_Data as per ISO)
  const ByteVector &GetPayload() const override { return uds_payload_; }

  // return the underlying buffer for write access
  ByteVector &GetPayload() override { return uds_payload_; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address_; }
};

}  // namespace uds_message
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_DM_UDS_MESSAGE_H
