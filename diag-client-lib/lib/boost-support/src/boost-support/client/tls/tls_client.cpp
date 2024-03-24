/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/client/tls/tls_client.h"

#include "boost-support/common/logger.h"
#include "boost-support/connection/tcp/tcp_connection.h"
#include "boost-support/error_domain/boost_support_error_domain.h"
#include "boost-support/socket/io_context.h"
#include "boost-support/socket/tls/tls_context.h"
#include "boost-support/socket/tls/tls_socket.h"

namespace boost_support {
namespace client {
namespace tls {

/**
 * @brief    Class to provide implementation of tls client
 */
template<typename TlsVersion>
class TlsClient<TlsVersion>::TlsClientImpl final {
 private:
  /**
   * @brief     Type alias for secured socket with Tls version 1.3
   */
  using TlsSocket = socket::tls::TlsSocket;

  /**
   * @brief     Type alias for secured tcp connection with tls version 1.3
   */
  using TcpConnectionSecured = connection::tcp::TcpConnection<connection::tcp::ConnectionType::kClient, TlsSocket>;

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

  /**
   * @brief  Type alias for tls context
   */
  using TlsContext = boost_support::socket::tls::TlsContext;

 public:
  /**
   * @brief         Constructs an instance of TcpClient
   * @param[in]     local_ip_address
   *                The local ip address of client
   * @param[in]     local_port_num
   *                The local port number of client
   */
  TlsClientImpl(std::string_view local_ip_address, std::uint16_t local_port_num, std::string_view ca_certification_path,
                TlsVersion tls_version) noexcept
      : io_context_{},
        tls_context_{tls_version, ca_certification_path},
        connection_state_{State::kDisconnected},
        tcp_connection_{TlsSocket{local_ip_address, local_port_num, tls_context_, io_context_}} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  TlsClientImpl(const TlsClientImpl &other) noexcept = delete;
  TlsClientImpl &operator=(const TlsClientImpl &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  TlsClientImpl(TlsClientImpl &&other) noexcept = delete;
  TlsClientImpl &operator=(TlsClientImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of TcpClientImpl
   */
  ~TlsClientImpl() noexcept = default;

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept { tcp_connection_.Initialize(); }

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept { tcp_connection_.DeInitialize(); }

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) noexcept { tcp_connection_.SetReadHandler(std::move(read_handler)); }

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
      if (tcp_connection_.ConnectToHost(host_ip_address, host_port_num)) {
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
      tcp_connection_.DisconnectFromHost();
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
      if (tcp_connection_.Transmit(std::move(tcp_message))) { result.EmplaceValue(); }
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
   * @brief  Stores the tls context
   */
  TlsContext tls_context_;

  /**
   * @brief  Store the state of tcp connection
   */
  std::atomic<State> connection_state_;

  /**
   * @brief      Store the tcp connection
   */
  TcpConnectionSecured tcp_connection_;
};

template<typename TlsVersion>
TlsClient<TlsVersion>::TlsClient(std::string_view local_ip_address, std::uint16_t local_port_num,
                                 std::string_view ca_certification_path, TlsVersion tls_version) noexcept
    : tls_client_impl_{std::make_unique<TlsClientImpl>(local_ip_address, local_port_num, ca_certification_path,
                                                       std::move(tls_version))} {}

template<typename TlsVersion>
TlsClient<TlsVersion>::TlsClient(TlsClient &&other) noexcept = default;

template<typename TlsVersion>
TlsClient<TlsVersion> &TlsClient<TlsVersion>::operator=(TlsClient &&other) noexcept = default;

template<typename TlsVersion>
TlsClient<TlsVersion>::~TlsClient() noexcept = default;

template<typename TlsVersion>
void TlsClient<TlsVersion>::Initialize() noexcept {
  tls_client_impl_->Initialize();
}

template<typename TlsVersion>
void TlsClient<TlsVersion>::DeInitialize() noexcept {
  tls_client_impl_->DeInitialize();
}

template<typename TlsVersion>
void TlsClient<TlsVersion>::SetReadHandler(TlsClient::HandlerRead read_handler) noexcept {
  tls_client_impl_->SetReadHandler(std::move(read_handler));
}

template<typename TlsVersion>
core_type::Result<void> TlsClient<TlsVersion>::ConnectToHost(std::string_view host_ip_address,
                                                             std::uint16_t host_port_num) {
  return tls_client_impl_->ConnectToHost(host_ip_address, host_port_num);
}

template<typename TlsVersion>
core_type::Result<void> TlsClient<TlsVersion>::DisconnectFromHost() {
  return tls_client_impl_->DisconnectFromHost();
}

template<typename TlsVersion>
auto TlsClient<TlsVersion>::IsConnectedToHost() const noexcept -> bool {
  return tls_client_impl_->IsConnectedToHost();
}

template<typename TlsVersion>
core_type::Result<void> TlsClient<TlsVersion>::Transmit(MessageConstPtr tcp_message) {
  return tls_client_impl_->Transmit(std::move(tcp_message));
}

template class TlsClient<TlsVersion13>;
template class TlsClient<TlsVersion12>;
}  // namespace tls
}  // namespace client
}  // namespace boost_support
