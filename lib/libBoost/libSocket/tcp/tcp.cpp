/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "tcp.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libSocket {
namespace tcp {

// ctor
createTcpSocket::createTcpSocket(Boost_String &localIpaddress, uint16_t localportNum, TcpHandlerRead tcpHandlerRead)
                :localIpaddress_e(localIpaddress),
                 localportNum_e(localportNum),
                 exit_request_e(false),
                 running_e(false),
                 tcpHandlerRead_e(tcpHandlerRead) {
    DLT_REGISTER_CONTEXT(tcp_socket_ctx,"tcps","Tcp Socket Context");
    // Create socket
    tcpSocket_e = std::make_unique<TcpSocket::socket>(io_context);
    // Start thread to receive messages
    thread_e = std::thread(&createTcpSocket::Run, this);
}

// dtor
createTcpSocket::~createTcpSocket() {
    exit_request_e = true;
	running_e = false;
	cond_var_e.notify_all();
	thread_e.join();
    DLT_UNREGISTER_CONTEXT(tcp_socket_ctx);
}

bool createTcpSocket::Open() {
    TcpErrorCodeType ec;
    bool retVal = false;
    // Open the socket
    tcpSocket_e->open(TcpSocket::v4(), ec);
    if(ec.value() == boost::system::errc::success) {
        // reuse address
        boost::asio::socket_base::reuse_address reuseaddress_option(true);
        tcpSocket_e->set_option(reuseaddress_option);
        // Set socket to non blocking
        tcpSocket_e->non_blocking(false);
        //bind to local address and random port
        tcpSocket_e->bind(TcpSocket::endpoint(TcpIpAddress::from_string(localIpaddress_e), 0), ec);
        if(ec.value() == boost::system::errc::success) {
            // Socket binding success
            TcpSocket::endpoint endpoint_ = tcpSocket_e->local_endpoint();
            DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
                DLT_CSTRING("Tcp Socket Opened and binded to"),
                DLT_CSTRING("<"),
                DLT_CSTRING(endpoint_.address().to_string().c_str()),
                DLT_CSTRING(","),
                DLT_UINT16(endpoint_.port()),
                DLT_CSTRING(">"));
            retVal = true;
        }
        else {
            // Socket binding failed            
            DLT_LOG(tcp_socket_ctx, DLT_LOG_ERROR, 
                DLT_CSTRING("Tcp Socket Bind failed with message: "), 
                DLT_CSTRING(ec.message().c_str()));
            retVal = false;
        }
    }
    else {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_ERROR, 
            DLT_CSTRING("Tcp Socket Opening failed with error: "), 
            DLT_CSTRING(ec.message().c_str()));
    }

    return retVal;
}

// connect to host
bool createTcpSocket::ConnectToHost(std::string hostIpaddress, uint16_t hostportNum) {
    TcpErrorCodeType ec;
    bool retVal = false;
    // connect to provided ipAddress
    tcpSocket_e->connect(TcpSocket::endpoint(TcpIpAddress::from_string(hostIpaddress), hostportNum), 
                            ec);
    if(ec.value() == boost::system::errc::success) {
        TcpSocket::endpoint endpoint_ = tcpSocket_e->remote_endpoint();
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp Socket Connected to host"),
                DLT_CSTRING("<"),
                DLT_CSTRING(endpoint_.address().to_string().c_str()),
                DLT_CSTRING(","),
                DLT_UINT16(endpoint_.port()),
                DLT_CSTRING(">"));
        // start reading
        running_e = true;
	    cond_var_e.notify_all();
        retVal = true;
    }
    else {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_ERROR, 
            DLT_CSTRING("Tcp Socket Connect to host failed with error: "), 
            DLT_CSTRING(ec.message().c_str()));
    }
    return retVal;
}

// Disconnect from Host
bool createTcpSocket::DisconnectFromHost() {
    TcpErrorCodeType ec;
    bool retVal = false;
    // Graceful shutdown
    tcpSocket_e->shutdown(TcpSocket::socket::shutdown_both, ec);
    if(ec.value() == boost::system::errc::success) {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp Socket Disconnected from host"));
        // Socket shutdown success
        retVal = true;
        // stop reading
        running_e = false;
    }
    else {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_ERROR, 
            DLT_CSTRING("Tcp Socket Disconnect from host failed with error: "), 
            DLT_CSTRING(ec.message().c_str()));
    }
    return retVal;
}

