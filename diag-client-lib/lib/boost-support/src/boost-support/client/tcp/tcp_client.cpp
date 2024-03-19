/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/client/tcp/tcp_client.h"

#include <variant>

#include "boost-support/common/logger.h"
#include "boost-support/connection/tcp/tcp_connection.h"
#include "boost-support/error_domain/boost_support_error_domain.h"
#include "boost-support/socket/io_context.h"
#include "boost-support/socket/tcp/tcp_socket.h"
#include "boost-support/socket/tls/tls_socket.h"
#include "core/include/variant_helper.h"

namespace boost_support {
namespace client {
namespace tcp {

/**
 * @brief    Class to provide implementation of tcp client
 */
class TcpClient::TcpClientImpl final {
 private:
  /**
   * @brief     Type alias for unsecured tcp connection
   */
  using TcpConnectionUnsecured =
      connection::tcp::TcpConnection<connection::tcp::ConnectionType::kClient, socket::tcp::TcpSocket>;

  /**
   * @brief     Type alias for secured socket with Tls version 1.2
   */
  using TlsSocket12 = socket::tls::TlsSocket<socket::tls::TlsVersionType::kTls12>;

  /**
   * @brief     Type alias for secured tcp connection with tls version 1.2
   */
  using TcpConnectionSecuredTls12 =
      connection::tcp::TcpConnection<connection::tcp::ConnectionType::kClient, TlsSocket12>;

  /**
   * @brief     Type alias for secured socket with Tls version 1.3
   */
  using TlsSocket13 = socket::tls::TlsSocket<socket::tls::TlsVersionType::kTls13>;

  /**
   * @brief     Type alias for secured tcp connection with tls version 1.3
   */
  using TcpConnectionSecuredTls13 =
      connection::tcp::TcpConnection<connection::tcp::ConnectionType::kClient, TlsSocket13>;

  /**
   * @brief  Definitions of different connection state
   */
  enum class State : std::uint8_t {
    kConnected = 0U,   /**< Connected to remote server */
    kDisconnected = 1U /**< Disconnected from remote server */
  };

  /**
   * @brief  Type alias for boost context
   */
  using IoContext = boost_support::socket::IoContext;

 public:
  /**
   * @brief         Constructs an instance of TcpClientImpl
   * @param[in]     local_ip_address
   *                The local ip address of client
   * @param[in]     local_port_num
   *                The local port number of client
   */
  TcpClientImpl(std::string_view local_ip_address, std::uint16_t local_port_num) noexcept
      : io_context_{},
        connection_state_{State::kDisconnected},
        tcp_connection_{std::in_place_type<TcpConnectionUnsecured>,
                        socket::tcp::TcpSocket{local_ip_address, local_port_num, io_context_.GetContext()}} {}

  /**
   * @brief         Constructs an instance of TcpClient
   * @param[in]     local_ip_address
   *                The local ip address of client
   * @param[in]     local_port_num
   *                The local port number of client
   */
  TcpClientImpl(TlsVersion12, std::string_view local_ip_address, std::uint16_t local_port_num,
                std::string_view ca_certification_path) noexcept
      : io_context_{},
        connection_state_{State::kDisconnected},
        tcp_connection_{
            std::in_place_type<TcpConnectionSecuredTls12>,
            TlsSocket12{local_ip_address, local_port_num, ca_certification_path, io_context_.GetContext()}} {}

