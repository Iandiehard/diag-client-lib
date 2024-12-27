/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "boost-support/server/tcp/tcp_acceptor.h"

#include <boost/asio.hpp>

#include "boost-support/common/logger.h"
#include "boost-support/socket/tcp/tcp_socket.h"

namespace boost_support {
namespace server {
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

/**
 * @brief  Function to create server name
 */
std::string CreateServerName(std::string_view server_name, std::uint16_t server_count) {
  std::string final_server_name{server_name};
  final_server_name.append(std::to_string(server_count));
  return final_server_name;
}
}  // namespace

class TcpAcceptor::TcpAcceptorImpl final {
 public:
  /**
   * @brief  Type alias for tcp unsecured socket
   */
  using TcpSocket = socket::tcp::TcpSocket;

 public:
  /**
   * @brief         Constructs an instance of Acceptor
   * @details       Tcp connection shall be accepted on this ip address and port
   * @param[in]     local_ip_address
   *                The local ip address
   * @param[in]     local_port_num
   *                The local port number
   * @param[in]     maximum_connection
   *                The maximum number of accepted connection
   */
  TcpAcceptorImpl(std::string_view acceptor_name, std::string_view local_ip_address,
                  std::uint16_t local_port_num, std::uint8_t maximum_connection) noexcept
      : io_context_{},
        server_count_{0u},
        acceptor_name_{acceptor_name},
        acceptor_{
            io_context_,
            Tcp::endpoint(boost::asio::ip::make_address(std::string{local_ip_address}.c_str()),
                          local_port_num)} {
    acceptor_.listen(maximum_connection);
  }

  /**
   * @brief         Get a tcp server ready to communicate
   * @details       This blocks until new server is created
   * @return        Tcp server object on success, else nothing
   */
  std::optional<TcpServer> GetTcpServer() noexcept {
    using TcpErrorCodeType = boost::system::error_code;
    std::optional<TcpServer> tcp_server{};
    TcpErrorCodeType ec{};
    Tcp::endpoint endpoint{};

    // blocking accept
    TcpSocket::Socket accepted_socket{acceptor_.accept(endpoint, ec)};
    if (ec.value() == boost::system::errc::success) {
      tcp_server.emplace(CreateServerName(acceptor_name_, server_count_),
                         TcpSocket{std::move(accepted_socket)});
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__, [&endpoint](std::stringstream &msg) {
            msg << "Tcp socket connection received from client "
                << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
          });
      // increment the server count
      server_count_++;
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Tcp socket accept failed with error: " << ec.message();
          });
    }
    return tcp_server;
  }

 private:
  /**
   * @brief  Type alias for tcp acceptor
   */
  using Acceptor = boost::asio::ip::tcp::acceptor;

  /**
   * @brief  Store the io context
   */
  boost::asio::io_context io_context_;

  /**
   * @brief  Keeps the count of server created
   */
  std::uint16_t server_count_;

  /**
   * @brief  Store the name of the acceptor
   */
  std::string acceptor_name_;

  /**
   * @brief  Store the tcp acceptor
   */
  Acceptor acceptor_;
};

TcpAcceptor::TcpAcceptor(std::string_view acceptor_name, std::string_view local_ip_address,
                         std::uint16_t local_port_num, std::uint8_t maximum_connection) noexcept
    : tcp_acceptor_impl_{std::make_unique<TcpAcceptorImpl>(acceptor_name, local_ip_address,
                                                           local_port_num, maximum_connection)} {}

TcpAcceptor::~TcpAcceptor() noexcept = default;

std::optional<TcpServer> TcpAcceptor::GetTcpServer() noexcept {
  return tcp_acceptor_impl_->GetTcpServer();
}

}  // namespace tcp
}  // namespace server
}  // namespace boost_support