// Function to transmit tcp messages
bool createTcpSocket::Transmit(TcpMessageConstPtr tcpMessage) {
    TcpErrorCodeType ec;
    bool retVal = false;
    boost::asio::write(*tcpSocket_e.get(), 
                        boost::asio::buffer(tcpMessage->txBuffer, std::size_t(tcpMessage->txBuffer.size())), 
                        ec);
    // Check for error 
    if(ec.value() == boost::system::errc::success) {
        TcpSocket::endpoint endpoint_ = tcpSocket_e->remote_endpoint();
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp message sent to "),
            DLT_CSTRING("<"),
            DLT_CSTRING(endpoint_.address().to_string().c_str()),
            DLT_CSTRING(","),
            DLT_UINT16(endpoint_.port()),
            DLT_CSTRING(">"));
        retVal = true;
    }
    else {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_ERROR, 
            DLT_CSTRING("Tcp message sending failed with error: "), 
            DLT_CSTRING(ec.message().c_str()));
    }
    return retVal;
} 

// Destroy the socket
bool createTcpSocket::Destroy() {
    // destroy the socket
    tcpSocket_e->close();
    return true;
}

// Handle reading from socket
void createTcpSocket::HandleMessage() {
    TcpErrorCodeType ec;
    TcpMessagePtr tcpRxMessage  = std::make_unique<TcpMessageType>();
    // reserve the buffer
    tcpRxMessage->rxBuffer.resize(kDoipheadrSize);
    // start blocking read to read Header first
    boost::asio::read(*tcpSocket_e.get(),
                boost::asio::buffer(&tcpRxMessage->rxBuffer[0], kDoipheadrSize), ec);
    
    // Check for error 
    if(ec.value() == boost::system::errc::success) {
        // read the next bytes to read
        uint32_t readnextbytes = GetNextBytesToReadFrmDoIPHeader(tcpRxMessage->rxBuffer);
        // reserve the buffer
        tcpRxMessage->rxBuffer.resize(kDoipheadrSize + std::size_t(readnextbytes));
        boost::asio::read(*tcpSocket_e.get(),
                            boost::asio::buffer(&tcpRxMessage->rxBuffer[kDoipheadrSize], readnextbytes), ec);
        // all message received, transfer to upper layer
        TcpSocket::endpoint endpoint_ = tcpSocket_e->remote_endpoint();
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp Message received from"),
            DLT_CSTRING("<"),
            DLT_CSTRING(endpoint_.address().to_string().c_str()),
            DLT_CSTRING(","),
            DLT_UINT16(endpoint_.port()),
            DLT_CSTRING(">"));
        // send data to upper layer
        tcpHandlerRead_e(std::move(tcpRxMessage));
    }
    else if(ec.value() == boost::asio::error::eof) {
        // remote disconnection
        running_e = false;
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Remote Disconnected with:"), 
            DLT_CSTRING(ec.message().c_str()));
    }
    else {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_WARN, 
            DLT_CSTRING("Remote Disconnected with undefined error:"), 
            DLT_CSTRING(ec.message().c_str()));
    }
}

//
void createTcpSocket::Run() {
    std::unique_lock<std::mutex> lck(mutex_e);
    while (!exit_request_e) {
        if (!running_e) {
            cond_var_e.wait(lck);
		}
		if (running_e) {
            HandleMessage();
		}
    }
}

// function to read next bytes to be received
uint32_t createTcpSocket::GetNextBytesToReadFrmDoIPHeader(std::vector<uint8_t> doipHeader) {
    return((uint32_t)((uint32_t)((doipHeader[4] << 24) & 0xFF000000) | \
                      (uint32_t)((doipHeader[5] << 16) & 0x00FF0000) | \
                      (uint32_t)((doipHeader[6] <<  8) & 0x0000FF00) | \
                      (uint32_t)((doipHeader[7] & 0x000000FF))));
}


} // tcp
} // libSocket
} // libBoost
} // libOsAbstraction
