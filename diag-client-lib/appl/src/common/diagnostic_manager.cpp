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
//ctor
DiagnosticManager::DiagnosticManager(/* DiagnosticManagerPluginFactory &plugin_factory, */
                                    property_tree &ptree)
                                    : ptree_e(ptree)
                                    , exit_requested(false) {
}

//dtor
DiagnosticManager::~DiagnosticManager() {
    exit_requested = true;
}

// Main function which keeps DCM alive
void DiagnosticManager::Main() {
  // Initialize the module
  Initialize();
  // Run the module
  Run();
  // Entering infinite loop
  while (!exit_requested) {
    std::unique_lock<std::mutex> lck(_mutex_lock);
    cond_var.wait(lck);
    // Thread exited
  }
  // Shutdown Module
  Shutdown();
}

// Function to shut down the component
void DiagnosticManager::SignalShutdown() {
    exit_requested = true;
    cond_var.notify_all();
}


} // common
} // client
} // diag