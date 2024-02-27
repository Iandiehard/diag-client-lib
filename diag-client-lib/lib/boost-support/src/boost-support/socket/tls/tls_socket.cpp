/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/socket/tls/tls_socket.h"

#include <utility>

#include "boost-support/common/logger.h"

namespace boost_support {
namespace socket {
namespace tls {

template<TlsVersionType TlsVersion>
TlsSocket<TlsVersion>::TlsSocket(std::string_view local_ip_address, std::uint16_t local_port_num,
                                 std::string_view ca_certification_path, boost::asio::io_context &io_context) noexcept
    : TlsContext<TlsVersion>{ca_certification_path, io_context},
      local_ip_address_{local_ip_address},
      local_port_num_{local_port_num},
      tcp_socket_{io_context, this->GetContext()} {}

template<TlsVersionType TlsVersion>
TlsSocket<TlsVersion>::TlsSocket(TlsSocket &&other) noexcept
    : TlsContext<TlsVersion>(std::move(other)),
      local_ip_address_{std::move(other.local_ip_address_)},
      local_port_num_{std::move(other.local_port_num_)},
      tcp_socket_{std::move(other.tcp_socket_)} {}

template<TlsVersionType TlsVersion>
TlsSocket<TlsVersion> &TlsSocket<TlsVersion>::operator=(TlsSocket &&other) noexcept {
  TlsContext<TlsVersion>::operator=(std::move(other));
  local_ip_address_ = std::move(other.local_ip_address_);
  local_port_num_ = std::move(other.local_port_num_);
  tcp_socket_ = std::move(other.tcp_socket_);
  return *this;
}

template<TlsVersionType TlsVersion>
TlsSocket<TlsVersion>::~TlsSocket() noexcept = default;

template<TlsVersionType TlsVersion>
core_type::Result<void, typename TlsSocket<TlsVersion>::SocketError> TlsSocket<TlsVersion>::Open() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  // Open the socket
  GetNativeTcpSocket().open(Tcp::v4(), ec);
  if (ec.value() == boost::system::errc::success) {
    // Reuse address
    GetNativeTcpSocket().set_option(boost::asio::socket_base::reuse_address{true});
    // Set socket to non blocking
    GetNativeTcpSocket().non_blocking(false);
    // Bind to local ip address and random port
    GetNativeTcpSocket().bind(Tcp::endpoint(TcpIpAddress::from_string(local_ip_address_), local_port_num_), ec);

    if (ec.value() == boost::system::errc::success) {
      // Socket binding success
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
            Tcp::endpoint const endpoint_{GetNativeTcpSocket().local_endpoint()};
            msg << "Tcp Socket opened and bound to "
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
          });
      result.EmplaceValue();
    } else {
      // Socket binding failed
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__,
          [ec](std::stringstream &msg) { msg << "Tcp Socket binding failed with message: " << ec.message(); });
      result.EmplaceError(SocketError::kBindingFailed);
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp Socket opening failed with error: " << ec.message(); });
    result.EmplaceError(SocketError::kOpenFailed);
  }
  return result;
}

template<TlsVersionType TlsVersion>
core_type::Result<void, typename TlsSocket<TlsVersion>::SocketError> TlsSocket<TlsVersion>::Connect(
    std::string_view host_ip_address, std::uint16_t host_port_num) noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  // Connect to provided Ip address
  GetNativeTcpSocket().connect(Tcp::endpoint(TcpIpAddress::from_string(std::string{host_ip_address}), host_port_num),
                               ec);
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          Tcp::endpoint const endpoint_{GetNativeTcpSocket().remote_endpoint()};
          msg << "Tcp Socket connected to host "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    // Perform TLS handshake
    tcp_socket_.handshake(boost::asio::ssl::stream_base::client, ec);
    if (ec.value() == boost::system::errc::success) {
      printf("Connected with %s encryption\n", SSL_get_cipher(tcp_socket_.native_handle()));
      result.EmplaceValue();
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Tls client handshake with host failed with error: " << ec.message();
          });
      result.EmplaceError(SocketError::kTlsHandshakeFailed);
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp Socket connect to host failed with error: " << ec.message(); });
  }
  return result;
}

