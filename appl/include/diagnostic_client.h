/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H

#include <string>
#include "diagnostic_client_conversation.h"

namespace diag {
namespace client {

// Description   : Diagnostic client class acts as generic diagnostic tester to diagnose multiple ECU
class DiagClient {

public:
    // Description   : Construct the instance of diag-client
    // @requirement  : DiagClientLib-Construction
    DiagClient() = default;

    // Description   : Destruct the instance of diag-client
    // @requirement  : DiagClientLib-Destruction
    virtual ~DiagClient() = default;

    // Description   : Function to initialize the diag-client
    // @requirement  : DiagClientLib-Initialization
    virtual void Initialize() = 0;

    // Description   : Function to de-initialize the diag-client
    // @requirement  : DiagClientLib-DeInitialization
    virtual void DeInitialize() = 0;

    // Description   : Function to get the diag-client conversation reference
    // @param input  : Name of conversation present in json file
    // @return value : Reference to diag-client conversation object
    // @requirement  : DiagClientLib-MultipleTester-Connection, DiagClientLib-Conversation-Construction
    virtual diag::client::conversation::DiagClientConversation&
                        GetDiagnosticClientConversation(std::string conversation_name) = 0;
};

} // client
} // diag

#endif // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H
