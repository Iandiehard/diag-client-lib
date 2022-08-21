#ifndef _COMMON_DIAGNOSTIC_MANAGER_H_
#define _COMMON_DIAGNOSTIC_MANAGER_H_

/* includes */
#include "common_Header.h"
#include "libJsonParser/jsonParser.h"

namespace diag {
namespace client {
namespace common {

using property_tree = libOsAbstraction::libBoost::jsonparser::boostTree;

/*
 @ Class Name        : DiagnosticManager
 @ Class Description : Parent class to create DCM and DEM class                            
 */
class DiagnosticManager
{
public:
    //ctor
    explicit DiagnosticManager(/* DiagnosticManagerPluginFactory &plugin_factory, */ 
                                property_tree &ptree);
    // dtor
    virtual ~DiagnosticManager();
    // main function
    virtual void Main();
    // signal shutdown
    virtual void SignalShutdown();
    // Initialize
    virtual void Initialize() = 0;
    // Run
    virtual void Run() = 0;
    // Shutdown
    virtual void Shutdown() = 0;
protected:
    // store the json tree 
    property_tree &ptree_e;
private:
    // flag to terminate the main thread
    std::atomic_bool exit_requested;
    /* conditional variable to block the thread */
    std::condition_variable cond_var;
    // For locking critical section of code 
    std::mutex _mutex_lock;
};


} // common
} // client
} // diag



#endif // _COMMON_DIAGNOSTIC_MANAGER_H_