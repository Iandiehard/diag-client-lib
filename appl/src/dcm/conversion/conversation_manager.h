/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H

/* includes */
#include "common_Header.h"
#include "dcm/connection/uds_transport_protocol_manager.h"
#include "dcm/config_parser/config_parser_type.h"
#include "dcm/conversion/dm_conversation.h"

namespace diag {
namespace client {
namespace conversation_manager {

/*
 @ Class Name        : ConversationManager
 @ Class Description : Class to manage all the conversion created from usr request                           
 */
class ConversationManager
{
public:
    
    // ctor
    explicit ConversationManager(diag::client::config_parser::ConversationConfig config,
                                 diag::client::uds_transport::UdsTransportProtocolManager &uds_transport_mgr);
    
    // dtor
    ~ConversationManager() = default;
    
    // startup
    void Startup();
    
    // shutdown
    void Shutdown();

    // Get the required conversion
    std::unique_ptr<diag::client::conversation::DmConversation>
                GetDiagnosticClientConversion(std::string conversion_name);
private:
    
    // store uds transport manager
    uds_transport::UdsTransportProtocolManager &uds_transport_mgr_e;
    
    // store the conversion name with conversion configurations
    std::map<std::string, ::ara::diag::conversion_manager::ConversionIdentifierType> conversation_config_e;

    // function to create or find new conversion
    void CreateConversationConfig(diag::client::config_parser::ConversationConfig config);
};


} // conversation_manager
} // client
} // diag


#endif // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONVERSATION_CONVERSATION_MANAGER_H