template<TlsVersionType TlsVersion>
core_type::Result<void, typename TlsSocket<TlsVersion>::SocketError> TlsSocket<TlsVersion>::Disconnect() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};
  // Shutdown TLS connection
  tcp_socket_.shutdown(ec);
  // Shutdown of TCP connection
  GetNativeTcpSocket().shutdown(Tcp::socket ::shutdown_both, ec);

  if (ec.value() == boost::system::errc::success) {
    // Socket shutdown success
    result.EmplaceValue();
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp Socket disconnection from host failed with error: " << ec.message();
        });
  }
  return result;
}

template<TlsVersionType TlsVersion>
core_type::Result<void, typename TlsSocket<TlsVersion>::SocketError> TlsSocket<TlsVersion>::Transmit(
    TcpMessageConstPtr tcp_message) noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  TcpErrorCodeType ec{};

  boost::asio::write(tcp_socket_, boost::asio::buffer(tcp_message->GetTxBuffer(), tcp_message->GetTxBuffer().size()),
                     ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [this](std::stringstream &msg) {
          Tcp::endpoint const endpoint_{GetNativeTcpSocket().remote_endpoint()};
          msg << "Tcp message sent to "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    result.EmplaceValue();
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp message sending failed with error: " << ec.message(); });
  }
  return result;
}

template<TlsVersionType TlsVersion>
core_type::Result<void, typename TlsSocket<TlsVersion>::SocketError> TlsSocket<TlsVersion>::Close() noexcept {
  core_type::Result<void, SocketError> result{SocketError::kGenericError};
  // Destroy the socket
  GetNativeTcpSocket().close();
  result.EmplaceValue();
  return result;
}

template<TlsVersionType TlsVersion>
core_type::Result<typename TlsSocket<TlsVersion>::TcpMessagePtr, typename TlsSocket<TlsVersion>::SocketError>
TlsSocket<TlsVersion>::Read() noexcept {
  core_type::Result<TcpMessagePtr, SocketError> result{SocketError::kRemoteDisconnected};
  TcpErrorCodeType ec{};
  // create and reserve the buffer
  TcpMessage::BufferType rx_buffer{};
  rx_buffer.resize(message::tcp::kDoipheadrSize);
  // start blocking read to read Header first
  boost::asio::read(tcp_socket_, boost::asio::buffer(&rx_buffer[0u], message::tcp::kDoipheadrSize), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    // read the next bytes to read
    std::uint32_t const read_next_bytes = [&rx_buffer]() noexcept -> std::uint32_t {
      return static_cast<std::uint32_t>((static_cast<std::uint32_t>(rx_buffer[4u] << 24u) & 0xFF000000) |
                                        (static_cast<std::uint32_t>(rx_buffer[5u] << 16u) & 0x00FF0000) |
                                        (static_cast<std::uint32_t>(rx_buffer[6u] << 8u) & 0x0000FF00) |
                                        (static_cast<std::uint32_t>(rx_buffer[7u] & 0x000000FF)));
    }();

    if (read_next_bytes != 0u) {
      // reserve the buffer
      rx_buffer.resize(message::tcp::kDoipheadrSize + std::size_t(read_next_bytes));
      boost::asio::read(tcp_socket_, boost::asio::buffer(&rx_buffer[message::tcp::kDoipheadrSize], read_next_bytes),
                        ec);

      // all message received, transfer to upper layer
      Tcp::endpoint const endpoint_{GetNativeTcpSocket().remote_endpoint()};
      TcpMessagePtr tcp_rx_message{
          std::make_unique<TcpMessage>(endpoint_.address().to_string(), endpoint_.port(), std::move(rx_buffer))};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
            msg << "Tcp Message received from "
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
          });
      result.EmplaceValue(std::move(tcp_rx_message));
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__,
          [](std::stringstream &msg) { msg << "Tcp Message read ignored as header size is zero"; });
    }

  } else if (ec.value() == boost::asio::error::eof) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with: " << ec.message(); });
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with undefined error: " << ec.message(); });
  }
  return result;
}

template<TlsVersionType TlsVersion>
TlsSocket<TlsVersion>::Socket::lowest_layer_type &TlsSocket<TlsVersion>::GetNativeTcpSocket() {
  return tcp_socket_.lowest_layer();
}

}  // namespace tls
}  // namespace socket
}  // namespace boost_support
