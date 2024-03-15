/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "common/handler/doip_tcp_handler.h"

#include <gtest/gtest.h>

#include "common/message/doip_message.h"

namespace test {
namespace component {
namespace common {
namespace handler {
namespace {

constexpr std::uint8_t kDoipProtocolVersion{0x03};
constexpr std::uint16_t kDoipRoutingActivationReqType{0x0005};
constexpr std::uint16_t kDoipRoutingActivationResType{0x0006};
constexpr std::uint32_t kDoipRoutingActivationResMinLen{9u};  // without OEM specific use byte
/**
 * @brief  Diagnostic message type
 */
constexpr std::uint16_t kDoipDiagMessage{0x8001};
constexpr std::uint16_t kDoipDiagMessagePosAck{0x8002};
constexpr std::uint16_t kDoipDiagMessageNegAck{0x8003};
/**
 * @brief  Diagnostic Message request/response lengths
 */
constexpr std::uint8_t kDoipDiagMessageReqResMinLen = 4U;  // considering SA and TA
constexpr std::uint8_t kDoipDiagMessageAckResMinLen = 5U;  // considering SA, TA, Ack code

auto CreateDoipGenericHeader(std::uint16_t payload_type, std::uint32_t payload_len) noexcept
    -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> output_buffer{};
  output_buffer.emplace_back(kDoipProtocolVersion);
  output_buffer.emplace_back(~(static_cast<std::uint8_t>(kDoipProtocolVersion)));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_type & 0xFF00) >> 8));
  output_buffer.emplace_back(static_cast<std::uint8_t>(payload_type & 0x00FF));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0xFF000000) >> 24));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x00FF0000) >> 16));
  output_buffer.emplace_back(static_cast<std::uint8_t>((payload_len & 0x0000FF00) >> 8));
  output_buffer.emplace_back(static_cast<std::uint8_t>(payload_len & 0x000000FF));
  return output_buffer;
}

auto ConvertToAddr(core_type::Span<std::uint8_t const> payload) noexcept -> std::uint16_t {
  return static_cast<std::uint16_t>(((payload[0u] << 8) & 0xFF00) | payload[1u]);
}

auto GetVmSpecific(core_type::Span<std::uint8_t const> payload) noexcept -> std::uint32_t {
  return ((static_cast<std::uint32_t>(payload[7u] << 24) & 0xFF000000) |
          (static_cast<std::uint32_t>(payload[8u] << 16) & 0x00FF0000) |
          (static_cast<std::uint32_t>(payload[9u] << 8) & 0x0000FF00) |
          (static_cast<std::uint32_t>(payload[10u] & 0x000000FF)));
}

}  // namespace

DoipTcpHandler::DoipTcpHandler(boost_support::server::tcp::TcpServer tcp_server) : tcp_server_{std::move(tcp_server)} {}

void DoipTcpHandler::Initialize() {
  tcp_server_.SetReadHandler(
      [this](TcpServer::MessagePtr tcp_message) { ProcessReceivedTcpMessage(std::move(tcp_message)); });
  tcp_server_.Initialize();
}

void DoipTcpHandler::DeInitialize() { tcp_server_.DeInitialize(); }

void DoipTcpHandler::SendTcpMessage(boost_support::server::tcp::TcpServer::MessageConstPtr tcp_message) noexcept {
  EXPECT_TRUE(tcp_server_.Transmit(std::move(tcp_message)).HasValue());
}

void DoipTcpHandler::ProcessReceivedTcpMessage(boost_support::server::tcp::TcpServer::MessagePtr tcp_message) {
  message::DoipMessage doip_message{tcp_message->GetHostIpAddress(), tcp_message->GetHostPortNumber(),
                                    tcp_message->GetPayload()};
  switch (doip_message.GetPayloadType()) {
    case kDoipRoutingActivationReqType: {
      std::uint16_t const client_source_address{ConvertToAddr(doip_message.GetPayload())};
      std::uint8_t const activation_type{doip_message.GetPayload()[2u]};
      std::optional<std::uint8_t> vm_specific{};
      // Reserved bytes must be zero
      EXPECT_EQ(doip_message.GetPayload()[3u], 0u);
      EXPECT_EQ(doip_message.GetPayload()[4u], 0u);
      EXPECT_EQ(doip_message.GetPayload()[5u], 0u);
      EXPECT_EQ(doip_message.GetPayload()[6u], 0u);

      if (doip_message.GetPayload().size() > 7u) {
        // Extract vm specific data
        vm_specific.emplace(GetVmSpecific(doip_message.GetPayload()));
      }
      ProcessRoutingActivationRequestMessage(client_source_address, activation_type, vm_specific);
    } break;

    case kDoipDiagMessage: {
      constexpr std::uint8_t kSourceAddressSize{4u};
      std::uint16_t const client_source_address{ConvertToAddr(doip_message.GetPayload())};
      std::uint16_t const server_target_address{ConvertToAddr({&doip_message.GetPayload()[2u], 2u})};
      core_type::Span<std::uint8_t const> diag_request{core_type::Span<std::uint8_t const>{
          &doip_message.GetPayload()[kSourceAddressSize], doip_message.GetPayload().size() - kSourceAddressSize}};
      ProcessDiagnosticRequestMessage(client_source_address, server_target_address, diag_request);
    } break;
  }
}

