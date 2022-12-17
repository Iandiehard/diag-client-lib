/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UDP_TYPES_H
#define UDP_TYPES_H

// includes
#include "libCommon/Boost_Header.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libSocket {
namespace udp {


class UdpMessageType
{
    public:
        // buffer type
        using buffType = std::vector<uint8_t> ;
        // ip addresstype
        using ipAddressType = std::string;
    public:
        // Receive buffer
        buffType rxBuffer;
        // Transmit buffer
        buffType txBuffer;
        // Local ipaddress
        ipAddressType localIpAddress;
        // host ipaddress
        ipAddressType hostIpAddress;
        // host portnum
        uint16_t hostportNum;
    // ctor
    UdpMessageType()
    {};
    // dtor
    virtual ~UdpMessageType()
    {};
};
//
using UdpMessageConstPtr = std::unique_ptr<const UdpMessageType>;
// unique pointer to UdpMessage
using UdpMessagePtr = std::unique_ptr<UdpMessageType>;
// Udp function template used for reception
using UdpHandlerRead = std::function<void(UdpMessagePtr)>;
//
constexpr uint8_t kDoipUdpResSize = 40;

} // udp
} // libSocket
} // libBoost
} // libOsAbstraction

#endif // UDP_TYPES_H
