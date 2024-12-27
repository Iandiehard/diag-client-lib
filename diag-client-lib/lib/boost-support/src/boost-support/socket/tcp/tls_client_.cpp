/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/socket/tcp/tls_client_.h"

#include <utility>

#include "boost-support/common/logger.h"

namespace boost_support {
namespace socket {
namespace tcp {
namespace {

void print_cn_name(const char *label, X509_NAME *const name) {
  int idx = -1, success = 0;
  unsigned char *utf8 = NULL;

  do {
    if (!name) break; /* failed */

    idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
    if (!(idx > -1)) break; /* failed */

    X509_NAME_ENTRY *entry = X509_NAME_get_entry(name, idx);
    if (!entry) break; /* failed */

    ASN1_STRING *data = X509_NAME_ENTRY_get_data(entry);
    if (!data) break; /* failed */

    int length = ASN1_STRING_to_UTF8(&utf8, data);
    if (!utf8 || !(length > 0)) break; /* failed */

    fprintf(stdout, "  %s: %s\n", label, utf8);
    success = 1;

  } while (0);

  if (utf8) OPENSSL_free(utf8);

  if (!success) fprintf(stdout, "  %s: <not available>\n", label);
}

}  // namespace

TlsClientSocket::TlsClientSocket(std::string_view local_ip_address, std::uint16_t local_port_num,
                                 TcpHandlerRead tcp_handler_read,
                                 std::string_view ca_certification_path)
    : local_ip_address_{local_ip_address},
      local_port_num_{local_port_num},
      io_context_{},
      io_ssl_context_{boost::asio::ssl::context::tlsv13_client},
      tls_socket_{io_context_, io_ssl_context_},
      exit_request_{false},
      running_{false},
      cond_var_{},
      mutex_{},
      thread_{},
      tcp_handler_read_{std::move(tcp_handler_read)} {
  // Set verification mode
  tls_socket_.set_verify_mode(boost::asio::ssl::verify_peer);
  // Set the verification callback
  tls_socket_.set_verify_callback(
      [](bool pre_verified, boost::asio::ssl::verify_context &ctx) noexcept -> bool {
        X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

        fprintf(stdout, "verify_callback (depth=%d)(preverify=%d)\n", depth, pre_verified);

        X509_NAME *iname = cert ? X509_get_issuer_name(cert) : nullptr;
        X509_NAME *sname = cert ? X509_get_subject_name(cert) : nullptr;

        /* Issuer is the authority we trust that warrants nothing useful */
        print_cn_name("Issuer (cn)", iname);
        /* Subject is who the certificate is issued to by the authority  */
        print_cn_name("Subject (cn)", sname);
        return true;
      });
  // Load the root CA certificates
  io_ssl_context_.load_verify_file(std::string{ca_certification_path});

  SSL_set_ciphersuites(tls_socket_.native_handle(),
                       "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_"
                       "CHACHA20_POLY1305_SHA256");

  // Start thread to receive messages
  thread_ = std::thread([this]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_) {
      if (!running_) {
        cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
      }
      if (!exit_request_.load()) {
        if (running_) {
          lck.unlock();
          HandleMessage();
          lck.lock();
        }
      }
    }
  });
}

TlsClientSocket::~TlsClientSocket() {
  {
    std::unique_lock<std::mutex> lck(mutex_);
    exit_request_ = true;
    running_ = false;
  }
  cond_var_.notify_all();
  thread_.join();
}

core_type::Result<void, TlsClientSocket::TlsErrorCode> TlsClientSocket::Open() {
  core_type::Result<void, TlsErrorCode> result{TlsErrorCode::kGenericError};
  TcpErrorCodeType ec{};

  // Open the socket
  GetNativeTcpSocket().open(Tcp::v4(), ec);
  if (ec.value() == boost::system::errc::success) {
    // Re-use address
    GetNativeTcpSocket().set_option(boost::asio::socket_base::reuse_address{true});
    // Set socket to non blocking
    GetNativeTcpSocket().non_blocking(false);
    // Bind to local ip address and random port
    GetNativeTcpSocket().bind(
        Tcp::endpoint(boost::asio::ip::make_address(local_ip_address_), local_port_num_), ec);

    if (ec.value() == boost::system::errc::success) {
      // Socket binding success
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__, [this](std::stringstream &msg) {
            Tcp::endpoint const endpoint_{GetNativeTcpSocket().local_endpoint()};
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
      result.EmplaceError(TlsErrorCode::kBindingFailed);
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp Socket opening failed with error: " << ec.message();
        });
    result.EmplaceError(TlsErrorCode::kOpenFailed);
  }
  return result;
}

core_type::Result<void, TlsClientSocket::TlsErrorCode> TlsClientSocket::ConnectToHost(
    std::string_view host_ip_address, std::uint16_t host_port_num) {
  core_type::Result<void, TlsErrorCode> result{TlsErrorCode::kGenericError};
  TcpErrorCodeType ec{};

  // Connect to provided Ip address
  GetNativeTcpSocket().connect(
      Tcp::endpoint(boost::asio::ip::make_address(std::string{host_ip_address}), host_port_num),
      ec);
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        FILE_NAME, __LINE__, __func__, [this](std::stringstream &msg) {
          Tcp::endpoint const endpoint_{GetNativeTcpSocket().remote_endpoint()};
          msg << "Tcp Socket connected to host "
              << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
        });
    // Perform TLS handshake
    tls_socket_.handshake(boost::asio::ssl::stream_base::client, ec);
    if (ec.value() == boost::system::errc::success) {
      {  // start reading
        std::lock_guard<std::mutex> lock{mutex_};
        running_ = true;
      }
      cond_var_.notify_all();
      printf("Connected with %s encryption\n", SSL_get_cipher(tls_socket_.native_handle()));
      result.EmplaceValue();
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
          FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
            msg << "Tls client handshake with host failed with error: " << ec.message();
          });
      result.EmplaceError(TlsErrorCode::kTlsHandshakeFailed);
    }
  } else {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Tcp Socket connect to host failed with error: " << ec.message();
        });
    result.EmplaceError(TlsErrorCode::kConnectFailed);
  }
  return result;
}

