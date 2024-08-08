/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "boost-support/socket/tcp/tls_server_.h"

#include <utility>

#include "boost-support/common/logger.h"

namespace boost_support {
namespace socket {
namespace tcp {
namespace {
using TcpErrorCodeType = boost::system::error_code;
}  // namespace

TlsServerSocket::TlsServerSocket(std::string_view local_ip_address, std::uint16_t local_port_num)
    : local_ip_address_{local_ip_address},
      local_port_num_{local_port_num},
      io_context_{},
      io_ssl_context_{boost::asio::ssl::context::tlsv13_server},
      tcp_acceptor_{io_context_, Tcp::endpoint(Tcp::v4(), local_port_num_), true} {
  common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
      __FILE__, __LINE__, __func__, [&local_ip_address, &local_port_num](std::stringstream &msg) {
        msg << "Tcp Socket Acceptor created at "
            << "<" << local_ip_address << "," << local_port_num << ">";
      });
  // Load certificate and private key from provided locations
  io_ssl_context_.use_certificate_chain_file("../../../openssl/DiagClientLib.crt");
  io_ssl_context_.use_private_key_file("../../../openssl/DiagClientLib.key",
                                       boost::asio::ssl::context::pem);
  SSL_CTX_set_ciphersuites(io_ssl_context_.native_handle(),
                           "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");
}

std::optional<TcpServerConnection> TlsServerSocket::GetTcpServerConnection(
    TcpHandlerRead tcp_handler_read) {
  std::optional<TcpServerConnection> tcp_connection{std::nullopt};
  TcpErrorCodeType ec{};
  Tcp::endpoint endpoint{};
  TcpServerConnection::TlsStream tls_socket{io_context_, io_ssl_context_};

  // blocking accept
  tcp_acceptor_.accept(tls_socket.lowest_layer(), endpoint, ec);
  if (ec.value() == boost::system::errc::success) {
    tcp_connection.emplace(std::move(tls_socket), std::move(tcp_handler_read));
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [&endpoint](std::stringstream &msg) {
          msg << "TLS Socket connection received from client "
              << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
        });
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "TLS Socket Connect to client failed with error: " << ec.message();
        });
  }
  return tcp_connection;
}

TcpServerConnection::TcpServerConnection(TlsStream tls_socket, TcpHandlerRead tcp_handler_read)
    : tls_socket_{std::move(tls_socket)},
      tcp_handler_read_{std::move(tcp_handler_read)} {}

TcpServerConnection::TlsStream::lowest_layer_type &TcpServerConnection::GetNativeTcpSocket() {
  return tls_socket_.lowest_layer();
}

core_type::Result<void, TcpServerConnection::TcpErrorCode> TcpServerConnection::Transmit(
    message::tcp::TcpMessageConstPtr tcp_tx_message) {
  TcpErrorCodeType ec{};
  core_type::Result<void, TcpErrorCode> result{TcpErrorCode::kGenericError};

  boost::asio::write(tls_socket_,
                     boost::asio::buffer(tcp_tx_message->GetPayload().data(),
                                         std::size_t(tcp_tx_message->GetPayload().size())),
                     ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    Tcp::endpoint endpoint_{GetNativeTcpSocket().remote_endpoint()};
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
          msg << "Tcp message sent to "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    result.EmplaceValue();
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp message sending failed with error: " << ec.message();
        });
  }
  return result;
}

bool TcpServerConnection::TryReceivingMessage() {
  TcpErrorCodeType ec{};
  bool connection_closed{false};

  // Perform TLS handshake
  tls_socket_.handshake(boost::asio::ssl::stream_base::server, ec);

  if (ec.value() == boost::system::errc::success) {
    // Create and reserve the buffer
    message::tcp::TcpMessage::BufferType rx_buffer{};
    rx_buffer.resize(message::tcp::kDoipheadrSize);
    // Start blocking read to read Header first
    boost::asio::read(tls_socket_, boost::asio::buffer(&rx_buffer[0], message::tcp::kDoipheadrSize),
                      ec);
    // Check for error
    if (ec.value() == boost::system::errc::success) {
      // Read the next bytes to read
      std::uint32_t const read_next_bytes = [&rx_buffer]() noexcept -> std::uint32_t {
        return static_cast<std::uint32_t>(
            (static_cast<std::uint32_t>(rx_buffer[4u] << 24u) & 0xFF000000) |
            (static_cast<std::uint32_t>(rx_buffer[5u] << 16u) & 0x00FF0000) |
            (static_cast<std::uint32_t>(rx_buffer[6u] << 8u) & 0x0000FF00) |
            (static_cast<std::uint32_t>(rx_buffer[7u] & 0x000000FF)));
      }();
      // reserve the buffer
      rx_buffer.resize(message::tcp::kDoipheadrSize + std::size_t(read_next_bytes));
      boost::asio::read(
          tls_socket_,
          boost::asio::buffer(&rx_buffer[message::tcp::kDoipheadrSize], read_next_bytes), ec);

      // all message received, transfer to upper layer
      Tcp::endpoint endpoint_{GetNativeTcpSocket().remote_endpoint()};
      message::tcp::TcpMessagePtr tcp_rx_message{std::make_unique<message::tcp::TcpMessage>(
          endpoint_.address().to_string(), endpoint_.port(), std::move(rx_buffer))};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
            msg << "Tcp Message received from "
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
          });
      // send data to upper layer
      tcp_handler_read_(std::move(tcp_rx_message));
    } else if (ec.value() == boost::asio::error::eof) {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__,
          [ec](std::stringstream &msg) { msg << "Remote Disconnected with: " << ec.message(); });
      connection_closed = true;
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Remote Disconnected with undefined error: " << ec.message();
          });
      connection_closed = true;
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tls server handshake with host failed with error: " << ec.message();
        });
    connection_closed = true;
  }
  return connection_closed;
}

core_type::Result<void, TcpServerConnection::TcpErrorCode> TcpServerConnection::Shutdown() {
  core_type::Result<void, TcpErrorCode> result{TcpErrorCode::kGenericError};
  TcpErrorCodeType ec{};

  // Graceful shutdown
  if (GetNativeTcpSocket().is_open()) {
    // Shutdown TLS connection
    tls_socket_.shutdown(ec);
    // Shutdown of TCP connection
    GetNativeTcpSocket().shutdown(TcpSocket::shutdown_both, ec);
    if (ec.value() == boost::system::errc::success) {
      // Socket shutdown success
      result.EmplaceValue();
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Tcp Socket Disconnection failed with error: " << ec.message();
          });
    }
    GetNativeTcpSocket().close();
  }
  return result;
}

}  // namespace tcp
}  // namespace socket
}  // namespace boost_support
