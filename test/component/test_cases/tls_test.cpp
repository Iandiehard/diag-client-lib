/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>
#include <optional>
#include <thread>

#include "boost-support/client/tls/tls_cipher_list.h"
#include "boost-support/client/tls/tls_client.h"
#include "boost-support/client/tls/tls_version.h"
#include "boost-support/server/tls/tls_acceptor.h"
#include "boost-support/server/tls/tls_cipher_list.h"
#include "boost-support/server/tls/tls_server.h"
#include "boost-support/server/tls/tls_version.h"
#include "component_test.h"

namespace test {
namespace component {
namespace test_cases {

// Tls Server name
constexpr std::string_view kTlsServerName{"TlsServer"};
// Tls Server Tcp Ip Address
constexpr std::string_view kTlsServerIpAddress{"172.16.25.128"};
// Tls Server port number
constexpr std::uint16_t kTlsServerTcpPortNum{3496U};
// Tls client name
constexpr std::string_view kTlsClientName{"TlsClient"};
// Tls client Tcp Ip Address
constexpr std::string_view kTlsClientIpAddress{"172.16.25.127"};
// Tls client port number
constexpr std::uint16_t kTlsClientTcpPortNum{0U};
// Certificate path
constexpr std::string_view kServerCertificatePath{"./cert/DiagClientLibServer.pem"};
// Private key path
constexpr std::string_view kServerPrivateKeyPath{"./cert/DiagClientLibServer.key"};
// CA certificate path
constexpr std::string_view kCACertificatePath{"./cert/DiagClientLibRootCA.pem"};

/*!
 * @brief       Test fixture to test tls 1.2
 */
class Tls12Fixture : public component::ComponentTest {
 public:
  // Type Alias of acceptor
  using TlsAcceptor = boost_support::server::tls::TlsAcceptor12;
  // Type Alias of server
  using TlsServer = boost_support::server::tls::TlsServer;
  // Type Alias of client
  using TlsClient = boost_support::client::tls::TlsClient12;
  // Type Alias of tls server cipher suites version 1.2
  using TlsServerCipherSuite = boost_support::server::tls::Tls12CipherSuites;
  // Type Alias of tls server version
  using TlsServerVersion = boost_support::server::tls::TlsVersion12;
  // Type Alias of tls client cipher suites version 1.2
  using TlsClientCipherSuite = boost_support::client::tls::Tls12CipherSuites;
  // Type Alias of tls client version
  using TlsClientVersion = boost_support::client::tls::TlsVersion12;

 protected:
  Tls12Fixture()
      : tls_acceptor_{kTlsServerName,
                      kTlsServerIpAddress,
                      kTlsServerTcpPortNum,
                      1u,
                      TlsServerVersion{
                          {TlsServerCipherSuite::TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
                           TlsServerCipherSuite ::TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256}},
                      kServerCertificatePath,
                      kServerPrivateKeyPath},
        tls_server_{},
        tls_client_{
            kTlsClientName, kTlsClientIpAddress, kTlsClientTcpPortNum, kCACertificatePath,
            TlsClientVersion{{TlsClientCipherSuite::TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
                              TlsClientCipherSuite::TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256}}} {}

  void SetUp() override { tls_client_.Initialize(); }

  void TearDown() override {
    if (tls_server_.has_value()) { tls_server_->DeInitialize(); }
    tls_client_.DeInitialize();
  }

  template<typename Functor>
  auto CreateServerWithExpectation(Functor expectation_functor) noexcept -> std::future<bool> {
    return std::async(std::launch::async,
                      [this, expectation_functor = std::move(expectation_functor)]() {
                        std::optional<TlsServer> server{
                            tls_acceptor_.GetTlsServer()};  // blocks until client is connected
                        if (server.has_value()) {
                          tls_server_.emplace(std::move(server).value());
                          tls_server_->Initialize();
                          // Set Expectation
                          expectation_functor();
                        }
                        return tls_server_.has_value();
                      });
  }

 protected:
  // Tls acceptor
  TlsAcceptor tls_acceptor_;
  // Tls Server
  std::optional<TlsServer> tls_server_;
  // Tls client with Tls version 1.2
  TlsClient tls_client_;
};

/*!
 * @brief       Test fixture to test tls 1.3
 */
class Tls13Fixture : public component::ComponentTest {
 public:
  // Type Alias of acceptor
  using TlsAcceptor = boost_support::server::tls::TlsAcceptor13;
  // Type Alias of server
  using TlsServer = boost_support::server::tls::TlsServer;
  // Type Alias of client
  using TlsClient = boost_support::client::tls::TlsClient13;
  // Type Alias of tls server cipher suites version 1.2
  using TlsServerCipherSuite = boost_support::server::tls::Tls13CipherSuites;
  // Type Alias of tls server version
  using TlsServerVersion = boost_support::server::tls::TlsVersion13;
  // Type Alias of tls client cipher suites version 1.2
  using TlsClientCipherSuite = boost_support::client::tls::Tls13CipherSuites;
  // Type Alias of tls client version
  using TlsClientVersion = boost_support::client::tls::TlsVersion13;

