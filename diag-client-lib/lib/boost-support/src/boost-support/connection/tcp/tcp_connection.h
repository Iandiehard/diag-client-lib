/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_TCP_TCP_CONNECTION_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_TCP_TCP_CONNECTION_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "boost-support/error_domain/boost_support_error_domain.h"
#include "core/include/result.h"

namespace boost_support {
namespace connection {
namespace tcp {

/**
 * @brief      Type of connection Client/ Server
 */
enum class ConnectionType : std::uint8_t { kClient, kServer };

/**
 * @brief       Primary connection template to handle transmission and reception of tcp message from socket
 * @tparam      Connection
 *              The type of connection : Client/ Server
 * @tparam      Socket
 *              The type of socket to read and write from/to
 */
template<ConnectionType Connection, typename Socket>
class TcpConnection;

/**
 * @brief       Client connection class used to handle transmission and reception of tcp message from socket
 * @tparam      Socket
 *              The type of socket to read and write from/to
 */
template<typename Socket>
class TcpConnection<ConnectionType::kClient, Socket> final {
 public:
  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = typename Socket::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = typename Socket::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = typename Socket::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(TcpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TcpConnection
   * @param[in]     socket
   *                The socket used for read and writing messages
   */
  explicit TcpConnection(Socket socket) noexcept
      : socket_{std::move(socket)},
        handler_read_{},
        exit_request_{false},
        running_{false},
        cond_var_{},
        thread_{},
        mutex_{} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TcpConnection(const TcpConnection &other) noexcept = delete;
  TcpConnection &operator=(const TcpConnection &other) &noexcept = delete;

  /**
   * @brief  Move assignment and move constructor
   */
  TcpConnection(TcpConnection &&other) noexcept = default;
  TcpConnection &operator=(TcpConnection &&other) &noexcept = default;

  /**
   * @brief         Destruct an instance of TcpConnection
   */
  ~TcpConnection() noexcept = default;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) { handler_read_ = std::move(read_handler); }

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept {
    // Open socket
    socket_.Open();
    // Start thread to receive messages
    thread_ = std::thread([this]() {
      std::unique_lock<std::mutex> lck(mutex_);
      while (!exit_request_) {
        if (!running_) {
          cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
        }
        if (!exit_request_.load() && running_) {
          lck.unlock();
          running_ = ReadMessage();
          lck.lock();
        }
      }
    });
  }

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept {
    socket_.Close();
    {
      std::unique_lock<std::mutex> lck(mutex_);
      exit_request_ = true;
      running_ = false;
    }
    cond_var_.notify_all();
    if (thread_.joinable()) { thread_.join(); }
  }

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty result on success otherwise error code
   */
  auto ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) noexcept
      -> bool {
    return socket_.Connect(host_ip_address, host_port_num)
        .AndThen([this]() noexcept {
          {  // start reading
            std::lock_guard<std::mutex> lock{mutex_};
            running_ = true;
            cond_var_.notify_all();
          }
        })
        .HasValue();
  }

  /**
   * @brief         Function to Disconnect from host
   * @return        Empty result on success otherwise error code
   */
  void DisconnectFromHost() noexcept {
    {  // stop reading
      std::lock_guard<std::mutex> lock{mutex_};
      running_ = false;
    }
    socket_.Disconnect();
  }

  /**
   * @brief         Function to trigger transmission
   * @param[in]     message
   *                The tcp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void> Transmit(TcpMessageConstPtr message) noexcept {
    return socket_.Transmit(std::move(message))
        .AndThen([]() { return core_type::Result<void>::FromValue(); })
        .MapError([](typename Socket::SocketError const &) {
          return error_domain::MakeErrorCode(error_domain::BoostSupportErrorErrc::kSocketError);
        });
  }

 private:
  /**
   * @brief  Store socket used for reading and writing tcp message
   */
  Socket socket_;

  /**
   * @brief  Store the handler
   */
  HandlerRead handler_read_;

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
   * @brief  Store the thread
   */
  std::thread thread_;

  /**
   * @brief  mutex to lock critical section
   */
  std::mutex mutex_;

