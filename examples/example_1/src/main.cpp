#include "create_diagnostic_client.h"
#include "uds_message.h"
#include <chrono>
#include <thread>

/*
 * Main Entry point of diag client example */
int main(void) {
    // Create the Diagnostic client and pass the config as well
    std::unique_ptr<diag::client::DiagClient> diagclient =
                diag::client::CreateDiagnosticClient("etc/diag_client_config.json");
    
    // Initialize the Diagnostic Client library
    diagclient->Initialize();
    
    // Get conversion for tester one
    diag::client::conversion::DiagClientConversion& diag_client_conversion1 =
                diagclient->GetDiagnosticClientConversion("DiagTesterOne");
    // Start the conversion, its important to start the conversion
    diag_client_conversion1.Startup();

    // Get conversion for tester two
    diag::client::conversion::DiagClientConversion& diag_client_conversion2 =
                diagclient->GetDiagnosticClientConversion("DiagTesterTwo");
    // Start the conversion, its important to start the conversion
    diag_client_conversion2.Startup();

    // Create a uds payload 10 01 ( Default Session )
    diag::client::uds_message::ByteVector payload;
    payload.push_back(0x10);
    payload.push_back(0x01);

    while(1) {
        // Create a uds message with payload
        diag::client::uds_message::UdsRequestMessagePtr uds_message = 
                            std::make_unique<UdsMessage>("192.168.0.1", payload);

        // Connect Tester One to ECU1 with remote ip address 192.168.0.1
        diag_client_conversion1.ConnectToDiagServer("192.168.0.1");

        // Connect Tester Two to ECU2 with remote ip address 192.168.0.2
        diag_client_conversion1.ConnectToDiagServer("192.168.0.2");

        // Use Tester One to send the diagnostic message to ECU1
        diag_client_conversion1.SendDiagnosticRequest(std::move(uds_message));
        
        // Use Tester Two to send the diagnostic message to ECU2
        diag_client_conversion2.SendDiagnosticRequest(std::move(uds_message));

        // Disconnect Tester One from ECU1 with remote ip address 192.168.0.1
        diag_client_conversion1.DisconnectFromDiagServer();

        // Disconnect Tester Two from ECU2 with remote ip address 192.168.0.2
        diag_client_conversion2.DisconnectFromDiagServer();

        // sleep and repeat
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // shutdown all the conversion
    diag_client_conversion1.Shutdown();
    diag_client_conversion2.Shutdown();

    // important to release all the resources by calling de-inilialize
    diagclient->DeInitialize();

    return (0);
}