auto ComposeRoutingActivationResponse(std::uint16_t client_logical_address, std::uint16_t server_logical_address,
                                      std::uint8_t activation_response_code,
                                      std::optional<std::uint32_t> vm_specific) noexcept
    -> DoipTcpHandler::TcpServer::MessagePtr {
  // Create header
  DoipTcpHandler::TcpServer::Message::BufferType response_buffer{
      CreateDoipGenericHeader(kDoipRoutingActivationResType, kDoipRoutingActivationResMinLen)};
  // Add client LA
  response_buffer.emplace_back(client_logical_address >> 8U);
  response_buffer.emplace_back(client_logical_address & 0xFFU);
  // Add server LA
  response_buffer.emplace_back(server_logical_address >> 8U);
  response_buffer.emplace_back(server_logical_address & 0xFFU);
  // Add activation response code
  response_buffer.emplace_back(activation_response_code);
  // Add reserved byte
  response_buffer.emplace_back(0x00);
  response_buffer.emplace_back(0x00);
  response_buffer.emplace_back(0x00);
  response_buffer.emplace_back(0x00);

  DoipTcpHandler::TcpServer::MessagePtr response{
      std::make_unique<DoipTcpHandler::TcpServer::Message>("", 0u, std::move(response_buffer))};
  return response;
}

auto ComposeDiagnosticPositiveAcknowledgementMessage(std::uint16_t source_address, std::uint16_t target_address,
                                                     std::uint8_t ack_code) noexcept
    -> DoipTcpHandler::TcpServer::MessagePtr {
  // Create header
  DoipTcpHandler::TcpServer::Message::BufferType response_buffer{
      CreateDoipGenericHeader(kDoipDiagMessagePosAck, kDoipDiagMessageAckResMinLen)};
  // Add SA
  response_buffer.emplace_back(source_address >> 8U);
  response_buffer.emplace_back(source_address & 0xFFU);
  // Add TA
  response_buffer.emplace_back(target_address >> 8U);
  response_buffer.emplace_back(target_address & 0xFFU);
  // Add ack
  response_buffer.emplace_back(ack_code);

  DoipTcpHandler::TcpServer::MessagePtr response{
      std::make_unique<DoipTcpHandler::TcpServer::Message>("", 0u, std::move(response_buffer))};
  return response;
}

auto ComposeDiagnosticNegativeAcknowledgementMessage(std::uint16_t source_address, std::uint16_t target_address,
                                                     std::uint8_t ack_code) noexcept
    -> DoipTcpHandler::TcpServer::MessagePtr {
  // Create header
  DoipTcpHandler::TcpServer::Message::BufferType response_buffer{
      CreateDoipGenericHeader(kDoipDiagMessageNegAck, kDoipDiagMessageAckResMinLen)};
  // Add SA
  response_buffer.emplace_back(source_address >> 8U);
  response_buffer.emplace_back(source_address & 0xFFU);
  // Add TA
  response_buffer.emplace_back(target_address >> 8U);
  response_buffer.emplace_back(target_address & 0xFFU);
  // Add ack
  response_buffer.emplace_back(ack_code);

  DoipTcpHandler::TcpServer::MessagePtr response{
      std::make_unique<DoipTcpHandler::TcpServer::Message>("", 0u, std::move(response_buffer))};
  return response;
}

auto ComposeDiagnosticResponseMessage(std::uint16_t source_address, std::uint16_t target_address,
                                      core_type::Span<const std::uint8_t> diag_response) noexcept
    -> DoipTcpHandler::TcpServer::MessagePtr {
  EXPECT_TRUE(diag_response.size() > 1u);
  constexpr std::uint8_t kDoipHeaderSize{8u};
  constexpr std::uint8_t kSourceAddressSize{4u};
  // Create header
  DoipTcpHandler::TcpServer::Message::BufferType response_buffer{
      CreateDoipGenericHeader(kDoipDiagMessage, kDoipDiagMessageReqResMinLen + diag_response.size())};
  // Add SA
  response_buffer.emplace_back(source_address >> 8U);
  response_buffer.emplace_back(source_address & 0xFFU);
  // Add TA
  response_buffer.emplace_back(target_address >> 8U);
  response_buffer.emplace_back(target_address & 0xFFU);
  // Copy data bytes
  response_buffer.insert(response_buffer.begin() + kDoipHeaderSize + kSourceAddressSize, diag_response.cbegin(),
                         diag_response.cend());

  DoipTcpHandler::TcpServer::MessagePtr response{
      std::make_unique<DoipTcpHandler::TcpServer::Message>("", 0u, std::move(response_buffer))};
  return response;
}

}  // namespace handler
}  // namespace common
}  // namespace component
}  // namespace test
