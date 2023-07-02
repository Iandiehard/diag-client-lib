/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H_
#define DIAG_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H_

#include <memory>
#include <string_view>

namespace diag {
namespace client {

// forward declaration
class DiagClient;

/**
 * @brief       Function to get the instance of Diagnostic Client Object.
 *              This instance to be further used for all the functionalities.
 * @param[in]   diag_client_config_path
 *              path to diag client config file
 * @return      std::unique_ptr<diag::client::DiagClient>
 *              Unique pointer to diag client object
 * @implements  DiagClientLib-Library-Support, DiagClientLib-ComParam-Settings
 */
std::unique_ptr<diag::client::DiagClient> CreateDiagnosticClient(std::string_view diag_client_config_path);

}  // namespace client
}  // namespace diag

#endif  // DIAG_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H_
