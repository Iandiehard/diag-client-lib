/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* includes */
#include "dcm/conversion/conversionManager.h"
#include "dcm/conversion/dmconversion.h"

namespace diag {
namespace client {
namespace conversion_manager{

//ctor
ConversionManager::ConversionManager(diag::client::config_parser::ConversionConfig config, 
                                     diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr)
                 : uds_transport_mgr_e(uds_transport_mgr) {
    CreateConverionConfig(config);
}

//dtor
ConversionManager::~ConversionManager() {
}

// Initialize
void ConversionManager::Startup() {
}

// Shutdown
void ConversionManager::Shutdown() {
}

//
std::unique_ptr<diag::client::conversion::DmConversion>
            ConversionManager::GetDiagnosticClientConversion(std::string conversion_name) {
    std::unique_ptr<diag::client::conversion::DmConversion> dm_coversion {};
    auto it = conversion_config_e.find(conversion_name);
    if(it != conversion_config_e.end()) {
        dm_coversion = std::make_unique<diag::client::conversion::DmConversion>(it->second);
        // Register the connection
        dm_coversion->RegisterConnection(
                uds_transport_mgr_e.doip_transport_handler->FindorCreateConnection(
                                            dm_coversion->dm_conversion_handler,
                                            it->second.tcp_address,
                                            it->second.udp_address,
                                            it->second.port_num
                ));

    }
    return dm_coversion;
}

// function to find or create conversion
void ConversionManager::CreateConverionConfig(diag::client::config_parser::ConversionConfig config)
{
    for(uint8_t conv_count = 0; conv_count < config.num_of_conversion; conv_count ++) {
        ::ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier;
        conversion_identifier.tx_buffer_size = config.conversions[conv_count].txBufferSize;
        conversion_identifier.rx_buffer_size = config.conversions[conv_count].rxBufferSize;
        conversion_identifier.p2_client_max = config.conversions[conv_count].p2ClientMax;
        conversion_identifier.p2_star_client_max = config.conversions[conv_count].p2StarClientMax;
        conversion_identifier.source_address = config.conversions[conv_count].sourceAddress;
        conversion_identifier.target_address = config.conversions[conv_count].targetAddress;
        conversion_identifier.tcp_address = config.conversions[conv_count].network.tcpIpAddress;
        conversion_identifier.udp_address = config.conversions[conv_count].network.udpIpAddress;
        conversion_identifier.udp_broadcast_address = config.conversions[conv_count].network.udpBroadcastAddress;
        conversion_identifier.port_num = config.conversions[conv_count].network.portNum;
        // push to config map
        conversion_config_e.insert(
            std::pair<std::string, ::ara::diag::conversion_manager::ConversionIdentifierType>(
                                                    config.conversions[conv_count].conversionName, 
                                                    conversion_identifier));
    }
}

} // conversion manager
} // client
} // diag