 protected:
  Tls13Fixture()
      : tls_acceptor_{kTlsServerName,
                      kTlsServerIpAddress,
                      kTlsServerTcpPortNum,
                      1u,
                      TlsServerVersion{{TlsServerCipherSuite::TLS_AES_128_GCM_SHA256,
                                        TlsServerCipherSuite ::TLS_AES_256_GCM_SHA384}},
                      kServerCertificatePath,
                      kServerPrivateKeyPath},
        tls_server_{},
        tls_client_{kTlsClientName, kTlsClientIpAddress, kTlsClientTcpPortNum, kCACertificatePath,
                    TlsClientVersion{{TlsClientCipherSuite::TLS_AES_128_GCM_SHA256,
                                      TlsClientCipherSuite::TLS_AES_256_GCM_SHA384}}} {}

  void SetUp() override { tls_client_.Initialize(); }

  void TearDown() override {
    if (tls_server_.has_value()) { tls_server_->DeInitialize(); }
    tls_client_.DeInitialize();
  }

  template<typename Functor>
  auto CreateServerWithExpectation(Functor expectation_functor) noexcept -> std::future<bool> {
    return std::async(std::launch::async,
                      [this, expectation_functor = std::move(expectation_functor)]() {
                        std::optional<TlsServer> server{
                            tls_acceptor_.GetTlsServer()};  // blocks until client is connected
                        if (server.has_value()) {
                          tls_server_.emplace(std::move(server).value());
                          tls_server_->Initialize();
                          // Set Expectation
                          expectation_functor();
                        }
                        return tls_server_.has_value();
                      });
  }

 protected:
  // Tls acceptor
  TlsAcceptor tls_acceptor_;
  // Tls Server
  std::optional<TlsServer> tls_server_;
  // Tls client with Tls version 1.2
  TlsClient tls_client_;
};

/**
 * @brief  Verify that sending of data from tls client to server works.
 */
TEST_F(Tls13Fixture, DISABLED_SendDataFromClientToServer) {
  std::vector<std::uint8_t> const kTestData{1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};

  std::future<bool> is_server_created{CreateServerWithExpectation([this, &kTestData]() {
    ASSERT_TRUE(tls_server_.has_value());
    // Create expectation that Receive Handler is invoked with same data
    tls_server_->SetReadHandler([&kTestData](TlsServer::MessagePtr message) {
      EXPECT_THAT(kTestData, ::testing::ElementsAreArray(message->GetPayload()));
    });
  })};

  // Try connecting to server and verify
  EXPECT_TRUE(tls_client_.ConnectToHost(kTlsServerIpAddress, kTlsServerTcpPortNum).HasValue());
  EXPECT_TRUE(tls_client_.IsConnectedToHost());

  ASSERT_TRUE(is_server_created.get());
  // Send test data to tls server
  EXPECT_TRUE(tls_client_
                  .Transmit(std::make_unique<TlsClient::Message>(kTlsServerIpAddress,
                                                                 kTlsServerTcpPortNum, kTestData))
                  .HasValue());
  tls_client_.DisconnectFromHost();
}

TEST_F(Tls13Fixture, DISABLED_SendDataFromServerToClient) {
  std::vector<std::uint8_t> const kTestData{1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};

  std::future<bool> is_server_created{
      CreateServerWithExpectation([this]() { ASSERT_TRUE(tls_server_.has_value()); })};

  // Set expectation on receive handler of client
  tls_client_.SetReadHandler([&kTestData](TlsClient::MessagePtr message) {
    EXPECT_THAT(kTestData, ::testing::ElementsAreArray(message->GetPayload()));
  });

  // Try connecting to server and verify
  EXPECT_TRUE(tls_client_.ConnectToHost(kTlsServerIpAddress, kTlsServerTcpPortNum).HasValue());
  EXPECT_TRUE(tls_client_.IsConnectedToHost());

  ASSERT_TRUE(is_server_created.get());
  // Send test data to tls server
  EXPECT_TRUE(tls_server_
                  ->Transmit(std::make_unique<TlsServer::Message>(kTlsServerIpAddress,
                                                                  kTlsServerTcpPortNum, kTestData))
                  .HasValue());

  std::this_thread::sleep_for(std::chrono::milliseconds{2000});
  // tls_client_.DisconnectFromHost();
}

}  // namespace test_cases
}  // namespace component
}  // namespace test
