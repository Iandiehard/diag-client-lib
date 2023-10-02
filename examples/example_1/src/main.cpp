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
  // Type alias for diag client conversation
  using DiagClientConversation = diag::client::conversation::DiagClientConversation;

  // Create the Diagnostic client and pass the config for creating internal properties
  std::unique_ptr<diag::client::DiagClient> diag_client{
      diag::client::CreateDiagnosticClient("etc/diag_client_config.json")};

  // Initialize the Diagnostic Client library
  diag_client->Initialize();

  // Get conversation for tester one by providing the conversation name configured
  // in diag_client_config file passed while creating the diag client
  DiagClientConversation diag_client_conversation1{diag_client->GetDiagnosticClientConversation("DiagTesterOne")};

  // Start the conversation for tester one
  diag_client_conversation1.Startup();

  // Get conversation for tester two by providing the conversation name configured
  // in diag_client_config file passed while creating the diag client
  DiagClientConversation diag_client_conversation2{diag_client->GetDiagnosticClientConversation("DiagTesterTwo")};

  // Start the conversation for tester two
  diag_client_conversation2.Startup();

  // Create an uds payload 10 01 ( Default Session )
  diag::client::uds_message::UdsMessage::ByteVector payload_1{0x10, 0x01};

  // Create an uds payload 3E 00( Tester Present )
  diag::client::uds_message::UdsMessage::ByteVector payload_2{0x3E, 0x00};

  {
    // Create an uds message with payload for ECU1 with remote ip address 172.16.25.128
    diag::client::uds_message::UdsRequestMessagePtr uds_message_1{
        std::make_unique<UdsMessage>("172.16.25.128", payload_1)};

    // Create an uds message with payload for ECU2 with remote ip address 172.16.25.129
    diag::client::uds_message::UdsRequestMessagePtr uds_message_2{
        std::make_unique<UdsMessage>("172.16.25.129", payload_2)};

    // Connect Tester One to ECU1 with target address "0x1234" and remote ip address "172.16.25.128"
    diag_client_conversation1.ConnectToDiagServer(0x1234, uds_message_1->GetHostIpAddress());

    // Connect Tester Two to ECU2 with target address "0x1235" and remote ip address "172.16.25.129"
    diag_client_conversation2.ConnectToDiagServer(0x1235, uds_message_1->GetHostIpAddress());

    // Use Tester One to send the diagnostic message to ECU1
    diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError> const
        ret_val_1{diag_client_conversation1.SendDiagnosticRequest(std::move(uds_message_1))};

    if (ret_val_1.HasValue()) {
      // Use Value() to get the pointer to UdsMessage and then use different method to get payload or host ip address etc.
      std::cout << "diag_client_conversation1 Total size: " << ret_val_1.Value()->GetPayload().size() << std::endl;
      // Print the payload
      for (auto const byte: ret_val_1.Value()->GetPayload()) {
        std::cout << "diag_client_conversation1 byte: " << std::hex << static_cast<int>(byte) << std::endl;
      }
    } else {
      // Use Error() to get the error type that occured when sending the request
      std::cout << "diag_client_conversation1 error code: " << static_cast<std::uint8_t>(ret_val_1.Error())
                << std::endl;
    }

    // Use Tester Two to send the diagnostic message to ECU2
    diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr, DiagClientConversation::DiagError> const
        ret_val_2{diag_client_conversation2.SendDiagnosticRequest(std::move(uds_message_2))};

    if (ret_val_2.HasValue()) {
      std::cout << "diag_client_conversation2 Total size: " << ret_val_2.Value()->GetPayload().size() << std::endl;
      // Print the payload
      for (auto const byte: ret_val_2.Value()->GetPayload()) {
        std::cout << "diag_client_conversation2 byte: " << std::hex << static_cast<int>(byte) << std::endl;
      }
    }

    // Disconnect Tester One from ECU1 with remote ip address "172.16.25.128"
    diag_client_conversation1.DisconnectFromDiagServer();
    // Disconnect Tester Two from ECU2 with remote ip address "172.16.25.129"
    diag_client_conversation2.DisconnectFromDiagServer();
  }

  // shutdown all the conversation
  diag_client_conversation1.Shutdown();
  diag_client_conversation2.Shutdown();

  // important to release all the resources by calling de-initialize
  diag_client->DeInitialize();
  return (0);
}
