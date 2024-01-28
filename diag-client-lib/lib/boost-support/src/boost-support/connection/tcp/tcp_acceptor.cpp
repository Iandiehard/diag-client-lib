/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>
#include <utility>

#include "boost-support/connection/tcp/tcp_acceptor.h"
#include "boost-support/common/logger.h"

namespace boost_support {
namespace connection {
namespace tcp {
namespace {
/**
 * @brief  Type alias for tcp protocol
 */
using Tcp = boost::asio::ip::tcp;

/**
 * @brief  Type alias for tcp ip address
 */
using TcpIpAddress = boost::asio::ip::address;

}  // namespace

TcpAcceptor::TcpAcceptor(std::string_view local_ip_address, std::uint16_t local_port_num,
                         boost::asio::io_context io_context) noexcept
    : acceptor_{io_context,
                Tcp::endpoint(TcpIpAddress::from_string(std::string{local_ip_address}.c_str()), local_port_num)} {
  acceptor_.listen();
}

std::optional<TcpAcceptor::Connection> TcpAcceptor::GetConnection(TcpAcceptor::HandlerRead read_handler) noexcept {
  using TcpErrorCodeType = boost::system::error_code;
  std::optional<Connection> tcp_connection{};
  TcpErrorCodeType ec{};
  Tcp::endpoint endpoint{};

  // blocking accept
  TcpSocket::Socket accepted_socket{acceptor_.accept(endpoint, ec)};
  if (ec.value() == boost::system::errc::success) {
    tcp_connection.emplace(TcpSocket{std::move(accepted_socket)}, std::move(read_handler));
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [&endpoint](std::stringstream &msg) {
          msg << "Tcp socket connection received from client "
              << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
        });
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Tcp socket accept failed with error: " << ec.message(); });
  }
  return tcp_connection;
}

}  // namespace tcp
}  // namespace connection
}  // namespace boost_support
