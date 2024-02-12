/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/socket/udp/udp_socket.h"

#include <boost/asio/ip/address.hpp>

#include "boost-support/common/logger.h"
#include "boost-support/error_domain/boost_support_error_domain.h"

namespace boost_support {
namespace socket {
namespace udp {

UdpSocket::UdpSocket(std::string_view local_ip_address, std::uint16_t local_port_num,
                     boost::asio::io_context &io_context) noexcept
    : udp_socket_{io_context},
      local_endpoint_{boost::asio::ip::make_address(local_ip_address), local_port_num} {}

UdpSocket::~UdpSocket() noexcept = default;

core_type::Result<void, UdpSocket::SocketError> UdpSocket::Open() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  UdpErrorCodeType ec{};

  udp_socket_.open(local_endpoint_.protocol(), ec);
  if (ec.value() == boost::system::errc::success) {
    // set broadcast option
    udp_socket_.set_option(boost::asio::socket_base::broadcast{true});
    // reuse address
    udp_socket_.set_option(boost::asio::socket_base::reuse_address{true});
    udp_socket_.bind(local_endpoint_, ec);
  }
  if (ec.value() == boost::system::errc::success) {
    Udp::endpoint endpoint{udp_socket_.local_endpoint()};
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint](std::stringstream &msg) {
          msg << "Udp Socket opened and bound to "
              << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
        });
    // start async receive
    StartReceivingMessage();
    result.EmplaceValue();
  } else {
    // Socket binding failed
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Udp Socket Bind failed with message: " << ec.message(); });
  }
  return result;
}

core_type::Result<void, UdpSocket::SocketError> UdpSocket::Transmit(UdpSocket::UdpMessageConstPtr udp_message) noexcept {
  return core_type::Result<void, SocketError>();
}

core_type::Result<UdpSocket::UdpMessagePtr> UdpSocket::Read(std::size_t total_bytes_received) {
  core_type::Result<UdpMessagePtr> result{
      error_domain::MakeErrorCode(error_domain::BoostSupportErrorErrc::kGenericError)};
  // Ignore self reception
  if (local_endpoint_.address().to_string() != remote_endpoint_.address().to_string()) {
    UdpMessage::BufferType received_data{};
    received_data.reserve(total_bytes_received);
    // copy the received bytes into local buffer
    received_data.insert(received_data.begin(), rx_buffer_.begin(), rx_buffer_.begin() + total_bytes_received);

    UdpMessagePtr udp_rx_message{std::make_unique<UdpMessage>(remote_endpoint_.address().to_string(),
                                                              remote_endpoint_.port(), std::move(received_data))};

    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogInfo(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          msg << "Udp Message received: "
              << "<" << remote_endpoint_.address() << "," << remote_endpoint_.port() << ">";
        });
    result.EmplaceValue(std::move(udp_rx_message));
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          msg << "Udp Message received from "
              << "<" << remote_endpoint_.address() << "," << remote_endpoint_.port() << ">"
              << " ignored as received by self ip"
              << " <" << local_endpoint_.address() << "," << local_endpoint_.port() << ">";
        });
  }
  return result;
}

void UdpSocket::StartReceivingMessage() {
  // start async receive
  udp_socket_.async_receive_from(boost::asio::buffer(rx_buffer_), remote_endpoint_,
                                 [this](const UdpErrorCodeType &error, std::size_t bytes_received) {
                                   if (error.value() == boost::system::errc::success) {
                                     static_cast<void>(Read(bytes_received).AndThen([this](UdpMessagePtr udp_message) {
                                       // send data to upper layer
                                       if (udp_handler_read_) { udp_handler_read_(std::move(udp_message)); }
                                       return core_type::Result<void>::FromValue();
                                     }));
                                   } else {
                                     {
                                       if (error.value() != boost::asio::error::operation_aborted) {
                                         common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
                                             __FILE__, __LINE__, __func__, [error](std::stringstream &msg) {
                                               msg << "Remote Disconnected with undefined error: " << error.message();
                                             });
                                       }
                                     }
                                   }
                                 });
}

}  // namespace udp
}  // namespace socket
}  // namespace boost_support
