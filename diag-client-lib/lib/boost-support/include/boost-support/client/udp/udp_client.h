/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_UDP_UDP_CLIENT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_UDP_UDP_CLIENT_H_

#include <functional>
#include <string_view>

#include "boost-support/message/udp/udp_message.h"
#include "core/include/result.h"

namespace boost_support {
namespace client {
namespace udp {

/**
* @brief    Client that manages udp connection
*/
class UdpClient final {
 public:
  /**
  * @brief  Type alias for Tcp message
  */
  using Message = boost_support::message::udp::UdpMessage;

  /**
  * @brief  Type alias for Tcp message pointer
  */
  using MessagePtr = boost_support::message::udp::UdpMessagePtr;

  /**
  * @brief  Type alias for Tcp message const pointer
  */
  using MessageConstPtr = boost_support::message::udp::UdpMessageConstPtr;

  /**
  * @brief         Tcp function template used for reception
  */
  using HandlerRead = std::function<void(MessagePtr)>;

 public:
  /**
  * @brief         Constructs an instance of UdpClient
  * @param[in]     local_ip_address
  *                The local ip address
  * @param[in]     local_port_num
  *                The local port number
  */
  UdpClient(std::string_view local_ip_address, std::uint16_t local_port_num) noexcept;

  /**
  * @brief         Deleted copy assignment and copy constructor
  */
  UdpClient(const UdpClient &other) noexcept = delete;
  UdpClient &operator=(const UdpClient &other) noexcept = delete;

  /**
  * @brief         Move assignment and move constructor
  */
  UdpClient(UdpClient &&other) noexcept;
  UdpClient &operator=(UdpClient &&other) noexcept;

  /**
  * @brief         Destruct an instance of UdpClient
  */
  ~UdpClient() noexcept;

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept;

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept;

  /**
  * @brief         Function to set the read handler that is invoked when message is received
  * @details       The ownership of provided read handler is moved
  * @param[in]     read_handler
  *                The handler to be set
  */
  void SetReadHandler(HandlerRead read_handler) noexcept;

  /**
  * @brief         Function to transmit the provided tcp message
  * @param[in]     udp_message
  *                The udp message
  * @return        Empty void on success, otherwise error is returned
  */
  core_type::Result<void> Transmit(MessageConstPtr udp_message);

 private:
  /**
  * @brief    Forward declaration of udp client implementation
  */
  class UdpClientImpl;

  /**
  * @brief    Unique pointer to udp client implementation
  */
  std::unique_ptr<UdpClientImpl> udp_client_impl_;
};

}  // namespace udp
}  // namespace client
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_CLIENT_UDP_UDP_CLIENT_H_
