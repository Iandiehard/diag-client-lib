/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"
#include "doip_handler/udp_socket_handler.h"

namespace doip_client {

class DoipClientFixture : public ::testing::Test {
protected:
  DoipClientFixture()
    : diag_client_{diag::client::CreateDiagnosticClient(
    "/tmp/diag-client-lib/diag-client-lib/appl/etc/diag_client_config.json")} {}
  
  ~DoipClientFixture() = default;
  
  void SetUp() override {
    // Initialize diag client library
    diag_client_->Initialize();
  }
  
  void TearDown() override {
    // De-initialize diag client library
    diag_client_->DeInitialize();
  }
  
  // Function to Diag client library reference
  auto GetDiagClientRef()
  noexcept -> diag::client::DiagClient & {
    return *diag_client_;
  }

private:
  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;
};

} // doip_client