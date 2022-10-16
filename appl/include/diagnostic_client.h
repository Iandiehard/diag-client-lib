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

class DiagClient {

public:
    // ctor
    DiagClient() = default;

    // dtor
    virtual ~DiagClient() = default;

    // Initialize
    virtual void Initialize() = 0;

    // De-Initialize
    virtual void DeInitialize() = 0;

    // Get Required Conversion based on Conversion Name
    virtual diag::client::conversation::DiagClientConversation&
                        GetDiagnosticClientConversation(std::string conversion_name) = 0;
};

} // client
} // diag

#endif // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_H
