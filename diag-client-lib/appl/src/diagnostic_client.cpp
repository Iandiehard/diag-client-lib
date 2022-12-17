/* Diagnostic Client library
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

std::unique_ptr<diag::client::DiagClient> CreateDiagnosticClient(
                                                            std::string diag_client_config_path) {
    return (std::make_unique<diag::client::DiagClientImpl>(diag_client_config_path));
}

} // client
} // diag