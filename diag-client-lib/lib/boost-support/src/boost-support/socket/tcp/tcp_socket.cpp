/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/socket/tcp/tcp_socket.h"

#include <utility>

#include "boost-support/common/logger.h"

namespace boost_support {
namespace socket {
namespace tcp {

TcpSocket::TcpSocket(std::string_view local_ip_address, std::uint16_t local_port_num,
                     IoContext &io_context) noexcept
    : tcp_socket_{io_context.GetContext()},
      local_endpoint_{boost::asio::ip::make_address(local_ip_address), local_port_num} {}

TcpSocket::TcpSocket(TcpSocket::Socket socket) noexcept
    : tcp_socket_{std::move(socket)},
      local_endpoint_{tcp_socket_.local_endpoint()} {}

TcpSocket::~TcpSocket() noexcept = default;

core_type::Result<void, TcpSocket::SocketError> TcpSocket::Open() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  // Open the socket
  tcp_socket_.open(local_endpoint_.protocol(), ec);
  if (ec.value() == boost::system::errc::success) {
    // reuse address
    tcp_socket_.set_option(boost::asio::socket_base::reuse_address{true});
    // Set socket to non blocking
    tcp_socket_.non_blocking(false);
    // Bind to local ip address and random port
    tcp_socket_.bind(local_endpoint_, ec);

    if (ec.value() == boost::system::errc::success) {
      local_endpoint_ = tcp_socket_.local_endpoint();
      // Socket binding success
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__, [this](std::stringstream &msg) {
            Tcp::endpoint const endpoint_{tcp_socket_.local_endpoint()};
            msg << "Tcp Socket opened and bound to "
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
          });
      result.EmplaceValue();
    } else {
      // Socket binding failed
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Tcp Socket binding failed with message: " << ec.message();
          });
      result.EmplaceError(SocketError::kBindingFailed);
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp Socket opening failed with error: " << ec.message();
        });
    result.EmplaceError(SocketError::kOpenFailed);
  }
  return result;
}

core_type::Result<void, TcpSocket::SocketError> TcpSocket::Connect(
    std::string_view host_ip_address, std::uint16_t host_port_num) noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  // Connect to provided Ip address
  tcp_socket_.connect(
      Tcp::endpoint(TcpIpAddress::from_string(std::string{host_ip_address}), host_port_num), ec);
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        FILE_NAME, __LINE__, __func__, [this](std::stringstream &msg) {
          Tcp::endpoint const endpoint_{tcp_socket_.remote_endpoint()};
          msg << "Tcp Socket connected to host "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    result.EmplaceValue();
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp Socket connect to host failed with error: " << ec.message();
        });
  }
  return result;
}

core_type::Result<void, TcpSocket::SocketError> TcpSocket::Disconnect() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  // Graceful shutdown
  tcp_socket_.shutdown(Socket::shutdown_both, ec);
  if (ec.value() == boost::system::errc::success) {
    // Socket shutdown success
    result.EmplaceValue();
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp Socket disconnection from host failed with error: " << ec.message();
        });
  }
  return result;
}

core_type::Result<void, TcpSocket::SocketError> TcpSocket::Transmit(
    TcpMessageConstPtr tcp_message) noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  boost::asio::write(
      tcp_socket_,
      boost::asio::buffer(tcp_message->GetPayload().data(), tcp_message->GetPayload().size()), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        FILE_NAME, __LINE__, __func__, [this](std::stringstream &msg) {
          Tcp::endpoint const endpoint_{tcp_socket_.remote_endpoint()};
          msg << "Tcp message sent to "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    result.EmplaceValue();
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp message sending failed with error: " << ec.message();
        });
  }
  return result;
}

core_type::Result<void, TcpSocket::SocketError> TcpSocket::Close() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  // destroy the socket
  tcp_socket_.shutdown(boost::asio::socket_base::shutdown_receive);
  tcp_socket_.close();
  result.EmplaceValue();
  return result;
}

core_type::Result<TcpSocket::TcpMessagePtr, TcpSocket::SocketError> TcpSocket::Read() noexcept {
  core_type::Result<TcpMessagePtr, SocketError> result{SocketError::kRemoteDisconnected};
  TcpErrorCodeType ec{};
  // create and reserve the buffer
  TcpMessage::BufferType rx_buffer{};
  rx_buffer.resize(message::tcp::kDoipheadrSize);
  // start blocking read to read Header first
  boost::asio::read(tcp_socket_, boost::asio::buffer(&rx_buffer[0u], message::tcp::kDoipheadrSize),
                    ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    // read the next bytes to read
    std::uint32_t const read_next_bytes{[&rx_buffer]() noexcept -> std::uint32_t {
      return static_cast<std::uint32_t>(rx_buffer[4u] << 24u) & 0xFF000000 |
             static_cast<std::uint32_t>(rx_buffer[5u] << 16u) & 0x00FF0000 |
             static_cast<std::uint32_t>(rx_buffer[6u] << 8u) & 0x0000FF00 |
             static_cast<std::uint32_t>(rx_buffer[7u] & 0x000000FF);
    }()};

    if (read_next_bytes != 0u) {
      // reserve the buffer
      rx_buffer.resize(message::tcp::kDoipheadrSize + std::size_t(read_next_bytes));
      boost::asio::read(
          tcp_socket_,
          boost::asio::buffer(&rx_buffer[message::tcp::kDoipheadrSize], read_next_bytes), ec);

      Tcp::endpoint const remote_endpoint{tcp_socket_.remote_endpoint()};
      TcpMessagePtr tcp_rx_message{std::make_unique<TcpMessage>(
          remote_endpoint.address().to_string(), remote_endpoint.port(), std::move(rx_buffer))};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__, [&remote_endpoint](std::stringstream &msg) {
            msg << "Tcp Message received from "
                << "<" << remote_endpoint.address().to_string() << "," << remote_endpoint.port()
                << ">";
          });
      result.EmplaceValue(std::move(tcp_rx_message));
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__,
          [](std::stringstream &msg) { msg << "Tcp Message read ignored as header size is zero"; });
    }
  } else if (ec.value() == boost::asio::error::eof) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        FILE_NAME, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with: " << ec.message(); });
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Remote Disconnected with undefined error: " << ec.message();
        });
  }
  return result;
}
}  // namespace tcp
}  // namespace socket
}  // namespace boost_support
