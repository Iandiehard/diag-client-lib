/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "socket/tcp/tcp_server.h"

#include "common/logger.h"

namespace boost_support {
namespace socket {
namespace tcp {

using TcpIpAddress = boost::asio::ip::address;
using TcpErrorCodeType = boost::system::error_code;

CreateTcpServerSocket::CreateTcpServerSocket(std::string_view local_ip_address, uint16_t local_port_num)
    : local_ip_address_{local_ip_address},
      local_port_num_{local_port_num} {
  // Create accepter
  tcp_accepter_ = std::make_unique<TcpAccepter>(io_context_, Tcp::endpoint(Tcp::v4(), local_port_num_), true);
  common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
      __FILE__, __LINE__, __func__, [&local_ip_address, &local_port_num](std::stringstream &msg) {
        msg << "Tcp Socket Accepter created at "
            << "<" << local_ip_address << "," << local_port_num << ">";
      });
}

CreateTcpServerSocket::TcpServerConnection CreateTcpServerSocket::GetTcpServerConnection(
    TcpHandlerRead &&tcp_handler_read) {
  TcpErrorCodeType ec;
  Tcp::endpoint endpoint{};
  CreateTcpServerSocket::TcpServerConnection tcp_connection{io_context_, std::move(tcp_handler_read)};

  // blocking accept
  auto socket = tcp_accepter_->accept(tcp_connection.GetSocket(), endpoint, ec);
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint](std::stringstream &msg) {
          msg << "Tcp Socket connection received from client "
              << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
        });
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp Socket Connect to client failed with error: " << ec.message(); });
  }
  return tcp_connection;
}

CreateTcpServerSocket::TcpServerConnection::TcpServerConnection(boost::asio::io_context &io_context,
                                                                TcpHandlerRead &&tcp_handler_read)
    : tcp_socket_{io_context},
      tcp_handler_read_{tcp_handler_read} {}

TcpSocket &CreateTcpServerSocket::TcpServerConnection::GetSocket() { return tcp_socket_; }

bool CreateTcpServerSocket::TcpServerConnection::Transmit(TcpMessageConstPtr tcp_tx_message) {
  TcpErrorCodeType ec{};
  bool ret_val{false};
  boost::asio::write(
      tcp_socket_,
      boost::asio::buffer(tcp_tx_message->GetTxBuffer(), std::size_t(tcp_tx_message->GetTxBuffer().size())), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    Tcp::endpoint endpoint_{tcp_socket_.remote_endpoint()};
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
          msg << "Tcp message sent to "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    ret_val = true;
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp message sending failed with error: " << ec.message(); });
  }
  return ret_val;
}

bool CreateTcpServerSocket::TcpServerConnection::ReceivedMessage() {
  TcpErrorCodeType ec;
  bool connection_closed{false};

  // create and reserve the buffer
  TcpMessage::BufferType rx_buffer{};
  rx_buffer.resize(kDoipheadrSize);
  // start blocking read to read Header first
  boost::asio::read(tcp_socket_, boost::asio::buffer(&rx_buffer[0], kDoipheadrSize), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    // read the next bytes to read
    uint32_t read_next_bytes = [&rx_buffer]() noexcept -> std::uint32_t {
      return ((uint32_t) ((uint32_t) ((rx_buffer[4u] << 24) & 0xFF000000) |
                          (uint32_t) ((rx_buffer[5u] << 16) & 0x00FF0000) |
                          (uint32_t) ((rx_buffer[6u] << 8) & 0x0000FF00) | (uint32_t) ((rx_buffer[7u] & 0x000000FF))));
    }();
    // reserve the buffer
    rx_buffer.resize(kDoipheadrSize + std::size_t(read_next_bytes));
    boost::asio::read(tcp_socket_, boost::asio::buffer(&rx_buffer[kDoipheadrSize], read_next_bytes), ec);

    // all message received, transfer to upper layer
    Tcp::endpoint endpoint_{tcp_socket_.remote_endpoint()};
    TcpMessagePtr tcp_rx_message{
        std::make_unique<TcpMessage>(endpoint_.address().to_string(), endpoint_.port(), std::move(rx_buffer))};
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
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with undefined error: " << ec.message(); });
    connection_closed = true;
  }
  return connection_closed;
}

bool CreateTcpServerSocket::TcpServerConnection::Shutdown() {
  TcpErrorCodeType ec{};
  bool ret_val{false};
  // Graceful shutdown
  if (tcp_socket_.is_open()) {
    tcp_socket_.shutdown(TcpSocket::shutdown_both, ec);
    if (ec.value() == boost::system::errc::success) {
      ret_val = true;
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__,
          [ec](std::stringstream &msg) { msg << "Tcp Socket Disconnection failed with error: " << ec.message(); });
    }
    tcp_socket_.close();
  }
  return ret_val;
}

}  // namespace tcp
}  // namespace socket
}  // namespace boost_support
