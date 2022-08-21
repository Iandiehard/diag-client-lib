/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _CONVERSION_MANAGER_H
#define _CONVERSION_MANAGER_H

/* includes */
#include "common_Header.h"
#include "dcm/connection/udsTransportProtocolManager.h"
#include "dcm/config_parser/config_parser_type.h"
#include "dcm/conversion/dmconversion.h"
// #include "diagnostic_client_conversion.h"

namespace diag {
namespace client {
namespace conversion_manager{

/*
 @ Class Name        : ConversionManager
 @ Class Description : Class to manage all the conversion created from usr request                           
 */
class ConversionManager
{
public:
    
    // ctor
    explicit ConversionManager(diag::client::config_parser::ConversionConfig config, 
                               diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr);
    
    // dtor
    ~ConversionManager();
    
    // startup
    void Startup();
    
    // shutdown
    void Shutdown();

    // Get the required conversion
    std::unique_ptr<diag::client::conversion::DmConversion> 
                GetDiagnosticClientConversion(std::string conversion_name);
private:
    
    // store uds transport manager
    uds_transport::UdsTransportProtocolManager &uds_transport_mgr_e;
    
    // store the conversion name with conversion configurations
    std::map<std::string, ::ara::diag::conversion_manager::ConversionIdentifierType> conversion_config_e;

    // function to create or find new conversion
    void CreateConverionConfig(diag::client::config_parser::ConversionConfig config);
};


} // conversion manager
} // client
} // diag


#endif // _CONVERSION_MANAGER_H
