/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_VD_MESSAGE_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_VD_MESSAGE_H
/* includes */
#include "diag-client/diagnostic_client_uds_message_type.h"
#include "diag-client/diagnostic_client_vehicle_info_message_type.h"
#include "uds_transport-layer-api/uds_message.h"

namespace diag {
namespace client {
namespace vd_message {

class VdMessage final : public uds_transport::UdsMessage {
 public:
  // ctor
  VdMessage(std::uint8_t preselection_mode, uds_transport::ByteVector& preselection_value,
            std::string_view host_ip_address);

  // default ctor
  VdMessage() noexcept;

  // dtor
  ~VdMessage() noexcept override = default;

 private:
  // SA
  Address source_address_;

  // TA
  Address target_address_;

  // TA type
  TargetAddressType target_address_type;

  // Host Ip Address
  IpAddress host_ip_address_;

  // store the vehicle info payload
  uds_transport::ByteVector vehicle_info_payload_;

  // store the
  std::shared_ptr<const MetaInfoMap> meta_info_{};

  // add new metaInfo to this message.
  void AddMetaInfo(std::shared_ptr<const MetaInfoMap> meta_info) override {
    // update meta info data
    if (meta_info != nullptr) {
      meta_info_ = meta_info;
      host_ip_address_ = meta_info_->at("kRemoteIpAddress");
    }
  }

  // Get the UDS message data starting with the SID (A_Data as per ISO)
  const uds_transport::ByteVector& GetPayload() const override { return vehicle_info_payload_; }

  // return the underlying buffer for write access
  uds_transport::ByteVector& GetPayload() override { return vehicle_info_payload_; }

  // Get the source address of the uds message.
  Address GetSa() const noexcept override { return source_address_; }

  // Get the target address of the uds message.
  Address GetTa() const noexcept override { return target_address_; }

  // Get the target address type (phys/func) of the uds message.
  TargetAddressType GetTaType() const noexcept override { return target_address_type; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address_; }

  // Get Host port number
  PortNumber GetHostPortNumber() const noexcept override { return 13400U; }
};

}  // namespace vd_message
}  // namespace client
}  // namespace diag

#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_SERVICE_VD_MESSAGE_H
