/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
DCMClient::DCMClient(diag::client::common::property_tree &ptree)
        : DiagnosticManager(ptree)
        , uds_transport_protocol_mgr(std::make_unique<uds_transport::UdsTransportProtocolManager>())
        , conversion_mgr(std::make_unique<conversion_manager::ConversionManager>(getConversionConfig(ptree), *uds_transport_protocol_mgr)) {
    DLT_REGISTER_CONTEXT(dcm_client,"dcmc","DCM Client Context");
}

// dtor
DCMClient::~DCMClient() {
    DLT_UNREGISTER_CONTEXT(dcm_client);
}

// Initialize
void DCMClient::Initialize() {
    // start Conversion Manager
    conversion_mgr->Startup();
    // start all the udsTransportProtocol Layer
    uds_transport_protocol_mgr->Startup();

    DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
        DLT_STRING("DCM Client Initialize done"));
}

// Run
void DCMClient::Run() {
    // run udsTransportProtocol layer
    uds_transport_protocol_mgr->Run();
}

// shutdown DCM
void DCMClient::Shutdown() {
    // shutdown Conversion Manager
    conversion_mgr->Shutdown();
    // shutdown udsTransportProtocol layer
    uds_transport_protocol_mgr->Shutdown();
    
    DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
        DLT_STRING("DCM Client Shutdown done"));
}

// Function to get the client conversion
diag::client::conversion::DiagClientConversion&
        DCMClient::GetDiagnosticClientConversion(std::string conversion_name) {
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
        DLT_LOG(dcm_client, DLT_LOG_ERROR, 
            DLT_STRING("Requested Diagnostic Client conversion not found with name: "), 
            DLT_STRING(conversion_name.c_str()));
    }
    return *ret_conversion;
}

// Function to get read from json tree and return the config structure
diag::client::config_parser::ConversionConfig
        DCMClient::getConversionConfig(diag::client::common::property_tree & ptree) {
    diag::client::config_parser::ConversionConfig config;
    // get total number of conversion
    config.num_of_conversion =  ptree.get<uint8_t>("Conversation.NumberOfConversion");
    // loop through all the conversion
    for(diag::client::common::property_tree::value_type &conversion_ptr : 
            ptree.get_child("Conversation.ConversionProperty")) {
        diag::client::config_parser::conversionType conversion;
        
        conversion.conversionName               = conversion_ptr.second.get<std::string>("ConversionName");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("ConversionName : "), 
            DLT_STRING(conversion.conversionName.c_str()));
        
        conversion.p2ClientMax                  = conversion_ptr.second.get<uint16_t>("p2ClientMax");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("p2ClientMax : "), 
            DLT_UINT16(conversion.p2ClientMax));
        
        conversion.p2StarClientMax              = conversion_ptr.second.get<uint16_t>("p2StarClientMax");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("p2StarClientMax : "), 
            DLT_UINT16(conversion.p2StarClientMax));
        
        conversion.txBufferSize                 = conversion_ptr.second.get<uint16_t>("TxBufferSize");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("TxBufferSize : "), 
            DLT_UINT16(conversion.txBufferSize));
        
        conversion.rxBufferSize                 = conversion_ptr.second.get<uint16_t>("RxBufferSize");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("RxBufferSize : "), 
            DLT_UINT16(conversion.rxBufferSize));

        conversion.sourceAddress                = conversion_ptr.second.get<uint16_t>("SourceAddress");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("SourceAddress : "), 
            DLT_UINT16(conversion.sourceAddress));

        conversion.targetAddress                = conversion_ptr.second.get<uint16_t>("TargetAddress");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("TargetAddress : "), 
            DLT_UINT16(conversion.targetAddress));

        conversion.network.tcpIpAddress         = conversion_ptr.second.get<std::string>("Network.TcpIpAddress");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("Network.TcpIpAddress : "), 
            DLT_STRING(conversion.network.tcpIpAddress.c_str()));

        conversion.network.udpIpAddress         = conversion_ptr.second.get<std::string>("Network.UdpIpAddress");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("Network.UdpIpAddress : "), 
            DLT_STRING(conversion.network.udpIpAddress.c_str()));

        conversion.network.udpBroadcastAddress  = conversion_ptr.second.get<std::string>("Network.UdpBroadcastAddress");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("Network.UdpBroadcastAddress : "), 
            DLT_STRING(conversion.network.udpBroadcastAddress.c_str()));

        conversion.network.portNum              = conversion_ptr.second.get<uint16_t>("Network.Port");
        DLT_LOG(dcm_client, DLT_LOG_VERBOSE, 
            DLT_STRING("Network.Port : "), 
            DLT_UINT16(conversion.network.portNum));

        config.conversions.push_back(conversion);
    }
    return config;
}

} // dcm
} // client
} // diag
