/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "doip_handler/doip_udp_handler.h"
#include "doip_handler/logger.h"
#include "include/create_diagnostic_client.h"
#include "include/diagnostic_client.h"

namespace doip_client {

// Diag Test Server Udp Ip Address
const std::string DiagUdpIpAddress{"172.16.25.128"};

// Port number
constexpr std::uint16_t DiagUdpPortNum{13400u};

// Path to json file
const std::string DiagClientJsonPath{"../../diag-client-lib/appl/etc/diag_client_config.json"};

class DoipClientFixture : public ::testing::Test {
protected:
  DoipClientFixture()
      : diag_client_{diag::client::CreateDiagnosticClient(DiagClientJsonPath)},
        doip_udp_handler_{DiagUdpIpAddress, DiagUdpPortNum} {
    // Initialize logger
    ara::diag::doip::logger::LibGtestLogger::GetLibGtestLogger();
    // Initialize doip test handler
    doip_udp_handler_.Initialize();
    // Initialize diag client library
    diag_client_->Initialize();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  ~DoipClientFixture() override {
    // De-initialize diag client library
    diag_client_->DeInitialize();
    // De-initialize doip test handler
    doip_udp_handler_.DeInitialize();
  }

  void SetUp() override {}

  void TearDown() override {}

  // Function to get Diag client library reference
  auto GetDiagClientRef() noexcept -> diag::client::DiagClient& { return *diag_client_; }

  // Function to get Doip Test Handler reference
  auto GetDoipTestUdpHandlerRef() noexcept -> ara::diag::doip::DoipUdpHandler& { return doip_udp_handler_; }

private:
  // diag client library
  std::unique_ptr<diag::client::DiagClient> diag_client_;

  // doip test handler
  ara::diag::doip::DoipUdpHandler doip_udp_handler_;
};

}  // namespace doip_client
