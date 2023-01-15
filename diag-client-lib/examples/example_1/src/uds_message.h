#ifndef _UDS_MESSAGE_H_
#define _UDS_MESSAGE_H_

#include "create_diagnostic_client.h"

class UdsMessage : public diag::client::uds_message::UdsRequestMessage {
public:
  // ctor
  UdsMessage(IpAddress hostipaddress, diag::client::uds_message::ByteVector &payload) : host_ip_address(hostipaddress),
                                                                                        uds_payload(payload) {
  }
  
  // dtor
  virtual ~UdsMessage() = default;

private:
  // host ip address
  IpAddress host_ip_address;
  // store only UDS payload to be send
  diag::client::uds_message::ByteVector &uds_payload;
  
  const diag::client::uds_message::ByteVector &GetPayload() const override { return uds_payload; }
  
  // return the underlying buffer for write access
  diag::client::uds_message::ByteVector &GetPayload() override { return uds_payload; }
  
  // Get Host Ip address
  IpAddress GetHostIpAddress() const noexcept override { return host_ip_address; };
};

#endif  // _UDS_MESSAGE_H_