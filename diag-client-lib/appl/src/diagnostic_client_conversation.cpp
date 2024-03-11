/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "include/diagnostic_client_conversation.h"

#include <memory>

#include "src/dcm/conversation/conversation.h"
#include "src/dcm/dcm_client.h"

namespace diag {
namespace client {
namespace conversation {

class DiagClientConversation::DiagClientConversationImpl final {
 public:
  /**
   * @brief         Constructor an instance of DiagClientConversationImpl
   * @param[in]     conversation_name
   *                The name of conversation configured as json parameter under "ConversationName"
   */
  explicit DiagClientConversationImpl(std::string_view conversation_name) noexcept
      : internal_conversation_{dcm::GetConversationManager().GetDiagnosticClientConversation(conversation_name)} {}

  /**
   * @brief         Destructor an instance of DiagClientConversationImpl
   */
  ~DiagClientConversationImpl() noexcept = default;

  /**
   * @brief         Function to startup the Diagnostic Client Conversation
   * @details       Must be called once and before using any other functions of DiagClientConversation
   */
  void Startup() noexcept { internal_conversation_.Startup(); }

  /**
   * @brief         Function to shutdown the Diagnostic Client Conversation
   * @details       Must be called during shutdown phase, no further processing of any function will
   *                be allowed after this call
   */
  void Shutdown() noexcept { internal_conversation_.Shutdown(); }

  /**
   * @brief         Function to connect to Diagnostic Server
   * @param[in]     target_address
   *                Logical address of the Remote server
   * @param[in]     host_ip_addr
   *                IP address of the Remote server
   * @return        ConnectResult
   *                Connection result returned
   */
  ConnectResult ConnectToDiagServer(std::uint16_t target_address, IpAddress host_ip_addr) noexcept {
    return internal_conversation_.ConnectToDiagServer(target_address, host_ip_addr);
  }

  /**
   * @brief         Function to disconnect from Diagnostic Server
   * @return        DisconnectResult
   *                Disconnection result returned
   */
  DisconnectResult DisconnectFromDiagServer() noexcept { return internal_conversation_.DisconnectFromDiagServer(); }

  /**
   * @brief         Function to send Diagnostic Request and get Diagnostic Response
   * @param[in]     message
   *                The diagnostic request message wrapped in a unique pointer
   * @return        DiagResult
   *                The result returned
   * @return        uds_message::UdsResponseMessagePtr
   *                Diagnostic Response message received, null_ptr in case of error
   */
  Result<uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError> SendDiagnosticRequest(
      uds_message::UdsRequestMessageConstPtr message) noexcept {
    return internal_conversation_.SendDiagnosticRequest(std::move(message));
  }

 private:
  /**
   * @brief         Reference to valid conversation created
   */
  Conversation& internal_conversation_;
};

DiagClientConversation::DiagClientConversation(std::string_view conversation_name) noexcept
    : diag_client_conversation_impl_{std::make_unique<DiagClientConversationImpl>(conversation_name)} {}

DiagClientConversation::~DiagClientConversation() noexcept = default;

void DiagClientConversation::Startup() noexcept { diag_client_conversation_impl_->Startup(); }

void DiagClientConversation::Shutdown() noexcept { diag_client_conversation_impl_->Shutdown(); }

DiagClientConversation::ConnectResult DiagClientConversation::ConnectToDiagServer(
    std::uint16_t target_address, DiagClientConversation::IpAddress host_ip_addr) noexcept {
  return diag_client_conversation_impl_->ConnectToDiagServer(target_address, host_ip_addr);
}

DiagClientConversation::DisconnectResult DiagClientConversation::DisconnectFromDiagServer() noexcept {
  return diag_client_conversation_impl_->DisconnectFromDiagServer();
}

Result<uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError>
DiagClientConversation::SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) noexcept {
  return diag_client_conversation_impl_->SendDiagnosticRequest(std::move(message));
}

}  // namespace conversation
}  // namespace client
}  // namespace diag
