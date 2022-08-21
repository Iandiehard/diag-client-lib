/* includes */
#include "diagnostic_communication_manager.h"

namespace diag {
namespace client {
namespace dcm{
/*
 @ Class Name        : DCM
 @ Class Description : Class to create Diagnostic Manager Client functionality                           
 */

// ctor
DCM::DCM(diag::client::common::property_tree &ptree)
        : DiagnosticManager(ptree)
        , uds_transport_protocol_mgr(std::make_unique<uds_transport::UdsTransportProtocolManager>())
        , conversion_mgr(std::make_unique<conversion_manager::ConversionManager>(getConversionConfig(ptree), *uds_transport_protocol_mgr)) {
}

// dtor
DCM::~DCM() {
}

// Initialize
void DCM::Initialize() {
    // start Conversion Manager
    conversion_mgr->Startup();
    // start all the udsTransportProtocol Layer
    uds_transport_protocol_mgr->Startup();
}

// Run
void DCM::Run() {
    // run udsTransportProtocol layer
    uds_transport_protocol_mgr->Run();
}

// shutdown DCM
void DCM::Shutdown() {
    // shutdown Conversion Manager
    conversion_mgr->Shutdown();
    // shutdown udsTransportProtocol layer
    uds_transport_protocol_mgr->Shutdown();
}

// Function to get the client conversion
diag::client::conversion::DiagClientConversion&
                DCM::GetDiagnosticClientConversion(std::string conversion_name) {
    diag::client::conversion::DiagClientConversion* ret_conversion = nullptr;
    std::unique_ptr<diag::client::conversion::DiagClientConversion> conversion =
        conversion_mgr->GetDiagnosticClientConversion(conversion_name);
    if(conversion != nullptr) {
        diag_client_conversion_map.insert(
            std::pair<std::string, std::unique_ptr<diag::client::conversion::DiagClientConversion>>(
                                    conversion_name,
                                    std::move(conversion)
            ));
        ret_conversion = diag_client_conversion_map.at(conversion_name).get();
    }
    else {
        // error logging, no conversion found
    }
    return *ret_conversion;
}

// Todo: Remove this function
void DCM::print(diag::client::common::property_tree const& pt) {
    diag::client::common::property_tree::const_iterator end = pt.end();
    for (diag::client::common::property_tree::const_iterator it = pt.begin(); it != end; ++it) {
        std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
        print(it->second);
    }
}

// Function to get read from json tree and return the config structure
diag::client::config_parser::ConversionConfig DCM::getConversionConfig(diag::client::common::property_tree & ptree) {
    diag::client::config_parser::ConversionConfig config;
    // get total number of conversion
    config.num_of_conversion =  ptree.get<uint8_t>("Conversation.NumberOfConversion");
    // loop through all the conversion
    for(diag::client::common::property_tree::value_type &conversion_ptr : 
            ptree.get_child("Conversation.ConversionProperty")) {
        diag::client::config_parser::conversionType conversion;
        conversion.conversionName               = conversion_ptr.second.get<std::string>("ConversionName");
        conversion.p2ClientMax                  = conversion_ptr.second.get<uint16_t>("p2ClientMax");
        conversion.p2StarClientMax              = conversion_ptr.second.get<uint16_t>("p2StarClientMax");
        conversion.txBufferSize                 = conversion_ptr.second.get<uint16_t>("TxBufferSize");
        conversion.rxBufferSize                 = conversion_ptr.second.get<uint16_t>("RxBufferSize");
        conversion.sourceAddress                = conversion_ptr.second.get<uint16_t>("SourceAddress");
        conversion.targetAddress                = conversion_ptr.second.get<uint16_t>("TargetAddress");
        conversion.network.tcpIpAddress         = conversion_ptr.second.get<std::string>("Network.TcpIpAddress");
        conversion.network.udpIpAddress         = conversion_ptr.second.get<std::string>("Network.UdpIpAddress");
        conversion.network.udpBroadcastAddress  = conversion_ptr.second.get<std::string>("Network.UdpBroadcastAddress");
        conversion.network.portNum              = conversion_ptr.second.get<uint16_t>("Network.Port");
        config.conversions.push_back(conversion);
    }
    return config;
}

} // dcm
} // client
} // diag