  /**
   * @brief         Constructs an instance of TcpClient
   * @param[in]     local_ip_address
   *                The local ip address of client
   * @param[in]     local_port_num
   *                The local port number of client
   */
  TcpClientImpl(TlsVersion13, std::string_view local_ip_address, std::uint16_t local_port_num,
                std::string_view ca_certification_path) noexcept
      : io_context_{},
        connection_state_{State::kDisconnected},
        tcp_connection_{
            std::in_place_type<TcpConnectionSecuredTls13>,
            TlsSocket13{local_ip_address, local_port_num, ca_certification_path, io_context_.GetContext()}} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TcpClientImpl(const TcpClientImpl &other) noexcept = delete;
  TcpClientImpl &operator=(const TcpClientImpl &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  TcpClientImpl(TcpClientImpl &&other) noexcept = delete;
  TcpClientImpl &operator=(TcpClientImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of TcpClientImpl
   */
  ~TcpClientImpl() noexcept = default;

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept {
    std::visit(core_type::visit::overloaded{
                   [](TcpConnectionUnsecured &tcp_connection) noexcept { tcp_connection.Initialize(); },
                   [](TcpConnectionSecuredTls12 &tcp_connection) noexcept { tcp_connection.Initialize(); },
                   [](TcpConnectionSecuredTls13 &tcp_connection) noexcept { tcp_connection.Initialize(); }},
               tcp_connection_);
  }

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept {
    std::visit(core_type::visit::overloaded{
                   [](TcpConnectionUnsecured &tcp_connection) noexcept { tcp_connection.DeInitialize(); },
                   [](TcpConnectionSecuredTls12 &tcp_connection) noexcept { tcp_connection.DeInitialize(); },
                   [](TcpConnectionSecuredTls13 &tcp_connection) noexcept { tcp_connection.DeInitialize(); }},
               tcp_connection_);
  }

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) noexcept {
    std::visit(core_type::visit::overloaded{[&read_handler](TcpConnectionUnsecured &tcp_connection) noexcept {
                                              tcp_connection.SetReadHandler(std::move(read_handler));
                                            },
                                            [&read_handler](TcpConnectionSecuredTls12 &tcp_connection) noexcept {
                                              tcp_connection.SetReadHandler(std::move(read_handler));
                                            },
                                            [&read_handler](TcpConnectionSecuredTls13 &tcp_connection) noexcept {
                                              tcp_connection.SetReadHandler(std::move(read_handler));
                                            }},
               tcp_connection_);
  }

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
    core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::BoostSupportErrorErrc::kSocketError)};
    if (connection_state_.load(std::memory_order_seq_cst) != State::kConnected) {
      if (std::visit(core_type::visit::overloaded{
                         [host_ip_address, host_port_num](TcpConnectionUnsecured &tcp_connection) noexcept {
                           return tcp_connection.ConnectToHost(host_ip_address, host_port_num);
                         },
                         [host_ip_address, host_port_num](TcpConnectionSecuredTls12 &tcp_connection) noexcept {
                           return tcp_connection.ConnectToHost(host_ip_address, host_port_num);
                         },
                         [host_ip_address, host_port_num](TcpConnectionSecuredTls13 &tcp_connection) noexcept {
                           return tcp_connection.ConnectToHost(host_ip_address, host_port_num);
                         }},
                     tcp_connection_)) {
        connection_state_.store(State::kConnected, std::memory_order_seq_cst);
        result.EmplaceValue();
      }  // else, connect failed
    } else {
      // already connected
      result.EmplaceValue();
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogVerbose(
          __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Tcp client is already connected"; });
    }
    return result;
  }

  /**
   * @brief         Function to disconnect from remote host if already connected
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> DisconnectFromHost() {
    core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::BoostSupportErrorErrc::kSocketError)};
    if (connection_state_.load(std::memory_order_seq_cst) == State::kConnected) {
      std::visit(core_type::visit::overloaded{
                     [](TcpConnectionUnsecured &tcp_connection) noexcept { tcp_connection.DisconnectFromHost(); },
                     [](TcpConnectionSecuredTls12 &tcp_connection) noexcept { tcp_connection.DisconnectFromHost(); },
                     [](TcpConnectionSecuredTls13 &tcp_connection) noexcept { tcp_connection.DisconnectFromHost(); }},
                 tcp_connection_);
      connection_state_.store(State::kDisconnected, std::memory_order_seq_cst);
      result.EmplaceValue();
    } else {
      // Not connected
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Tcp client is in disconnected state"; });
    }
    return result;
  }

  /**
   * @brief         Function to get the connection status
   * @return        True if connected, False otherwise
   */
  auto IsConnectedToHost() const noexcept -> bool {
    return (connection_state_.load(std::memory_order_seq_cst) == State::kConnected);
  }

