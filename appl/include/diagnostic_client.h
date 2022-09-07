/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _DIAGNOSTIC_CLIENT_H_
#define _DIAGNOSTIC_CLIENT_H_

#include <memory>
#include <vector>
#include "diagnostic_client_conversion.h"

namespace diag {
namespace client {

class DiagClient {

public:
    // ctor
    inline DiagClient() {}

    // dtor
    inline virtual ~DiagClient() = default;

    // Initialize
    virtual void Initialize() = 0;

    // De-Initialize
    virtual void DeInitialize() = 0;

    // Get Required Conversion based on Conversion Name
    virtual diag::client::conversion::DiagClientConversion& 
                        GetDiagnosticClientConversion(std::string conversion_name) = 0;
};

} // client
} // diag

#endif // _DIAGNOSTIC_CLIENT_H_