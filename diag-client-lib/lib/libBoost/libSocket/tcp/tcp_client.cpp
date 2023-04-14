/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// includes
#include "libSocket/tcp/tcp_client.h"

#include "libCommon/logger.h"

namespace libBoost {
namespace libSocket {
namespace tcp {
// ctor
CreateTcpClientSocket::CreateTcpClientSocket(Boost_String &local_ip_address, uint16_t local_port_num,
                                             TcpHandlerRead tcp_handler_read)
    : local_ip_address_{local_ip_address},
      local_port_num_{local_port_num},
      exit_request_{false},
      running_{false},
      tcp_handler_read_{tcp_handler_read} {
  // Create socket
  tcp_socket_ = std::make_unique<TcpSocket::socket>(io_context_);
  // Start thread to receive messages
  thread_ = std::thread([&]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_) {
      if (!running_) { cond_var_.wait(lck); }
      if (running_) { HandleMessage(); }
    }
  });
}

// dtor
CreateTcpClientSocket::~CreateTcpClientSocket() {
  exit_request_ = true;
  running_ = false;
  cond_var_.notify_all();
  thread_.join();
}

bool CreateTcpClientSocket::Open() {
  TcpErrorCodeType ec;
  bool retVal = false;
  // Open the socket
  tcp_socket_->open(TcpSocket::v4(), ec);
  if (ec.value() == boost::system::errc::success) {
    // reuse address
    boost::asio::socket_base::reuse_address reuse_address_option{true};
    tcp_socket_->set_option(reuse_address_option);
    // Set socket to non blocking
    tcp_socket_->non_blocking(false);
    //bind to local address and random port
    tcp_socket_->bind(TcpSocket::endpoint(TcpIpAddress::from_string(local_ip_address_), local_port_num_), ec);
    if (ec.value() == boost::system::errc::success) {
      // Socket binding success
      TcpSocket::endpoint endpoint_{tcp_socket_->local_endpoint()};
      logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
            msg << "Tcp Socket Opened and bound to"
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
          });
      retVal = true;
    } else {
      // Socket binding failed
      logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__,
          [ec](std::stringstream &msg) { msg << "Tcp Socket Bind failed with message: " << ec.message(); });
      retVal = false;
    }
  } else {
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp Socket Opening failed with error: " << ec.message(); });
  }
  return retVal;
}

// connect to host
bool CreateTcpClientSocket::ConnectToHost(std::string hostIpaddress, uint16_t hostportNum) {
  TcpErrorCodeType ec;
  bool ret_val{false};
  // connect to provided ipAddress
  tcp_socket_->connect(TcpSocket::endpoint(TcpIpAddress::from_string(hostIpaddress), hostportNum), ec);
  if (ec.value() == boost::system::errc::success) {
    TcpSocket::endpoint endpoint_ = tcp_socket_->remote_endpoint();
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
          msg << "Tcp Socket Connected to host"
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    // start reading
    running_ = true;
    cond_var_.notify_all();
    ret_val = true;
  } else {
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp Socket Connect to host failed with error: " << ec.message(); });
  }
  return ret_val;
}

// Disconnect from Host
bool CreateTcpClientSocket::DisconnectFromHost() {
  TcpErrorCodeType ec;
  bool retVal = false;
  // Graceful shutdown
  tcp_socket_->shutdown(TcpSocket::socket::shutdown_both, ec);
  if (ec.value() == boost::system::errc::success) {
    // Socket shutdown success
    retVal = true;
    // stop reading
    running_ = false;
  } else {
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp Socket Disconnect from host failed with error: " << ec.message(); });
  }
  return retVal;
}

// Function to transmit tcp messages
bool CreateTcpClientSocket::Transmit(TcpMessageConstPtr tcpMessage) {
  TcpErrorCodeType ec;
  bool ret_val{false};
  boost::asio::write(*tcp_socket_,
                     boost::asio::buffer(tcpMessage->txBuffer_, std::size_t(tcpMessage->txBuffer_.size())), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    TcpSocket::endpoint endpoint_{tcp_socket_->remote_endpoint()};
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
          msg << "Tcp message sent to "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    ret_val = true;
  } else {
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp message sending failed with error: " << ec.message(); });
  }
  return ret_val;
}

// Destroy the socket
bool CreateTcpClientSocket::Destroy() {
  // destroy the socket
  tcp_socket_->close();
  return true;
}

// Handle reading from socket
void CreateTcpClientSocket::HandleMessage() {
  TcpErrorCodeType ec;
  TcpMessagePtr tcp_rx_message = std::make_unique<TcpMessageType>();
  // reserve the buffer
  tcp_rx_message->rxBuffer_.resize(kDoipheadrSize);
  // start blocking read to read Header first
  boost::asio::read(*tcp_socket_, boost::asio::buffer(&tcp_rx_message->rxBuffer_[0], kDoipheadrSize), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    // read the next bytes to read
    uint32_t read_next_bytes = [&tcp_rx_message]() {
      return ((uint32_t) ((uint32_t) ((tcp_rx_message->rxBuffer_[4] << 24) & 0xFF000000) |
                          (uint32_t) ((tcp_rx_message->rxBuffer_[5] << 16) & 0x00FF0000) |
                          (uint32_t) ((tcp_rx_message->rxBuffer_[6] << 8) & 0x0000FF00) |
                          (uint32_t) ((tcp_rx_message->rxBuffer_[7] & 0x000000FF))));
    }();
    // reserve the buffer
    tcp_rx_message->rxBuffer_.resize(kDoipheadrSize + std::size_t(read_next_bytes));
    boost::asio::read(*tcp_socket_, boost::asio::buffer(&tcp_rx_message->rxBuffer_[kDoipheadrSize], read_next_bytes),
                      ec);
    // all message received, transfer to upper layer
    TcpSocket::endpoint endpoint_{tcp_socket_->remote_endpoint()};
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
          msg << "Tcp Message received from "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    // send data to upper layer
    tcp_handler_read_(std::move(tcp_rx_message));
  } else if (ec.value() == boost::asio::error::eof) {
    running_ = false;
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with: " << ec.message(); });
  } else {
    logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with undefined error: " << ec.message(); });
  }
}
}  // namespace tcp
}  // namespace libSocket
}  // namespace libBoost
