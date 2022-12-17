/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _UDP_SOCKETHANDLER_H_
#define _UDP_SOCKETHANDLER_H_

//includes
#include "common/common_doip_types.h"
#include "libSocket/udp/udp.h"

namespace ara{
namespace diag{
namespace doip{

// forward class declaration
namespace udpChannel {
class udpChannel;
}

namespace udpSocket{
    
// typedefs
constexpr uint8_t kDoIPHeaderSize = 8;
using UdpSocket             = libOsAbstraction::libBoost::libSocket::udp::createUdpSocket;
using UdpMessage            = libOsAbstraction::libBoost::libSocket::udp::UdpMessageType;
using UdpMessagePtr         = libOsAbstraction::libBoost::libSocket::udp::UdpMessagePtr;
using UdpMessageConstPtr    = libOsAbstraction::libBoost::libSocket::udp::UdpMessageConstPtr;


/*
 @ Class Name        : CreateUdpSocket
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver
 */
class udp_SocketHandler
{
    public:
        // Port Type
        enum class PortType : std::uint8_t
        {
            kUdp_Broadcast = 0,
            kUdp_Unicast
        };
        //ctor
        udp_SocketHandler(kDoip_String& localIpaddress, uint16_t portNum, PortType portType, ara::diag::doip::udpChannel::udpChannel& channel);
        //dtor
        virtual ~udp_SocketHandler();
        //start
        void Start();
        //stop
        void Stop();
        // function to trigger transmission
        bool Transmit(UdpMessageConstPtr udpTxMessage);
    private:
        // local Ip address
        kDoip_String localIpaddress_e;
        // Host Ip address
        kDoip_String hostIpaddress_e;
        // Host port number
        uint16_t portNum_e;
        // Port type
        UdpSocket::PortType portType_e;
        // udp socket
        std::unique_ptr<UdpSocket> udpSocket;
        // store tcp channel reference
        ara::diag::doip::udpChannel::udpChannel& channel_e;
        // next bytes to read from socket
        std::size_t bytes_to_read{0};
    private:
        // function to handle read broadcast
        void HandleMessageBroadcast(UdpMessagePtr udpRxMessage);
        // function to handle read unicast
        void HandleMessageUnicast(UdpMessagePtr udpRxMessage);
        // function to give transmit confirmation
        void TransmitConfirmation(bool result);
};

} // udpSocket
} // doip
} // diag
} // ara



#endif // _UDP_SOCKETHANDLER_H_
