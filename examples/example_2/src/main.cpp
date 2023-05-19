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
#include "include/diagnostic_client_vehicle_info_message_type.h"

/*
 * Main Entry point of diag client example 
 * Example to find available ECU in a network through vehicle discovery and connecting to it.
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

  // Create a vehicle info request for finding ECU with matching VIN - ABCDEFGH123456789
  diag::client::vehicle_info::VehicleInfoListRequestType
      vehicle_info_request{1U, "ABCDEFGH123456789"};
  // Send Vehicle Identification request and get the response with available ECU information
  std::pair<diag::client::DiagClient::VehicleResponseResult,
            diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr>
      vehicle_response_result{diag_client->SendVehicleIdentificationRequest(vehicle_info_request)};


  // Valid vehicle discovery response must be received before connecting to ECU
  if(vehicle_response_result.first == diag::client::DiagClient::VehicleResponseResult::kStatusOk &&
      (vehicle_response_result.second)) {
    // Get the list of all vehicle available with matching VIN
    diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType vehicle_response_collection{
        vehicle_response_result.second->GetVehicleList()};

    // Create an uds message using first vehicle available in the list of response collection
    std::string remote_ecu_ip_address{vehicle_response_collection[0].ip_address}; // Remote ECU ip address
    std::uint16_t remote_ecu_server_logical_address{vehicle_response_collection[0].logical_address}; // Remote ECU logical address

    diag::client::uds_message::UdsRequestMessagePtr uds_message{
        std::make_unique<UdsMessage>(remote_ecu_ip_address, UdsMessage::ByteVector{0x10, 0x01})};

    // Connect Tester One to remote ECU
    diag::client::conversation::DiagClientConversation::ConnectResult connect_result{
        diag_client_conversation.ConnectToDiagServer(remote_ecu_server_logical_address,
                                                     uds_message->GetHostIpAddress())};

    if(connect_result == diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess) {
      // Use Tester One to send the diagnostic message to remote ECU
      std::pair<diag::client::conversation::DiagClientConversation::DiagResult,
                diag::client::uds_message::UdsResponseMessagePtr>
          diag_response{diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

      if (diag_response.first == diag::client::conversation::DiagClientConversation::DiagResult::kDiagSuccess) {
        std::cout << "diag_client_conversation Total size: " << diag_response.second->GetPayload().size() << std::endl;
        // Print the payload
        for (auto const byte: diag_response.second->GetPayload()) {
          std::cout << "diag_client_conversation byte: " << std::hex << static_cast<int>(byte) << std::endl;
        }
      }

      // Disconnect Tester One from ECU1 with remote ip address "172.16.25.128"
      diag_client_conversation.DisconnectFromDiagServer();
    }

  }

  // shutdown the conversation
  diag_client_conversation.Shutdown();

  // important to release all the resources by calling de-initialize
  diag_client->DeInitialize();
  return (0);
}
