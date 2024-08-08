/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "boost-support/server/tls/tls_acceptor.h"

#include <boost/asio.hpp>

#include "boost-support/common/logger.h"
#include "boost-support/server/tls/tls_version.h"
#include "boost-support/socket/tls/tls_context.h"
#include "boost-support/socket/tls/tls_socket.h"

namespace boost_support {
namespace server {
namespace tls {
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

template<typename TlsVersion>
class TlsAcceptor<TlsVersion>::TlsAcceptorImpl final {
 public:
  /**
   * @brief  Type alias for tcp secured socket
   */
  using TlsSocket = socket::tls::TlsSocket;

  /**
   * @brief  Type alias for tls context
   */
  using TlsContext = socket::tls::TlsContext;

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
  TlsAcceptorImpl(std::string_view local_ip_address, std::uint16_t local_port_num,
                  std::uint8_t maximum_connection, TlsVersion tls_version,
                  std::string_view certificate_path, std::string_view private_key_path) noexcept
      : io_context_{},
        tls_context_{std::forward<TlsVersion>(tls_version), certificate_path, private_key_path},
        acceptor_{io_context_,
                  Tcp::endpoint(TcpIpAddress::from_string(std::string{local_ip_address}.c_str()),
                                local_port_num)} {
    acceptor_.listen(maximum_connection);
  }

  /**
   * @brief         Get a tls server ready to communicate
   * @details       This blocks until new server is created
   * @return        Tcp server object on success, else nothing
   */
  std::optional<TlsServer> GetTlsServer() noexcept {
    using TcpErrorCodeType = boost::system::error_code;
    std::optional<TlsServer> tls_server{};
    TcpErrorCodeType ec{};
    Tcp::endpoint endpoint{};

    // blocking accept
    TlsSocket::TcpSocket accepted_socket{acceptor_.accept(endpoint, ec)};
    if (ec.value() == boost::system::errc::success) {
      tls_server.emplace(TlsSocket{std::move(accepted_socket), tls_context_});
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [&endpoint](std::stringstream &msg) {
            msg << "Tls socket connection received from client "
                << "<" << endpoint.address().to_string() << "," << endpoint.port() << ">";
          });
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Tcp socket accept failed with error: " << ec.message();
          });
    }
    return tls_server;
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
   * @brief  Store the tls context
   */
  TlsContext tls_context_;

  /**
   * @brief  Store the tcp acceptor
   */
  Acceptor acceptor_;
};

template<typename TlsVersion>
TlsAcceptor<TlsVersion>::TlsAcceptor(std::string_view local_ip_address,
                                     std::uint16_t local_port_num, std::uint8_t maximum_connection,
                                     TlsVersion tls_version, std::string_view certificate_path,
                                     std::string_view private_key_path) noexcept
    : tls_acceptor_impl_{std::make_unique<TlsAcceptorImpl>(
          local_ip_address, local_port_num, maximum_connection, std::move(tls_version),
          certificate_path, private_key_path)} {}

template<typename TlsVersion>
TlsAcceptor<TlsVersion>::~TlsAcceptor() noexcept = default;

template<typename TlsVersion>
std::optional<TlsServer> TlsAcceptor<TlsVersion>::GetTlsServer() noexcept {
  return tls_acceptor_impl_->GetTlsServer();
}

template class TlsAcceptor<TlsVersion13>;
template class TlsAcceptor<TlsVersion12>;

}  // namespace tls
}  // namespace server
}  // namespace boost_support
