/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/client/udp/udp_client.h"

#include "boost-support/connection/udp/udp_connection.h"
#include "boost-support/socket/io_context.h"
#include "boost-support/socket/udp/udp_socket.h"

namespace boost_support {
namespace client {
namespace udp {

/**
 * @brief    Class to provide implementation of udp client
 */
class UdpClient::UdpClientImpl final {
 public:
  /**
   * @brief         Constructs an instance of UdpClientImpl
   * @param[in]     local_ip_address
   *                The local ip address of client
   * @param[in]     local_port_num
   *                The local port number of client
   */
  UdpClientImpl(std::string_view local_ip_address, std::uint16_t local_port_num) noexcept
      : io_context_{"UdpClient"},
        udp_connection_{
            socket::udp::UdpSocket{local_ip_address, local_port_num, io_context_.GetContext()}} {}

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  UdpClientImpl(const UdpClientImpl &other) noexcept = delete;
  UdpClientImpl &operator=(const UdpClientImpl &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  UdpClientImpl(UdpClientImpl &&other) noexcept = delete;
  UdpClientImpl &operator=(UdpClientImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of UdpClientImpl
   */
  ~UdpClientImpl() noexcept = default;

  /**
   * @brief         Initialize the client
   */
  void Initialize() noexcept {
    io_context_.Initialize();
    udp_connection_.Initialize();
  }

  /**
   * @brief         De-initialize the client
   */
  void DeInitialize() noexcept {
    udp_connection_.DeInitialize();
    io_context_.DeInitialize();
  }

  /**
   * @brief         Function to set the read handler that is invoked when message is received
   * @details       The ownership of provided read handler is moved
   * @param[in]     read_handler
   *                The handler to be set
   */
  void SetReadHandler(HandlerRead read_handler) noexcept {
    udp_connection_.SetReadHandler(std::move(read_handler));
  }

  /**
   * @brief         Function to transmit the provided udp message
   * @param[in]     udp_message
   *                The udp message
   * @return        Empty void on success, otherwise error is returned
   */
  core_type::Result<void> Transmit(MessageConstPtr udp_message) {
    return udp_connection_.Transmit(std::move(udp_message));
  }

 private:
  /**
   * @brief     Type alias for Udp connection
   */
  using UdpConnection = connection::udp::UdpConnection<socket::udp::UdpSocket>;

  /**
   * @brief  Type alias for boost context
   */
  using IoContext = boost_support::socket::IoContext;

  /**
   * @brief  Stores the io context
   */
  IoContext io_context_;

  /**
   * @brief      Store the udp connection
   */
  UdpConnection udp_connection_;
};

UdpClient::UdpClient(std::string_view local_ip_address, std::uint16_t local_port_num) noexcept
    : udp_client_impl_{std::make_unique<UdpClientImpl>(local_ip_address, local_port_num)} {}

UdpClient::UdpClient(UdpClient &&other) noexcept = default;

UdpClient &UdpClient::operator=(UdpClient &&other) noexcept = default;

UdpClient::~UdpClient() noexcept = default;

void UdpClient::Initialize() noexcept { udp_client_impl_->Initialize(); }

void UdpClient::DeInitialize() noexcept { udp_client_impl_->DeInitialize(); }

void UdpClient::SetReadHandler(UdpClient::HandlerRead read_handler) noexcept {
  udp_client_impl_->SetReadHandler(std::move(read_handler));
}

core_type::Result<void> UdpClient::Transmit(UdpClient::MessageConstPtr udp_message) {
  return udp_client_impl_->Transmit(std::move(udp_message));
}

}  // namespace udp
}  // namespace client
}  // namespace boost_support
