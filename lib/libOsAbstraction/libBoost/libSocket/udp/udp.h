/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UDP_H
#define UDP_H

// includes
#include "udp_Types.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libSocket {
namespace udp {

using UdpSocket         = boost::asio::ip::udp;
using UdpIpAddress      = boost::asio::ip::address;
using UdpErrorCodeType  = boost::system::error_code;

/*
 @ Class Name        : Create Udp Socket
 @ Class Description : Class used to create a udp socket for handling transmission
                       and reception of udp message from driver
*/
class createUdpSocket {

public:
    // Port Type
    enum class PortType : std::uint8_t
    {
        kUdp_Broadcast = 0,
        kUdp_Unicast
    };
    // ctor
    createUdpSocket(Boost_String& localIpaddress, uint16_t portNum, PortType portType, UdpHandlerRead udpHandlerRead);
    // dtor
    virtual ~createUdpSocket();
    // Function to Open the socket
    bool Open();
    // Transmit
    bool Transmit(UdpMessageConstPtr udpMessage);
    // Function to destroy the socket
    bool Destroy();
private:
    // local Ip address
    Boost_String localIpaddress_e;
    // local port number
    uint16_t localportNum_e;
    // Rxbuffer
    uint8_t rxbuffer_e[kDoipUdpResSize];
    // udp socket
    std::unique_ptr<UdpSocket::socket> udpSocket_e;
    // end points
    UdpSocket::endpoint remote_endpoint_e;
    // port type - broadcast / unicast
    PortType portType_e;
    // Handler invoked during read operation
    UdpHandlerRead udpHandlerRead_e;
    // boost io context 
    boost::asio::io_context io_context;
private:
    // function to handle read
    void HandleMessage(const UdpErrorCodeType &error, std::size_t bytes_recvd);
};

} // udp
} // libSocket
} // libBoost
} // libOsAbstraction

#endif // UDP_H
