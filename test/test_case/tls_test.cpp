/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

#include "socket/tcp/tls_client.h"
#include "socket/tcp/tls_server.h"

namespace doip_client {
namespace {

// TLS client Tcp Ip Address
constexpr std::string_view TlsClientIpAddress{"172.16.25.127"};

// TLS Server Tcp Ip Address
constexpr std::string_view TlsServerIpAddress{"172.16.25.128"};

// TLS port number
std::uint16_t TlsPort{3496u};

constexpr std::string_view RootCACertificatePath{"../../../tools/openssl/rootCA.pem"};

class TlsClient final {
 public:
  TlsClient(std::string_view local_ip_address, std::uint16_t local_port_num)
      : tls_client_socket_{
            local_ip_address, local_port_num,
            [this](boost_support::socket::tcp::TcpMessagePtr message) { OnMessageReceived(std::move(message)); },
            RootCACertificatePath} {
    // Open the socket
    tls_client_socket_.Open();
  }

  bool Connect(std::string_view host_ip_address, std::uint16_t host_port_num) {
    return tls_client_socket_.ConnectToHost(host_ip_address, host_port_num).HasValue();
  }

  bool Disconnect() { return tls_client_socket_.DisconnectFromHost().HasValue(); }

  bool Transmit(boost_support::socket::tcp::TcpMessageConstPtr tx_message) {
    return tls_client_socket_.Transmit(std::move(tx_message)).HasValue();
  }

  ~TlsClient() { tls_client_socket_.Destroy(); }

 private:
  /**
   * @brief       Mock method invoked on reception of message
   */
  MOCK_METHOD(void, OnMessageReceived, (boost_support::socket::tcp::TcpMessagePtr), ());

  /**
   * @brief       TLS client socket
   */
  boost_support::socket::tcp::TlsClientSocket tls_client_socket_;
};

class TlsServer final {
 public:
  TlsServer(std::string_view local_ip_address, std::uint16_t local_port_num)
      : tls_server_socket_{local_ip_address, local_port_num},
        tcp_server_connection_{},
        exit_request_{false},
        running_{false},
        cond_var_{},
        mutex_{},
        thread_{} {
    // Start thread to receive messages
    thread_ = std::thread([this]() {
      std::unique_lock<std::mutex> lck(mutex_);
      while (!exit_request_) {
        if (!running_) {
          cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
          CreateTcpServerConnection();
        }
        if (!exit_request_.load()) {
          if (running_) {
            lck.unlock();
            // start receiving messages on accepted message
            if (tcp_server_connection_->ReceivedMessage()) {
              tcp_server_connection_->Shutdown();
              // connection is closed
              running_ = false;
              exit_request_ = true;
            }
            lck.lock();
          }
        }
      }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds{200});
  }

  ~TlsServer() {
    exit_request_ = true;
    running_ = false;
    cond_var_.notify_all();
    thread_.join();
  }

  void StartAcceptingNewConnection() {
    // Start accepting new connection
    {  // start reading
      std::lock_guard<std::mutex> lock{mutex_};
      running_ = true;
    }
    cond_var_.notify_all();
  }

 private:
  /**
   * @brief       Mock method invoked on reception of message
   */
  MOCK_METHOD(void, OnMessageReceived, (boost_support::socket::tcp::TcpMessagePtr), ());

  /**
   * @brief       Function to create the server connection
   */
  void CreateTcpServerConnection() {
    tcp_server_connection_ = tls_server_socket_.GetTcpServerConnection(
        [this](boost_support::socket::tcp::TcpMessagePtr message) { OnMessageReceived(std::move(message)); });
    // terminate if no connection received
    assert(tcp_server_connection_);
  }

 private:
  /**
   * @brief       TLS client socket
   */
  boost_support::socket::tcp::TlsServerSocket tls_server_socket_;

  /**
   * @brief       Tcp server connection
   */
  std::optional<boost_support::socket::tcp::TcpServerConnection> tcp_server_connection_;

  /**
   * @brief  Flag to terminate the thread
   */
  std::atomic_bool exit_request_;

  /**
   * @brief  Flag to start the thread
   */
  std::atomic_bool running_;

  /**
   * @brief  Conditional variable to block the thread
   */
  std::condition_variable cond_var_;

  /**
   * @brief  mutex to lock critical section
   */
  std::mutex mutex_;

  /**
   * @brief  The thread itself
   */
  std::thread thread_;
};

}  // namespace

/**
 * @brief       Fixture to test the TLS connection between client and server with supported cipher list
 */
class TLSFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    tls_server_.emplace(TlsServerIpAddress, TlsPort);
    tls_client_.emplace(TlsClientIpAddress, 0u);
  }

  void TearDown() override {
    tls_client_.reset();
    tls_server_.reset();
  }

 protected:
  /**
   * @brief       TLS server
   */
  std::optional<TlsServer> tls_server_{};

  /**
   * @brief       TLS client
   */
  std::optional<TlsClient> tls_client_{};
};

TEST_F(TLSFixture, SendAndReceiveMessage) {
  tls_server_->StartAcceptingNewConnection();
  tls_client_->Connect(TlsServerIpAddress, TlsPort);
  std::this_thread::sleep_for(std::chrono::seconds(5));
}

}  // namespace doip_client