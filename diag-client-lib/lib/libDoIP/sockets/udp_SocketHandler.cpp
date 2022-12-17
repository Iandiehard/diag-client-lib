/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sockets/udp_SocketHandler.h"
#include "channel/udp_Channel.h"

namespace ara{
namespace diag{
namespace doip{
namespace udpSocket{

udp_SocketHandler::udp_SocketHandler(kDoip_String& localIpaddress, uint16_t portNum, PortType portType, ara::diag::doip::udpChannel::udpChannel &channel)
                : localIpaddress_e{localIpaddress},
                  portNum_e{portNum},
                  channel_e{channel}
{
    using namespace std::placeholders;
    portType_e = (portType == PortType::kUdp_Broadcast) ? 
                UdpSocket::PortType::kUdp_Broadcast : UdpSocket::PortType::kUdp_Unicast;    
    // create sockets and start receiving
    if(portType_e == UdpSocket::PortType::kUdp_Broadcast) {
        udpSocket = std::make_unique<UdpSocket>(localIpaddress_e, portNum_e, portType_e , 
                    std::bind(&udp_SocketHandler::HandleMessageBroadcast, this, _1));
    }
    else {
        // unicast
        udpSocket = std::make_unique<UdpSocket>(localIpaddress_e, portNum_e, portType_e , 
                    std::bind(&udp_SocketHandler::HandleMessageUnicast, this, _1));
    }
    
}

udp_SocketHandler::~udp_SocketHandler()
{
}

void udp_SocketHandler::Start()
{
    udpSocket->Open();
}

void udp_SocketHandler::Stop()
{// destroy the socket
    udpSocket->Destroy();
}

bool udp_SocketHandler::Transmit(UdpMessageConstPtr udpTxMessage)
{
    bool retVal = false;
    retVal = udpSocket->Transmit(std::move(udpTxMessage));
    if(retVal)
    {// success
        TransmitConfirmation(true);
    }
    else
    {// failure
        TransmitConfirmation(false);
    }   
    return retVal;
}

// Function to receive broadcast message from 255.255.255.255
void udp_SocketHandler::HandleMessageBroadcast(UdpMessagePtr udpRxMessage)
{// send the datagram to udpChannel for further processing
    channel_e.HandleMessageBroadcast(std::move(udpRxMessage));
}

// Function to receive unicast message
void udp_SocketHandler::HandleMessageUnicast(UdpMessagePtr udpRxMessage)
{// send the datagram to udpChannel for further processing
    channel_e.HandleMessageUnicast(std::move(udpRxMessage));
}

// Function to confirm the transmit of udp message
void udp_SocketHandler::TransmitConfirmation(bool result)
{// send confirmation to udp channel
    channel_e.TransmitConfirmation(result);
}

} // udpSocket
} // doip
} // diag
} // ara