  /**
   * @brief         Function to transmit the provided tcp message
   * @param[in]     tcp_message
   *                The tcp message
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(MessageConstPtr tcp_message) {
    core_type::Result<void> result{error_domain::MakeErrorCode(error_domain::BoostSupportErrorErrc::kGenericError)};
    if (connection_state_.load(std::memory_order_seq_cst) == State::kConnected) {
      if (std::visit(core_type::visit::overloaded{[&tcp_message](TcpConnectionUnsecured &tcp_connection) noexcept {
                                                    return tcp_connection.Transmit(std::move(tcp_message));
                                                  },
                                                  [&tcp_message](TcpConnectionSecuredTls12 &tcp_connection) noexcept {
                                                    return tcp_connection.Transmit(std::move(tcp_message));
                                                  },
                                                  [&tcp_message](TcpConnectionSecuredTls13 &tcp_connection) noexcept {
                                                    return tcp_connection.Transmit(std::move(tcp_message));
                                                  }},
                     tcp_connection_)) {
        result.EmplaceValue();
      }
    } else {
      // not connected
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__,
          [](std::stringstream &msg) { msg << "Tcp client is Offline, please connect to server first"; });
    }
    return result;
  }

 private:
  /**
   * @brief  Stores the io context
   */
  IoContext io_context_;

  /**
   * @brief  Store the state of tcp connection
   */
  std::atomic<State> connection_state_;

  /**
   * @brief      Store the tcp connection
   */
  std::variant<TcpConnectionUnsecured, TcpConnectionSecuredTls12, TcpConnectionSecuredTls13> tcp_connection_;
};

TcpClient::TcpClient(std::string_view local_ip_address, std::uint16_t local_port_num) noexcept
    : tcp_client_impl_{std::make_unique<TcpClientImpl>(local_ip_address, local_port_num)} {}

TcpClient::TcpClient(TlsVersion12, std::string_view local_ip_address, std::uint16_t local_port_num,
                     std::string_view ca_certification_path) noexcept
    : tcp_client_impl_{
          std::make_unique<TcpClientImpl>(TlsVersion12{}, local_ip_address, local_port_num, ca_certification_path)} {}

TcpClient::TcpClient(TlsVersion13, std::string_view local_ip_address, std::uint16_t local_port_num,
                     std::string_view ca_certification_path) noexcept
    : tcp_client_impl_{
          std::make_unique<TcpClientImpl>(TlsVersion13{}, local_ip_address, local_port_num, ca_certification_path)} {}

TcpClient::TcpClient(TcpClient &&other) noexcept = default;

TcpClient &TcpClient::operator=(TcpClient &&other) noexcept = default;

TcpClient::~TcpClient() noexcept = default;

void TcpClient::Initialize() noexcept { tcp_client_impl_->Initialize(); }

void TcpClient::DeInitialize() noexcept { tcp_client_impl_->DeInitialize(); }

void TcpClient::SetReadHandler(TcpClient::HandlerRead read_handler) noexcept {
  tcp_client_impl_->SetReadHandler(std::move(read_handler));
}

core_type::Result<void> TcpClient::ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) {
  return tcp_client_impl_->ConnectToHost(host_ip_address, host_port_num);
}

core_type::Result<void> TcpClient::DisconnectFromHost() { return tcp_client_impl_->DisconnectFromHost(); }

auto TcpClient::IsConnectedToHost() const noexcept -> bool { return tcp_client_impl_->IsConnectedToHost(); }

core_type::Result<void> TcpClient::Transmit(MessageConstPtr tcp_message) {
  return tcp_client_impl_->Transmit(std::move(tcp_message));
}

}  // namespace tcp
}  // namespace client
}  // namespace boost_support
