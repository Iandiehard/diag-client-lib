/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <iostream>

#include "uds_message.h"

// includes from diag-client library
#include "diag-client/create_diagnostic_client.h"
#include "diag-client/diagnostic_client.h"
#include "diag-client/diagnostic_client_result.h"
#include "diag-client/diagnostic_client_uds_message_type.h"
#include "diag-client/diagnostic_client_vehicle_info_message_type.h"

/*
 * This is an example to find available ECU in a network through vehicle discovery and connecting to it.
 */
int main() {
  // Type alias for diag client conversation
  using DiagClientConversation = diag::client::conversation::DiagClientConversation;

  // Create the Diagnostic client and pass the config for creating internal properties
  std::unique_ptr const diag_client{
      diag::client::CreateDiagnosticClient("etc/diag_client_config.json")};

  // Initialize the Diagnostic Client library
  diag_client->Initialize();

  // Get conversation for tester one by providing the conversation name configured
  // in diag_client_config file passed while creating the diag client
  DiagClientConversation diag_client_conversation{
      diag_client->GetDiagnosticClientConversation("DiagTesterOne")};

  // Create a vehicle info request for finding available ECU in the whole network
  // Use preselection mode `0` for broadcasting vehicle identification request to whole network
  diag::client::vehicle_info::VehicleInfoListRequestType const vehicle_info_request{0u, ""};
  // Send Vehicle Identification request and get the response with available ECU information
  diag::client::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                       diag::client::DiagClient::VehicleInfoResponseError> const
      vehicle_response_result{diag_client->SendVehicleIdentificationRequest(vehicle_info_request)};

  // Valid vehicle discovery response must be received before connecting to ECU
  if (vehicle_response_result.HasValue()) {
    // Get the list of all vehicle available
    diag::client::vehicle_info::VehicleInfoMessage::VehicleInfoListResponseType const
        vehicle_response_collection{vehicle_response_result.Value()->GetVehicleList()};

    // Create an uds message using first vehicle available in the list of response collection
    std::string remote_ecu_ip_address{
        vehicle_response_collection[0u].ip_address};  // Remote ECU ip address
    std::uint16_t remote_ecu_server_logical_address{
        vehicle_response_collection[0u].logical_address};  // Remote ECU logical address

    // Create an uds payload 10 01 ( Default Session )
    diag::client::uds_message::UdsMessage::ByteVector uds_payload{0x10, 0x01};

    diag::client::uds_message::UdsRequestMessagePtr uds_message{
        std::make_unique<UdsMessage>(remote_ecu_ip_address, uds_payload)};

    // Connect Tester One to remote ECU
    diag::client::conversation::DiagClientConversation::ConnectResult const connect_result{
        diag_client_conversation.ConnectToDiagServer(remote_ecu_server_logical_address,
                                                     uds_message->GetHostIpAddress())};

    if (connect_result ==
        diag::client::conversation::DiagClientConversation::ConnectResult::kConnectSuccess) {
      // Use Tester One to send the diagnostic message to remote ECU
      diag::client::Result<diag::client::uds_message::UdsResponseMessagePtr,
                           DiagClientConversation::DiagError> const diag_response{
          diag_client_conversation.SendDiagnosticRequest(std::move(uds_message))};

      if (diag_response.HasValue()) {
        std::cout << "diag_client_conversation Total size: "
                  << diag_response.Value()->GetPayload().size() << std::endl;
        // Print the payload
        for (auto const byte: diag_response.Value()->GetPayload()) {
          std::cout << "diag_client_conversation byte: " << std::hex << static_cast<int>(byte)
                    << std::endl;
        }
      }
      // Disconnect Tester One from remote Ecu
      diag_client_conversation.DisconnectFromDiagServer();
    }
  }

  // shutdown the conversation
  diag_client_conversation.Shutdown();

  // important to release all the resources by calling de-initialize
  diag_client->DeInitialize();
  return 0;
}
