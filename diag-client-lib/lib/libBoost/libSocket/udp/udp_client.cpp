/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "udp_client.h"

namespace libBoost {
namespace libSocket {
namespace udp {

// ctor
createUdpSocket::createUdpSocket(
  Boost_String& local_ip_address, 
  uint16_t local_port_num, 
  PortType port_type, 
  UdpHandlerRead udp_handler_read)
  : local_ip_address_{local_ip_address},
    local_port_num_{local_port_num},
    port_type_{port_type},
    udp_handler_read_{udp_handler_read},
    rxbuffer_{} {
  udp_socket_ = std::make_unique<UdpSocket::socket>(io_context_);
}

// dtor
createUdpSocket::~createUdpSocket() {
}

bool createUdpSocket::Open() {
  UdpErrorCodeType ec;
  bool retVal = true;
  
  // Open the socket
  udp_socket_->open(UdpSocket::v4(), ec);
  if(ec) {
      // Socket Open failed
      std::cerr << ec.message() << "\n";
      retVal = false;
  }
  else {
      // check the port type
      if(port_type_ == PortType::kUdp_Broadcast) {
          // set broadcast option
          boost::asio::socket_base::broadcast broadcast_option(true);
          udp_socket_->set_option(broadcast_option);
      }
      
      // reuse address
      boost::asio::socket_base::reuse_address reuseaddress_option(true);
      udp_socket_->set_option(reuseaddress_option);
  }
  
  //bind to local address and random port
  udp_socket_->bind(UdpSocket::endpoint(UdpIpAddress::from_string(local_ip_address_), local_port_num_), ec);
  
  if(ec) {
      // Socket binding failed
      boost::system::errc::address_family_not_supported;
      std::cerr << ec.message() << "\n";
      retVal = false;
  }
  
  // start async receive
  udp_socket_->async_receive_from(boost::asio::buffer(rxbuffer_, kDoipUdpResSize),
                                  remote_endpoint_, 
                                  boost::bind(&createUdpSocket::HandleMessage, this, 
                                  boost::placeholders::_1, boost::placeholders::_2));

  return retVal;
}

// function to transmit udp messages
bool createUdpSocket::Transmit(UdpMessageConstPtr udp_message) {
  bool retVal = false;
  
  try {
    // Transmit to remote endpoints
    std::size_t send_size = udp_socket_->send_to(boost::asio::buffer(udp_message->tx_buffer_, std::size_t(udp_message->tx_buffer_.size())), 
                                UdpSocket::endpoint(UdpIpAddress::from_string(udp_message->host_ip_address_), udp_message->host_port_num_));
    
    if(send_size == udp_message->tx_buffer_.size()) {
      // successful
      retVal = true;
      // start async receive
      udp_socket_->async_receive_from(boost::asio::buffer(rxbuffer_, kDoipUdpResSize),
                                      remote_endpoint_, 
                                      boost::bind(&createUdpSocket::HandleMessage, this, 
                                      boost::placeholders::_1, boost::placeholders::_2));
    }
  }
  catch (boost::system::system_error const& ec) {
      UdpErrorCodeType error = ec.code();
      std::cerr << error.message() << "\n";
  }
  return retVal;
}

// Function to destroy the socket
bool createUdpSocket::Destroy() {
  // destroy the socket
  udp_socket_->close();
  return true;
}

// function invoked when datagram is received
void createUdpSocket::HandleMessage(const UdpErrorCodeType &error, std::size_t bytes_recvd) {
  if(error.value() == boost::system::errc::success) {
    UdpMessagePtr udp_rx_message = std::make_unique<UdpMessageType>();
        
    // Copy the data
    udp_rx_message->rx_buffer_.insert(udp_rx_message->rx_buffer_.end(), rxbuffer_, rxbuffer_ + bytes_recvd); 
    
    // fill the remote endpoints
    udp_rx_message->host_ip_address_ = remote_endpoint_.address().to_string();
    udp_rx_message->host_port_num_   = remote_endpoint_.port();
    // send data to upper layer
    udp_handler_read_(std::move(udp_rx_message));
    // start next async receive
    udp_socket_->async_receive_from(boost::asio::buffer(rxbuffer_, kDoipUdpResSize),
                                    remote_endpoint_, 
                                    boost::bind(&createUdpSocket::HandleMessage, this, 
                                    boost::placeholders::_1, boost::placeholders::_2));
  }
}

} // udp
} // libSocket
} // libBoost
