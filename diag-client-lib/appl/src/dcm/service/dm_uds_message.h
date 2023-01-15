/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_DM_UDS_MESSAGE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_DM_UDS_MESSAGE_H
/* includes */
#include "ara/diag/uds_transport/uds_message.h"
#include "include/diagnostic_client_message_type.h"

namespace diag {
namespace client {
namespace uds_message {
class DmUdsMessage : public ara::diag::uds_transport::UdsMessage {
public:
  // ctor
  DmUdsMessage(Address sa, Address ta, IpAddress host_ip_address, ara::diag::uds_transport::ByteVector& payload);
  // dtor
  ~DmUdsMessage() = default;

private:
  // SA
  Address source_address;
  // TA
  Address target_address;
  // TA type
  TargetAddressType target_address_type;
  // Host Ip Address
  IpAddress host_ip_address;
  // store only UDS payload to be sent
  ara::diag::uds_transport::ByteVector& uds_payload;

  // add new metaInfo to this message.
  void AddMetaInfo(std::shared_ptr<const MetaInfoMap> meta_info) override {
    // Todo [Add meta info information]
  }

  // Get the UDS message data starting with the SID (A_Data as per ISO)
  const ara::diag::uds_transport::ByteVector& GetPayload() const override { return uds_payload; }

  // return the underlying buffer for write access
  ara::diag::uds_transport::ByteVector& GetPayload() override { return uds_payload; }

  // Get the source address of the uds message.
  Address GetSa() const noexcept override { return source_address; }

  // Get the target address of the uds message.
  Address GetTa() const noexcept override { return target_address; }

  // Get the target address type (phys/func) of the uds message.
  TargetAddressType GetTaType() const noexcept override { return target_address_type; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address; }

  // Get Host port number
  PortNumber GetHostPortNumber() const noexcept override { return 13400; }
};

class DmUdsResponse : public UdsRequestMessage {
public:
  DmUdsResponse(ByteVector& payload);
  virtual ~DmUdsResponse();

private:
  // store only UDS payload to be sent
  ByteVector& uds_payload;
  // Host Ip Address
  IpAddress host_ip_address;

  // Get the UDS message data starting with the SID (A_Data as per ISO)
  const ByteVector& GetPayload() const override { return uds_payload; }

  // return the underlying buffer for write access
  ByteVector& GetPayload() override { return uds_payload; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address; }
};
}  // namespace uds_message
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_DM_UDS_MESSAGE_H