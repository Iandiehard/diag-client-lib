/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H

#include <memory>
#include "diagnostic_client.h"

namespace diag {
namespace client {

// Description   : Function to create an instance of Diagnostic Client
// @param input  : path to diag client config file 
// @return value : unique_ptr to diag client object
std::unique_ptr<diag::client::DiagClient> CreateDiagnosticClient(
                                                            std::string diag_client_config_path);

} // client
} // diag

#endif // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H
