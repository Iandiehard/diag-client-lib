/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _TCP_SOCKET_HANDLER_H_
#define _TCP_SOCKET_HANDLER_H_

//includes
#include "common/common_doip_types.h"
#include "libSocket/tcp/tcp.h"
namespace ara{
namespace diag{
namespace doip{

// forward declaration
namespace tcpChannel{
    class tcpChannel;
}
namespace tcpSocket{

// typedefs
using TcpSocket           = libOsAbstraction::libBoost::libSocket::tcp::createTcpSocket;
using TcpMessage          = libOsAbstraction::libBoost::libSocket::tcp::TcpMessageType;
using TcpMessagePtr       = libOsAbstraction::libBoost::libSocket::tcp::TcpMessagePtr;
using TcpMessageConstPtr  = libOsAbstraction::libBoost::libSocket::tcp::TcpMessageConstPtr;

/*
 @ Class Name        : tcp_SocketHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */
class tcp_SocketHandler {
public:
    // ctor
    tcp_SocketHandler(kDoip_String& localIpaddress, ara::diag::doip::tcpChannel::tcpChannel& channel);

    // dtor
    virtual ~tcp_SocketHandler();

    // Start listening from socket
    void StartListeningFromSocket();  //not used

    // Start
    void Start();

    // Stop
    void Stop();

    // Connect to host
    bool ConnectToHost(kDoip_String hostIpaddress, uint16_t hostportNum);

    // Disconnect from host
    bool DisconnectFromHost();

    // Transmit function
    bool Transmit(TcpMessageConstPtr tcpMessage);
private:
    // local Ip address
    kDoip_String local_ip_address_;

    // local port number
    uint16_t local_port_num_;

    // tcp socket
    std::unique_ptr<TcpSocket> tcpSocket_;

    // store tcp channel reference
    ara::diag::doip::tcpChannel::tcpChannel& channel_;
private:
    // function to handle read
    void HandleMessage(TcpMessagePtr tcpMessage);
};

} // tcpSocket
} // doip
} // diag
} // ara  


#endif // _TCP_SOCKET_HANDLER_H_
