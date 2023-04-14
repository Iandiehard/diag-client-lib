/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "src/common/diagnostic_manager.h"

namespace diag {
namespace client {
namespace common {
/*
 @ Class Name        : Diagnostic_manager
 @ Class Description : Parent class to create DCM and DEM class                            
 */
DiagnosticManager::DiagnosticManager(/* DiagnosticManagerPluginFactory &plugin_factory, */
                                     property_tree &ptree)
    : ptree_e{ptree},
      exit_requested_{false} {}

DiagnosticManager::~DiagnosticManager() {
  {
    std::lock_guard<std::mutex> lock{mutex_};
    exit_requested_ = true;
  }
  cond_var.notify_all();
}

// Main function which keeps DCM alive
void DiagnosticManager::Main() {
  // Initialize the module
  Initialize();
  // Run the module
  Run();
  // Entering infinite loop
  while (!exit_requested_) {
    std::unique_lock<std::mutex> lck(mutex_);
    cond_var.wait(lck, [this]() { return exit_requested_; });
    // Thread exited
  }
  // Shutdown Module
  Shutdown();
}

// Function to shut down the component
void DiagnosticManager::SignalShutdown() {
  {
    std::lock_guard<std::mutex> lock{mutex_};
    exit_requested_ = true;
  }
  cond_var.notify_all();
}
}  // namespace common
}  // namespace client
}  // namespace diag
