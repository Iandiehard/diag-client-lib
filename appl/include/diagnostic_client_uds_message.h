/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _DIAGNOSTIC_CLIENT_UDS_MESSAGE_H_
#define _DIAGNOSTIC_CLIENT_UDS_MESSAGE_H_


namespace diag {
namespace client {
namespace uds_message {

// This is the type of ByteVector
using ByteVector = std::vector<uint8_t>;

class UdsRequestMessage {
  public:
    // Ip address
    using IpAddress = std::string;

    // ctor
    inline UdsRequestMessage() {}
    UdsRequestMessage (const UdsRequestMessage &other)=default;
    UdsRequestMessage (UdsRequestMessage &&other) noexcept=default;
    UdsRequestMessage& operator= (const UdsRequestMessage &other)=default;
    UdsRequestMessage& operator= (UdsRequestMessage &&other) noexcept=default;
    
    // dtor
    inline virtual ~UdsRequestMessage()=default;

    // Get the UDS message data starting with the SID (A_Data as per ISO)
    virtual const ByteVector& GetPayload () const = 0;
    
    // return the underlying buffer for write access
    virtual ByteVector& GetPayload () = 0;
    
    // Get Host Ip address
    virtual IpAddress GetHostIpAddress() const noexcept = 0;
};

// This is the unique_ptr for constant UdsRequestMessage 
using UdsRequestMessageConstPtr = std::unique_ptr<const UdsRequestMessage >;
// This is the unique_ptr for UdsRequestMessage 
using UdsRequestMessagePtr = std::unique_ptr<UdsRequestMessage>;

using UdsResponseMessagePtr = std::unique_ptr<UdsRequestMessage>;

} // uds_message
} // client
} // diag

#endif // _DIAGNOSTIC_CLIENT_UDS_MESSAGE_H_