 private:
  /**
   * @brief         Function to send the received message to stored handler
   * @return        True if message is read successfully, otherwise False
   */
  auto ReadMessage() -> bool {
    // Try reading from socket
    return socket_.Read()
        .AndThen([this](TcpMessagePtr tcp_message) {
          if (handler_read_) { handler_read_(std::move(tcp_message)); }
          return core_type::Result<void, typename Socket::SocketError>::FromValue();
        })
        .HasValue();
  }
};

/**
 * @brief       Server connection class used to handle transmission and reception of tcp message from socket
 * @tparam      Socket
 *              The type of socket to read and write from/to
 */
template<typename Socket>
class TcpConnection<ConnectionType::kServer, Socket> final {
 public:
  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = typename Socket::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = typename Socket::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = typename Socket::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(TcpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TcpConnection
   * @param[in]     socket
   *                The socket used for read and writing messages
   */
  explicit TcpConnection(Socket socket) noexcept
      : socket_{std::move(socket)},
        handler_read_{},
        exit_request_{false},
        running_{false},
        cond_var_{},
        thread_{},
        mutex_{} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  TcpConnection(const TcpConnection &other) noexcept = delete;
  TcpConnection &operator=(const TcpConnection &other) &noexcept = delete;

  /**
   * @brief  Move assignment
   */
  TcpConnection(TcpConnection &&other) noexcept
      : socket_{std::move(other.socket_)},
        handler_read_{std::move(other.handler_read_)},
        exit_request_{other.exit_request_.load()},
        running_{other.running_.load()},
        thread_{std::move(other.thread_)} {}

  /**
   * @brief  Move constructor
   */
  TcpConnection &operator=(TcpConnection &&other) &noexcept {
    socket_ = std::move(other.socket_);
    handler_read_ = std::move(other.handler_read_);
    exit_request_.store(other.exit_request_.load());
    running_.store(other.running_.load());
    thread_ = std::move(other.thread_);
    return *this;
  }

  /**
   * @brief         Destruct an instance of TcpConnection
   */
  ~TcpConnection() noexcept = default;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) { handler_read_ = std::move(read_handler); }

  /**
   * @brief         Initialize the Server
   */
  void Initialize() noexcept {
    // Start thread to receive messages
    thread_ = std::thread([this]() {
      std::unique_lock<std::mutex> lck(mutex_);
      while (!exit_request_) {
        if (!running_) {
          cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
        }
        if (!exit_request_ && running_) {
          lck.unlock();
          running_ = ReadMessage();
          lck.lock();
        }
      }
    });
    {  // start reading
      std::lock_guard<std::mutex> lock{mutex_};
      running_ = true;
      cond_var_.notify_all();
    }
  }

  /**
   * @brief         De-initialize the server
   */
  void DeInitialize() noexcept {
    socket_.Close();
    {
      std::unique_lock<std::mutex> lck(mutex_);
      exit_request_ = true;
      running_ = false;
    }
    cond_var_.notify_all();
    if (thread_.joinable()) { thread_.join(); }
  }

  /**
   * @brief         Function to trigger transmission
   * @param[in]     message
   *                The tcp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  core_type::Result<void> Transmit(TcpMessageConstPtr message) noexcept {
    return socket_.Transmit(std::move(message))
        .AndThen([]() { return core_type::Result<void>::FromValue(); })
        .MapError([](typename Socket::SocketError const &) {
          return error_domain::MakeErrorCode(error_domain::BoostSupportErrorErrc::kSocketError);
        });
  }

 private:
  /**
   * @brief  Store socket used for reading and writing tcp message
   */
  Socket socket_;

  /**
   * @brief  Store the handler
   */
  HandlerRead handler_read_;

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
   * @brief  Store the thread
   */
  std::thread thread_;

  /**
   * @brief  mutex to lock critical section
   */
  std::mutex mutex_;

 private:
  /**
   * @brief         Function to send the received message to stored handler
   * @return        True if message is read successfully, otherwise False
   */
  auto ReadMessage() noexcept -> bool {
    // Try reading from socket
    return socket_.Read()
        .AndThen([this](TcpMessagePtr tcp_message) {
          if (handler_read_) { handler_read_(std::move(tcp_message)); }
          return core_type::Result<void, typename Socket::SocketError>::FromValue();
        })
        .HasValue();
  }
};

}  // namespace tcp
}  // namespace connection
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_TCP_TCP_CONNECTION_H_
