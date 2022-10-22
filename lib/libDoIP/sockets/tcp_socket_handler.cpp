/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "sockets/tcp_socket_handler.h"
#include "channel/tcp_channel.h"

namespace ara{
namespace diag{
namespace doip{
namespace tcpSocket{

/*
 @ Class Name        : tcpSocket
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */

//ctor
tcp_SocketHandler::tcp_SocketHandler(kDoip_String& localIpaddress, ara::diag::doip::tcpChannel::tcpChannel& channel)
                :localIpaddress_e(localIpaddress),
                 channel_e(channel) {
    using namespace std::placeholders;
    //create socket
    tcpSocket = std::make_unique<TcpSocket>(localIpaddress_e, localportNum_e,
                                            std::bind(&tcp_SocketHandler::HandleMessage, this, _1));
}

//dtor
tcp_SocketHandler::~tcp_SocketHandler() {
}

//Function to Start listening from socket
void tcp_SocketHandler::StartListeningFromSocket() {    
}

//
void tcp_SocketHandler::Start() {

}

//
void tcp_SocketHandler::Stop() {

}

//
bool tcp_SocketHandler::ConnectToHost(kDoip_String hostIpaddress, 
                                      uint16_t hostportNum) {
    bool ret_val = false;
    if(tcpSocket->Open()) {
        ret_val = tcpSocket->ConnectToHost(hostIpaddress, hostportNum);
    }
    return ret_val;
}

//
bool tcp_SocketHandler::DisconnectFromHost() {
    bool ret_val = false;
    if(tcpSocket->DisconnectFromHost()) {
        ret_val = tcpSocket->Destroy();
    }
    return ret_val;
}


//Function to trigger transmission
bool tcp_SocketHandler::Transmit(TcpMessageConstPtr tcpMessage) {
    return (tcpSocket->Transmit(std::move(tcpMessage)));
}

// function called when tcp message received on socket
void tcp_SocketHandler::HandleMessage(TcpMessagePtr tcpMessage) {
    // send data to tcp channel
    channel_e.HandleMessage(std::move(tcpMessage));
}

} // tcpSocket
} // doip
} // diag
} // ara  
