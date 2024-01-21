/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_CLIENT_TCP_TCP_CLIENT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_CLIENT_TCP_TCP_CLIENT_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "core/include/result.h"

namespace boost_support {
namespace client {
namespace tcp {

/**
 * @brief       Client class used to handle transmission and reception of tcp message from socket
 * @tparam      SocketType
 *              The type of socket to read and write from/to
 */
template<typename SocketType>
class TcpClient final {
 public:
  /**
   * @brief  Type alias for Tcp message
   */
  using TcpMessage = typename SocketType::TcpMessage;

  /**
   * @brief  Type alias for Tcp message pointer
   */
  using TcpMessagePtr = typename SocketType::TcpMessagePtr;

  /**
   * @brief  Type alias for Tcp message const pointer
   */
  using TcpMessageConstPtr = typename SocketType::TcpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(TcpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of TcpClient
   * @param[in]     socket
   *                The socket used for read and writing messages
   * @param[in]     handler_read
   *                The handler to send received data to user
   */
  explicit TcpClient(SocketType socket) noexcept
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
  TcpClient(const TcpClient &other) noexcept = delete;
  TcpClient &operator=(const TcpClient &other) &noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  TcpClient(TcpClient &&other) noexcept = delete;
  TcpClient &operator=(TcpClient &&other) &noexcept = delete;

  /**
   * @brief         Destruct an instance of TcpClientSocket
   */
  ~TcpClient() noexcept = default;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) { handler_read_ = std::move(read_handler); }

  /**
   * @brief         Initialize the client
   * @return        Empty result on success otherwise error code
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
        if (!exit_request_.load()) {
          if (running_) {
            lck.unlock();
            running_ = ReadMessage();
            lck.lock();
          }
        }
      }
    });
  }

  /**
   * @brief         Function to connect to remote ip address and port number
   * @param[in]     host_ip_address
   *                The host ip address
   * @param[in]     host_port_num
   *                The host port number
   * @return        Empty result on success otherwise error code
   */
  auto ConnectToHost(std::string_view host_ip_address, std::uint16_t host_port_num) noexcept -> bool {
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
  auto Transmit(TcpMessageConstPtr message) noexcept -> bool { return socket_.Transmit(std::move(message)).HasValue(); }

  /**
   * @brief         De-initialize the client
   * @return        Empty result on success otherwise error code
   */
  void DeInitialize() noexcept {
    socket_.Close();
    {
      std::unique_lock<std::mutex> lck(mutex_);
      exit_request_ = true;
      running_ = false;
    }
    cond_var_.notify_all();
    thread_.join();
  }

 private:
  /**
   * @brief  Store socket used for reading and writing tcp message
   */
  SocketType socket_;

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
    core_type::Result<TcpMessagePtr, typename SocketType::SocketError> read_result{socket_.Read()};
    if (read_result.HasValue()) {
      if (handler_read_) { handler_read_(std::move(read_result).Value()); }
    }
    return read_result.HasValue();
  }
};
}  // namespace tcp
}  // namespace client
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_CLIENT_TCP_TCP_CLIENT_H_
