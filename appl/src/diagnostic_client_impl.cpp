/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "diagnostic_client.h"
#include "diagnostic_client_impl.h"

namespace diag {
namespace client {

// ctor
DiagClientImpl::DiagClientImpl(std::string dm_client_config)
                : diag::client::DiagClient() {
    // dlt register app & context
    DLT_REGISTER_APP("DCLT", "Diag Client Library");
    DLT_REGISTER_CONTEXT(diagclient_main,"main","Diag Client Main Context");

    // start parsing the config json file
    libOsAbstraction::libBoost::jsonparser::createJsonParser json_parser;
    json_parser.getJsonPtree(dm_client_config, ptree);
    
    // create single dcm instance and pass the config tree
    dcm_instance_ptr = std::make_unique<diag::client::dcm::DCMClient>(ptree);

    DLT_LOG(diagclient_main, DLT_LOG_INFO, 
        DLT_CSTRING("DiagClient instance created"));
}

// dtor
DiagClientImpl::~DiagClientImpl() {
    // de-register from dlt
    DLT_UNREGISTER_CONTEXT(diagclient_main);
    DLT_UNREGISTER_APP();
}

// Initialize all the resources and load the configs
void DiagClientImpl::Initialize(void) {
    // start DCM thread here
    _thread.push_back(std::thread(&diag::client::dcm::DCMClient::Main, std::ref(*dcm_instance_ptr.get())));
    DLT_LOG(diagclient_main, DLT_LOG_INFO, 
        DLT_CSTRING("DiagClient Initialize success"));
}

// De-initialize all the resource and free memory
void DiagClientImpl::DeInitialize(void) {
    // shutdown DCM module here
    dcm_instance_ptr->SignalShutdown();
    // destroy all threads here
    for(auto &thread_ptr : _thread) {
        thread_ptr.join();
    }
    DLT_LOG(diagclient_main, DLT_LOG_INFO, 
        DLT_CSTRING("DiagClient DeInitialized"));
}

// Get Required Conversion based on Conversion Name
diag::client::conversion::DiagClientConversion& 
            DiagClientImpl::GetDiagnosticClientConversion(std::string conversion_name) {
    return(dcm_instance_ptr->GetDiagnosticClientConversion(conversion_name));
}

} // client
} // diag