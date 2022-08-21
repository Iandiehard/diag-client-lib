
#include "common/diagnostic_manager.h"

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
    // Comp Main thread started, logging

    // Initialize the module
    Initialize();
    // Run the module
    Run();
    // Entering infinite loop
    std::unique_lock<std::mutex> lck(_mutex_lock);
    while (!exit_requested)
    {// do something , thread paused
        cond_var.wait(lck);
        // Thread exited
    }
    // Shutdown Module
    Shutdown();
}

// Function to shutdown the component
void DiagnosticManager::SignalShutdown() {
    exit_requested = true;
    cond_var.notify_all();
}


} // common
} // client
} // diag