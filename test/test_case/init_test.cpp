/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include "main.h"
#include "include/create_diagnostic_client.h"

namespace doip_client{

TEST_F(DoipClientFixture, StartupAndTermination) {
  // Get conversation for tester one
  diag::client::conversation::DiagClientConversation& diag_client_conversation1{
    GetDiagClientRef().GetDiagnosticClientConversation("DiagTesterOne")};
  
  diag_client_conversation1.Startup();
  diag_client_conversation1.Shutdown();
}

} // doip_client