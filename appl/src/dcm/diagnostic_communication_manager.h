/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _DIAGNOSTIC_COMMUNICATION_MANAGER_H_
#define _DIAGNOSTIC_COMMUNICATION_MANAGER_H_

/* includes */
#include "common/diagnostic_manager.h"
#include "conversion/conversionManager.h"
#include "dcm/connection/udsTransportProtocolManager.h"
#include "dcm/config_parser/config_parser_type.h"


namespace diag {
namespace client {
namespace dcm{

/*
 @ Class Name        : DCM Client
 @ Class Description : Class to create Diagnostic Manager Client functionality                           
 */
class DCMClient: public diag::client::common::DiagnosticManager
{
public:
    //ctor
    explicit DCMClient(diag::client::common::property_tree &ptree);
    
    //dtor
    virtual ~DCMClient();
    
    // Initialize
    void Initialize() override;
    
    // Run
    void Run() override;
    
    // Shutdown
    void Shutdown() override;
    
    // Function to get the diagnostic client conversion
    diag::client::conversion::DiagClientConversion& 
                    GetDiagnosticClientConversion(std::string conversion_name);
private:
    // uds transport protocol Manager 
    std::unique_ptr<uds_transport::UdsTransportProtocolManager> uds_transport_protocol_mgr;
    
    // conversion manager
    std::unique_ptr<conversion_manager::ConversionManager> conversion_mgr;
    
    // map to store conversion pointer along with conversion name
    std::unordered_map<std::string, std::unique_ptr<diag::client::conversion::DiagClientConversion>> diag_client_conversion_map;
       
    // function to read from property tree to config structure
    diag::client::config_parser::ConversionConfig 
            getConversionConfig(diag::client::common::property_tree & ptree);
        
    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(dcm_client);
};

} // dcm
} // client
} // diag



#endif
