/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "socket/udp/udp_client.h"

#include "common/logger.h"

namespace boost_support {
namespace socket {
namespace udp {

UdpClientSocket::UdpClientSocket(std::string_view local_ip_address, std::uint16_t local_port_num, PortType port_type,
                                 UdpHandlerRead udp_handler_read)
    : local_ip_address_{local_ip_address},
      local_port_num_{local_port_num},
      io_context_{},
      udp_socket_{io_context_},
      exit_request_{false},
      running_{false},
      cond_var_{},
      mutex_{},
      port_type_{port_type},
      udp_handler_read_{std::move(udp_handler_read)},
      rx_buffer_{} {
  // Start thread to receive messages
  thread_ = std::thread([this]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_) {
      if (!running_) {
        cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
      }
      if (!exit_request_) {
        if (running_) {
          io_context_.restart();
          io_context_.run();
        }
      }
    }
  });
}

UdpClientSocket::~UdpClientSocket() {
  exit_request_ = true;
  running_ = false;
  cond_var_.notify_all();
  thread_.join();
}

core_type::Result<void, UdpClientSocket::UdpErrorCode> UdpClientSocket::Open() {
  core_type::Result<void, UdpErrorCode> result{UdpErrorCode::kGenericError};
  UdpErrorCodeType ec{};

  // Open the socket
  udp_socket_.open(Udp::v4(), ec);
  if (ec.value() == boost::system::errc::success) {
    // set broadcast option
    boost::asio::socket_base::broadcast broadcast_option(true);
    udp_socket_.set_option(broadcast_option);
    // reuse address
    boost::asio::socket_base::reuse_address reuse_address_option(true);
    udp_socket_.set_option(reuse_address_option);

    if (port_type_ == PortType::kUdp_Broadcast) {
      // Todo : change the hardcoded value of port number 13400
      udp_socket_.bind(Udp::endpoint(boost::asio::ip::address_v4::any(), 13400), ec);
    } else {
      //bind to local address and random port
      udp_socket_.bind(Udp::endpoint(UdpIpAddress::from_string(local_ip_address_), local_port_num_), ec);
    }

    if (ec.value() == boost::system::errc::success) {
      Udp::endpoint endpoint{udp_socket_.local_endpoint()};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [endpoint](std::stringstream &msg) {
            msg << "Udp Socket Opened and bound to "
                << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
          });
      // Update the port number with new one
      local_port_num_ = udp_socket_.local_endpoint().port();
      {  // start reading
        std::lock_guard<std::mutex> lock{mutex_};
        running_ = true;
        cond_var_.notify_all();
      }
      // start async receive
      udp_socket_.async_receive_from(
          boost::asio::buffer(rx_buffer_), remote_endpoint_,
          [this](const UdpErrorCodeType &error, std::size_t bytes_recvd) { HandleMessage(error, bytes_recvd); });
      result.EmplaceValue();
    } else {
      // Socket binding failed
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__,
          [ec](std::stringstream &msg) { msg << "Udp Socket Bind failed with message: " << ec.message(); });
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Udp Socket Opening failed with error: " << ec.message(); });
  }
  return result;
}

core_type::Result<void, UdpClientSocket::UdpErrorCode> UdpClientSocket::Transmit(UdpMessageConstPtr udp_message) {
  core_type::Result<void, UdpErrorCode> result{UdpErrorCode::kGenericError};
  UdpErrorCodeType ec{};

  try {
    // Transmit to remote endpoints
    std::size_t send_size{udp_socket_.send_to(
        boost::asio::buffer(udp_message->GetTxBuffer(), std::size_t(udp_message->GetTxBuffer().size())),
        Udp::endpoint{UdpIpAddress::from_string(std::string{udp_message->GetHostIpAddress()}),
                      udp_message->GetHostPortNumber()})};
    // Check for error
    if (send_size == udp_message->GetTxBuffer().size()) {
      // successful
      Udp::endpoint endpoint{udp_socket_.local_endpoint()};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [&udp_message, endpoint](std::stringstream &msg) {
            msg << "Udp message sent : "
                << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">"
                << " -> "
                << "<" << udp_message->GetHostIpAddress() << "," << udp_message->GetHostPortNumber() << ">";
          });
      result.EmplaceValue();
      // start async receive
      udp_socket_.async_receive_from(
          boost::asio::buffer(rx_buffer_), remote_endpoint_,
          [this](const UdpErrorCodeType &error, std::size_t bytes_received) { HandleMessage(error, bytes_received); });
    }
  } catch (boost::system::system_error const &ec) {
    UdpErrorCodeType error = ec.code();
    std::cerr << error.message() << "\n";
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [error, &udp_message](std::stringstream &msg) {
          msg << "Udp message sending to "
              << "<" << udp_message->GetHostIpAddress() << "> "
              << "failed with error: " << error.message();
        });
  }
  return result;
}

core_type::Result<void, UdpClientSocket::UdpErrorCode> UdpClientSocket::Destroy() {
  core_type::Result<void, UdpErrorCode> result{UdpErrorCode::kGenericError};
  // destroy the socket
  udp_socket_.close();
  running_ = false;
  io_context_.stop();
  result.EmplaceValue();
  return result;
}

// function invoked when datagram is received
void UdpClientSocket::HandleMessage(const UdpErrorCodeType &error, std::size_t bytes_received) {
  // Check for error
  if (error.value() == boost::system::errc::success) {
    if (local_ip_address_ != remote_endpoint_.address().to_string()) {
      UdpMessage::BufferType received_data{};
      received_data.reserve(bytes_received);
      // copy the received bytes into local buffer
      received_data.insert(received_data.begin(), rx_buffer_.begin(), rx_buffer_.begin() + bytes_received);

      UdpMessagePtr udp_rx_message{std::make_unique<UdpMessage>(remote_endpoint_.address().to_string(),
                                                                remote_endpoint_.port(), std::move(received_data))};

      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogInfo(
          __FILE__, __LINE__, __func__, [this, &udp_rx_message](std::stringstream &msg) {
            msg << "Udp Message received: "
                << "<" << udp_rx_message->GetHostIpAddress() << "," << udp_rx_message->GetHostPortNumber() << ">"
                << " -> "
                << "<" << local_ip_address_ << "," << local_port_num_ << ">";
          });

      // send data to upper layer
      udp_handler_read_(std::move(udp_rx_message));
      // start async receive
      udp_socket_.async_receive_from(
          boost::asio::buffer(rx_buffer_), remote_endpoint_,
          [this](const UdpErrorCodeType &error, std::size_t bytes_received) { HandleMessage(error, bytes_received); });
    } else {
      Udp::endpoint endpoint_{remote_endpoint_};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogVerbose(
          __FILE__, __LINE__, __func__, [endpoint_, this](std::stringstream &msg) {
            msg << "Udp Message received from "
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">"
                << " ignored as received by self ip"
                << " <" << local_ip_address_ << ">";
          });
    }
  } else {
    if (error.value() != boost::asio::error::operation_aborted) {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__, [error, this](std::stringstream &msg) {
            msg << "<" << local_ip_address_ << ">: "
                << "Remote Disconnected with undefined error: " << error.message();
          });
    }
  }
}

}  // namespace udp
}  // namespace socket
}  // namespace boost_support
