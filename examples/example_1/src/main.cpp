#include "create_diagnostic_client.h"
#include "uds_message.h"
#include <chrono>
#include <thread>
#include <iostream>

/*
 * Main Entry point of diag client example 
 * This example explains how to use the diag client library efficiently
 */
int main(void) {
    // Create the Diagnostic client and pass the config as well
    std::unique_ptr<diag::client::DiagClient> diagclient =
                diag::client::CreateDiagnosticClient("etc/diag_client_config.json");
    
    // Initialize the Diagnostic Client library
    diagclient->Initialize();
    
    // Get conversation for tester one
    diag::client::conversation::DiagClientConversation& diag_client_conversation1 =
                diagclient->GetDiagnosticClientConversation("DiagTesterOne");
    
    // Start the conversation for tester one
    diag_client_conversation1.Startup();

    // Get conversation for tester two
    diag::client::conversation::DiagClientConversation& diag_client_conversation2 =
                diagclient->GetDiagnosticClientConversation("DiagTesterTwo");
    
    // Start the conversation for tester two
    diag_client_conversation2.Startup();

    // Create a uds payload 10 01 ( Default Session )
    diag::client::uds_message::ByteVector payload_1;
    payload_1.push_back(0x10);
    payload_1.push_back(0x01);

    // Create a uds payload 3E 00( Tester Present )
    diag::client::uds_message::ByteVector payload_2;
    payload_2.push_back(0x3E);
    payload_2.push_back(0x00);

    {
        // Create a uds message with payload for ECU1
        diag::client::uds_message::UdsRequestMessagePtr uds_message_1 = 
                            std::make_unique<UdsMessage>("192.168.0.2", payload_1);

        // Create a uds message with payload for ECU2
        diag::client::uds_message::UdsRequestMessagePtr uds_message_2 = 
                            std::make_unique<UdsMessage>("192.168.0.3", payload_2);
        
        // Connect Tester One to ECU1 with remote ip address 192.168.0.2
        diag_client_conversation1.ConnectToDiagServer(uds_message_1->GetHostIpAddress());
        
        // Connect Tester Two to ECU2 with remote ip address 192.168.0.3
        diag_client_conversation2.ConnectToDiagServer(uds_message_2->GetHostIpAddress());
        
        // Use Tester One to send the diagnostic message to ECU1
        std::pair<diag::client::conversation::DiagClientConversation::DiagResult, diag::client::uds_message::UdsResponseMessagePtr> 
            ret_val_1{diag::client::conversation::DiagClientConversation::DiagResult::kDiagFailed, nullptr};
        
        ret_val_1 = diag_client_conversation1.SendDiagnosticRequest(std::move(uds_message_1));

        if(ret_val_1.first == 
            diag::client::conversation::DiagClientConversation::DiagResult::kDiagSuccess) {
            std::cout << "diag_client_conversation1 Total size: " << ret_val_1.second->GetPayload().size() << std::endl;
            
            // Print the payload
            for (auto const byte: ret_val_1.second->GetPayload()) {
                std::cout << "diag_client_conversation1 byte: " << std::hex << static_cast<int>(byte) << std::endl;
            }
        }

        // Use Tester Two to send the diagnostic message to ECU2
        std::pair<diag::client::conversation::DiagClientConversation::DiagResult, diag::client::uds_message::UdsResponseMessagePtr> 
            ret_val_2{diag::client::conversation::DiagClientConversation::DiagResult::kDiagFailed, nullptr};
        
        ret_val_2 = diag_client_conversation2.SendDiagnosticRequest(std::move(uds_message_2));

        if(ret_val_2.first == 
            diag::client::conversation::DiagClientConversation::DiagResult::kDiagSuccess) {
            std::cout << "diag_client_conversation2 Total size: " << ret_val_2.second->GetPayload().size() << std::endl;
            
            // Print the payload
            for (auto const byte: ret_val_2.second->GetPayload()) {
                std::cout << "diag_client_conversation2 byte: " << std::hex << static_cast<int>(byte) << std::endl;
            }
        }

        // Disconnect Tester One from ECU1 with remote ip address 192.168.0.2
        diag_client_conversation1.DisconnectFromDiagServer();

        // Disconnect Tester Two from ECU2 with remote ip address 192.168.0.3
        diag_client_conversation2.DisconnectFromDiagServer();

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }    
    
    // shutdown all the conversation
    diag_client_conversation1.Shutdown();
    diag_client_conversation2.Shutdown();

    // important to release all the resources by calling de-inilialize
    diagclient->DeInitialize();

    return (0);
}
