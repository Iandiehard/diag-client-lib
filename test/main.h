/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <thread>
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"
#include "doip_handler/doip_udp_handler.h"

namespace doip_client {

class DoipClientFixture : public ::testing::Test {
protected:
  DoipClientFixture()
    : diag_client_{diag::client::CreateDiagnosticClient(
    "/workspace/diag-client-lib/diag-client-lib/appl/etc/diag_client_config.json")},
      doip_udp_handler_{"172.16.25.128", 13400u} {
    // Initialize doip test handler
    doip_udp_handler_.Initialize();
  }
  
  ~DoipClientFixture() {
    // De-initialize doip test handler
    doip_udp_handler_.DeInitialize();
  }
  
  void SetUp() override {
    // Initialize diag client library
    diag_client_->Initialize();
  }
  
  void TearDown() override {
    // De-initialize diag client library
    diag_client_->DeInitialize();
  }
  
  // Function to get Diag client library reference
  auto GetDiagClientRef()
  noexcept -> diag::client::DiagClient & {
    return *diag_client_;
  }

  // Function to get Doip Test Handler reference
  auto GetDoipTestUdpHandlerRef()
    noexcept -> ara::diag::doip::DoipUdpHandler & {
    return doip_udp_handler_;
  }

private:
  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;

  // doip test handler
  ara::diag::doip::DoipUdpHandler doip_udp_handler_;
};

} // doip_client
