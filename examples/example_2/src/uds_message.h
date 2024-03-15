/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef EXAMPLES_EXAMPLE_1_SRC_UDS_MESSAGE_H
#define EXAMPLES_EXAMPLE_1_SRC_UDS_MESSAGE_H

#include <string_view>
// includes from diag-client library
#include "diag-client/diagnostic_client_uds_message_type.h"

class UdsMessage final : public diag::client::uds_message::UdsMessage {
 public:
  // ctor
  UdsMessage(std::string_view host_ip_address, diag::client::uds_message::UdsMessage::ByteVector payload)
      : host_ip_address_{host_ip_address},
        uds_payload_{std::move(payload)} {}

  // dtor
  ~UdsMessage() = default;

 private:
  // host ip address
  IpAddress host_ip_address_;

  // store only UDS payload to be sent
  diag::client::uds_message::UdsMessage::ByteVector uds_payload_;

  const diag::client::uds_message::UdsMessage::ByteVector &GetPayload() const override { return uds_payload_; }

  // return the underlying buffer for write access
  diag::client::uds_message::UdsMessage::ByteVector &GetPayload() override { return uds_payload_; }

  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address_; };
};

#endif  // EXAMPLES_EXAMPLE_1_SRC_UDS_MESSAGE_H