core_type::Result<void, TlsClientSocket::TlsErrorCode> TlsClientSocket::DisconnectFromHost() {
  core_type::Result<void, TlsErrorCode> result{TlsErrorCode::kGenericError};
  TcpErrorCodeType ec{};
  // Shutdown TLS connection
  tls_socket_.shutdown(ec);
  // Shutdown of TCP connection
  GetNativeTcpSocket().shutdown(TcpSocket::shutdown_both, ec);

  if (ec.value() == boost::system::errc::success) {
    {
      std::lock_guard<std::mutex> lock{mutex_};
      // stop reading
      running_ = false;
    }
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

core_type::Result<void, TlsClientSocket::TlsErrorCode> TlsClientSocket::Transmit(
    message::tcp::TcpMessageConstPtr tcp_message) {
  core_type::Result<void, TlsErrorCode> result{TlsErrorCode::kGenericError};
  TcpErrorCodeType ec{};

  boost::asio::write(
      tls_socket_,
      boost::asio::buffer(tcp_message->GetPayload().data(), tcp_message->GetPayload().size()), ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        FILE_NAME, __LINE__, __func__, [this](std::stringstream &msg) {
          Tcp::endpoint const endpoint_{GetNativeTcpSocket().remote_endpoint()};
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

core_type::Result<void, TlsClientSocket::TlsErrorCode> TlsClientSocket::Destroy() {
  core_type::Result<void, TlsErrorCode> result{TlsErrorCode::kGenericError};
  // destroy the socket
  GetNativeTcpSocket().close();
  result.EmplaceValue();
  return result;
}

void TlsClientSocket::HandleMessage() {
  TcpErrorCodeType ec{};
  // create and reserve the buffer
  message::tcp::TcpMessage::BufferType rx_buffer{};
  rx_buffer.resize(message::tcp::kDoipheadrSize);
  // start blocking read to read Header first
  boost::asio::read(tls_socket_, boost::asio::buffer(&rx_buffer[0u], message::tcp::kDoipheadrSize),
                    ec);
  // Check for error
  if (ec.value() == boost::system::errc::success) {
    // read the next bytes to read
    std::uint32_t const read_next_bytes = [&rx_buffer]() noexcept -> std::uint32_t {
      return static_cast<std::uint32_t>(
          (static_cast<std::uint32_t>(rx_buffer[4u] << 24u) & 0xFF000000) |
          (static_cast<std::uint32_t>(rx_buffer[5u] << 16u) & 0x00FF0000) |
          (static_cast<std::uint32_t>(rx_buffer[6u] << 8u) & 0x0000FF00) |
          (static_cast<std::uint32_t>(rx_buffer[7u] & 0x000000FF)));
    }();

    if (read_next_bytes != 0u) {
      // reserve the buffer
      rx_buffer.resize(message::tcp::kDoipheadrSize + std::size_t(read_next_bytes));
      boost::asio::read(
          tls_socket_,
          boost::asio::buffer(&rx_buffer[message::tcp::kDoipheadrSize], read_next_bytes), ec);

      // all message received, transfer to upper layer
      Tcp::endpoint const endpoint_{GetNativeTcpSocket().remote_endpoint()};
      message::tcp::TcpMessagePtr tcp_rx_message{std::make_unique<message::tcp::TcpMessage>(
          endpoint_.address().to_string(), endpoint_.port(), std::move(rx_buffer))};
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__, [endpoint_](std::stringstream &msg) {
            msg << "Tcp Message received from "
                << "<" << endpoint_.address().to_string() << "," << endpoint_.port() << ">";
          });
      // notify upper layer about received message
      tcp_handler_read_(std::move(tcp_rx_message));
    } else {
      common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
          FILE_NAME, __LINE__, __func__,
          [](std::stringstream &msg) { msg << "Tcp Message read ignored as header size is zero"; });
    }
  } else if (ec.value() == boost::asio::error::eof) {
    running_ = false;
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogDebug(
        FILE_NAME, __LINE__, __func__,
        [ec](std::stringstream &msg) { msg << "Remote Disconnected with: " << ec.message(); });
  } else {
    running_ = false;
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        FILE_NAME, __LINE__, __func__, [ec](std::stringstream &msg) {
          msg << "Remote Disconnected with undefined error: " << ec.message();
        });
  }
}

TlsClientSocket::TlsStream::lowest_layer_type &TlsClientSocket::GetNativeTcpSocket() {
  return tls_socket_.lowest_layer();
}

}  // namespace tcp
}  // namespace socket
}  // namespace boost_support
