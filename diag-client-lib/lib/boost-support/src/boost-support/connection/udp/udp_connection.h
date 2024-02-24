/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_UDP_UDP_CONNECTION_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_UDP_UDP_CONNECTION_H_

#include <functional>

#include "core/include/result.h"

namespace boost_support {
namespace connection {
namespace udp {

/**
 * @brief       Client connection class used to handle transmission and reception of udp message from socket
 * @tparam      Socket
 *              The type of socket to read and write from/to
 */
template<typename Socket>
class UdpConnection final {
 public:
  /**
   * @brief  Type alias for Udp message
   */
  using UdpMessage = typename Socket::UdpMessage;

  /**
   * @brief  Type alias for Udp message pointer
   */
  using UdpMessagePtr = typename Socket::UdpMessagePtr;

  /**
   * @brief  Type alias for Udp message const pointer
   */
  using UdpMessageConstPtr = typename Socket::UdpMessageConstPtr;

  /**
   * @brief         Tcp function template used for reception
   */
  using HandlerRead = std::function<void(UdpMessagePtr)>;

 public:
  /**
   * @brief         Constructs an instance of UdpConnection
   * @param[in]     socket
   *                The socket used for read and writing messages
   */
  explicit UdpConnection(Socket socket) noexcept : socket_{std::move(socket)} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  UdpConnection(const UdpConnection &other) noexcept = delete;
  UdpConnection &operator=(const UdpConnection &other) &noexcept = delete;

  /**
   * @brief  Move assignment and move constructor
   */
  UdpConnection(UdpConnection &&other) noexcept = default;
  UdpConnection &operator=(UdpConnection &&other) &noexcept = default;

  /**
   * @brief         Destruct an instance of UdpConnection
   */
  ~UdpConnection() noexcept = default;

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) { socket_.SetReadHandler(std::move(read_handler)); }

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept {
    // Open socket
    socket_.Open();
  }

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept { socket_.Close(); }

  /**
   * @brief         Function to trigger transmission
   * @param[in]     message
   *                The udp message to be transmitted
   * @return        Empty result on success otherwise error code
   */
  auto Transmit(UdpMessageConstPtr message) noexcept -> bool { return socket_.Transmit(std::move(message)).HasValue(); }

 private:
  /**
   * @brief  Store socket used for reading and writing tcp message
   */
  Socket socket_;
};

}  // namespace udp
}  // namespace connection
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SRC_BOOST_SUPPORT_CONNECTION_UDP_UDP_CONNECTION_H_
