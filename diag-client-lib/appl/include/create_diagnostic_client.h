/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/** @file create_diagnostic_client.h
 *  @brief Entry function to create Diagnostic Client library
 *  @author Avijit Dey
 */
#ifndef DIAG_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H_
#define DIAG_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H_

#include <memory>
#include <string>

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
 */
std::unique_ptr<diag::client::DiagClient> CreateDiagnosticClient(const std::string &diag_client_config_path);

}  // namespace client
}  // namespace diag

#endif  // DIAG_CLIENT_LIB_APPL_INCLUDE_CREATE_DIAGNOSTIC_CLIENT_H_