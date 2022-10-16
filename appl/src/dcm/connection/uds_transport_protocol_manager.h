/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONNECTION_UDS_TRANSPORT_PROTOCOL_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONNECTION_UDS_TRANSPORT_PROTOCOL_MANAGER_H

#include "ara/diag/uds_transport/protocol_handler.h"

namespace diag {
namespace client {

// forward declaration
namespace conversation_manager{
class ConversationManager;
}

namespace uds_transport{

/*
 @ Class Name        : UdsTransportProtocolManager
 @ Class Description : This class must be instantiated by user for using the transport protocol functionalities.
                       This will inherit uds transport protocol handler
 */

class UdsTransportProtocolManager : public ::ara::diag::uds_transport::UdsTransportProtocolMgr {

public:
    //ctor
    UdsTransportProtocolManager(); 
    
    //dtor
    virtual ~UdsTransportProtocolManager();
    
    // initialize all the transport protocol handler
    void Startup();
    
    // start all the transport protocol handler
    void Run();
    
    // terminate all the transport protocol handler
    void Shutdown();

    // store doip transport handler
    std::unique_ptr<::ara::diag::uds_transport::UdsTransportProtocolHandler> doip_transport_handler;
    
    // handler id count
    ::ara::diag::uds_transport::UdsTransportProtocolHandlerID handler_id_count = 0;
};


} // uds_transport
} // client
} // diag

#endif // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONNECTION_UDS_TRANSPORT_PROTOCOL_MANAGER_H
