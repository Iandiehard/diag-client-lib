/* Diagnostic Client library
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
                :localIpaddress_e{localIpaddress},
                 localportNum_e{localportNum},
                 exit_request_{false},
                 running_{false},
                 tcp_handler_read_{tcpHandlerRead} {
    DLT_REGISTER_CONTEXT(tcp_socket_ctx,"tcps","Tcp Socket Context");
    // Create socket
    tcp_socket_ = std::make_unique<TcpSocket::socket>(io_context_);
    // Start thread to receive messages
    thread_ = std::thread([&](){
        std::unique_lock<std::mutex> lck(mutex_);
        while (!exit_request_) {
            if (!running_) {
                cond_var_.wait(lck);
            }
            if (running_) {
                HandleMessage();
            }
        }
    });
}

// dtor
createTcpSocket::~createTcpSocket() {
    exit_request_ = true;
	running_ = false;
	cond_var_.notify_all();
	thread_.join();
    DLT_UNREGISTER_CONTEXT(tcp_socket_ctx);
}

bool createTcpSocket::Open() {
    TcpErrorCodeType ec;
    bool retVal = false;
    // Open the socket
    tcp_socket_->open(TcpSocket::v4(), ec);
    if(ec.value() == boost::system::errc::success) {
        // reuse address
        boost::asio::socket_base::reuse_address reuseaddress_option(true);
        tcp_socket_->set_option(reuseaddress_option);
        // Set socket to non blocking
        tcp_socket_->non_blocking(false);
        //bind to local address and random port
        tcp_socket_->bind(TcpSocket::endpoint(TcpIpAddress::from_string(localIpaddress_e), 0), ec);
        if(ec.value() == boost::system::errc::success) {
            // Socket binding success
            TcpSocket::endpoint endpoint_ = tcp_socket_->local_endpoint();
            DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
                DLT_CSTRING("Tcp Socket Opened and bound to"),
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
    tcp_socket_->connect(TcpSocket::endpoint(TcpIpAddress::from_string(hostIpaddress), hostportNum),
                            ec);
    if(ec.value() == boost::system::errc::success) {
        TcpSocket::endpoint endpoint_ = tcp_socket_->remote_endpoint();
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp Socket Connected to host"),
                DLT_CSTRING("<"),
                DLT_CSTRING(endpoint_.address().to_string().c_str()),
                DLT_CSTRING(","),
                DLT_UINT16(endpoint_.port()),
                DLT_CSTRING(">"));
        // start reading
        running_ = true;
	    cond_var_.notify_all();
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
    tcp_socket_->shutdown(TcpSocket::socket::shutdown_both, ec);
    if(ec.value() == boost::system::errc::success) {
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp Socket Disconnected from host"));
        // Socket shutdown success
        retVal = true;
        // stop reading
        running_ = false;
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
    boost::asio::write(*tcp_socket_.get(),
                        boost::asio::buffer(tcpMessage->txBuffer_, std::size_t(tcpMessage->txBuffer_.size())),
                        ec);
    // Check for error 
    if(ec.value() == boost::system::errc::success) {
        TcpSocket::endpoint endpoint_ = tcp_socket_->remote_endpoint();
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
    tcp_socket_->close();
    return true;
}

// Handle reading from socket
void createTcpSocket::HandleMessage() {
    TcpErrorCodeType ec;
    TcpMessagePtr tcp_rx_message  = std::make_unique<TcpMessageType>();
    // reserve the buffer
    tcp_rx_message->rxBuffer_.resize(kDoipheadrSize);
    // start blocking read to read Header first
    boost::asio::read(*tcp_socket_.get(),
                boost::asio::buffer(&tcp_rx_message->rxBuffer_[0], kDoipheadrSize), ec);
    
    // Check for error 
    if(ec.value() == boost::system::errc::success) {
        // read the next bytes to read
        uint32_t read_next_bytes = [&tcp_rx_message](){
            return((uint32_t)((uint32_t)((tcp_rx_message->rxBuffer_[4] << 24) & 0xFF000000) | \
                      (uint32_t)((tcp_rx_message->rxBuffer_[5] << 16) & 0x00FF0000) | \
                      (uint32_t)((tcp_rx_message->rxBuffer_[6] <<  8) & 0x0000FF00) | \
                      (uint32_t)((tcp_rx_message->rxBuffer_[7] & 0x000000FF))));
        }();

        // reserve the buffer
        tcp_rx_message->rxBuffer_.resize(kDoipheadrSize + std::size_t(read_next_bytes));
        boost::asio::read(*tcp_socket_.get(),
                            boost::asio::buffer(&tcp_rx_message->rxBuffer_[kDoipheadrSize], read_next_bytes), ec);
        // all message received, transfer to upper layer
        TcpSocket::endpoint endpoint_ = tcp_socket_->remote_endpoint();
        DLT_LOG(tcp_socket_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Tcp Message received from"),
            DLT_CSTRING("<"),
            DLT_CSTRING(endpoint_.address().to_string().c_str()),
            DLT_CSTRING(","),
            DLT_UINT16(endpoint_.port()),
            DLT_CSTRING(">"));
        // send data to upper layer
        tcp_handler_read_(std::move(tcp_rx_message));
    }
    else if(ec.value() == boost::asio::error::eof) {
        // remote disconnection
        running_ = false;
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

} // tcp
} // libSocket
} // libBoost
} // libOsAbstraction
