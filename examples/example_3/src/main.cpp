/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <iostream>

#include "uds_message.h"

// includes from diag-client library
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_uds_message_type.h"

/*
 * Main Entry point of diag client example 
 * Example to connect to multiple ECU by creating multiple diagnostic tester instance.
 */
int main() {
  // Create the Diagnostic client and pass the config for creating internal properties
  std::unique_ptr<diag::client::DiagClient> diag_client{
      diag::client::CreateDiagnosticClient("etc/diag_client_config.json")};

  // Initialize the Diagnostic Client library
  diag_client->Initialize();

  // Get conversation for tester one by providing the conversation name configured
  // in diag_client_config file passed while creating the diag client
  diag::client::conversation::DiagClientConversation &diag_client_conversation {
      diag_client->GetDiagnosticClientConversation("DiagTesterOne")};

  // Start the conversation for tester one
  diag_client_conversation.Startup();

  // Create an uds payload 10 01 ( Default Session )
  diag::client::uds_message::UdsMessage::ByteVector payload{0x10, 0x01};

  {
    // Create an uds message with payload for ECU1 with remote ip address 192.168.1.200
    diag::client::uds_message::UdsRequestMessagePtr uds_message {
        std::make_unique<UdsMessage>("192.168.1.200", payload)};

    // Connect Tester One to ECU1 with target address "0x0001" and remote ip address "192.168.1.200"
    diag_client_conversation.ConnectToDiagServer(0x0001, uds_message->GetHostIpAddress());

    // Use Tester One to send the diagnostic message to ECU1
    std::pair<diag::client::conversation::DiagClientConversation::DiagResult,
              diag::client::uds_message::UdsResponseMessagePtr>
        ret_val{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

    if (ret_val.first == diag::client::conversation::DiagClientConversation::DiagResult::kDiagSuccess) {
      std::cout << "diag_client_conversation Total size: " << ret_val.second->GetPayload().size() << std::endl;
      // Print the payload
      for (auto const byte: ret_val.second->GetPayload()) {
        std::cout << "diag_client_conversation byte: " << std::hex << static_cast<int>(byte) << std::endl;
      }
    }

    // Disconnect Tester One from ECU1 with remote ip address "192.168.1.200"
    diag_client_conversation.DisconnectFromDiagServer();
  }

  // shutdown all the conversation
  diag_client_conversation.Shutdown();

  // important to release all the resources by calling de-initialize
  diag_client->DeInitialize();
  return (0);
}
