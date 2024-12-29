/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "doip-client/transport_protocol_handler/doip_transport_protocol_handler.h"

#include "doip-client/doip_client.h"

namespace doip_client {
namespace transport_protocol_handler {

class DoipTransportProtocolHandler::DoipTransportProtocolHandlerImpl final {
 public:
  DoipTransportProtocolHandlerImpl() noexcept : doip_client_{} {}

  /**
   * @brief       Function to create a new Tcp connection
   * @param[in]   conversation
   *              The conversation handler used by tcp connection to communicate
   * @param[in]   tcp_ip_address
   *              The local tcp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to the connection created
   */
  std::unique_ptr<uds_transport::Connection> CreateTcpConnection(
      uds_transport::ConversionHandler const &conversation, std::string_view tcp_ip_address,
      std::uint16_t port_num) {
    return doip_client_->CreateTcpConnection(conversation, tcp_ip_address, port_num);
  }

  /**
   * @brief       Function to create a new Udp connection
   * @param[in]   conversation
   *              The conversation handler used by udp connection to communicate
   * @param[in]   udp_ip_address
   *              The local udp ip address
   * @param[in]   port_num
   *              The local port number
   * @return      The unique pointer to the connection created
   */
  std::unique_ptr<uds_transport::Connection> CreateUdpConnection(
      uds_transport::ConversionHandler const &conversation, std::string_view udp_ip_address,
      std::uint16_t port_num) {
    return doip_client_->CreateUdpConnection(conversation, udp_ip_address, port_num);
  }

 private:
  std::unique_ptr<DoipClient> doip_client_;
};

DoipTransportProtocolHandler::DoipTransportProtocolHandler(
    UdsTransportProtocolHandlerId const handler_id,
    uds_transport::UdsTransportProtocolMgr const &transport_protocol_mgr)
    : uds_transport::UdsTransportProtocolHandler(handler_id, transport_protocol_mgr),
      doip_transport_protocol_handler_impl_{std::make_unique<DoipTransportProtocolHandlerImpl>()} {}

DoipTransportProtocolHandler::~DoipTransportProtocolHandler() = default;

DoipTransportProtocolHandler::InitializationResult DoipTransportProtocolHandler::Initialize() {
  return InitializationResult::kInitializeOk;
}

void DoipTransportProtocolHandler::Start() {}

void DoipTransportProtocolHandler::Stop() {}

std::unique_ptr<uds_transport::Connection> DoipTransportProtocolHandler::CreateTcpConnection(
    uds_transport::ConversionHandler &conversation, std::string_view tcp_ip_address,
    std::uint16_t port_num) {
  return doip_transport_protocol_handler_impl_->CreateTcpConnection(conversation, tcp_ip_address,
                                                                    port_num);
}

std::unique_ptr<uds_transport::Connection> DoipTransportProtocolHandler::CreateUdpConnection(
    uds_transport::ConversionHandler &conversation, std::string_view udp_ip_address,
    std::uint16_t port_num) {
  return doip_transport_protocol_handler_impl_->CreateUdpConnection(conversation, udp_ip_address,
                                                                    port_num);
}
}  // namespace transport_protocol_handler
}  // namespace doip_client
