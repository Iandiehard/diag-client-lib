#ifndef DIAG_CLIENT_ONE_SHOT_SYNC_TIMER_H
#define DIAG_CLIENT_ONE_SHOT_SYNC_TIMER_H

#include "common_Header.h"

namespace diag{
namespace client{
namespace one_shot_timer{

// Clock Type
using SteadyClock           = std::chrono::steady_clock;
using HighResolutionClock   = std::chrono::high_resolution_clock;

using Clock                 = HighResolutionClock;
using msTime                = std::chrono::milliseconds;

class oneShotSyncTimer 
{
public:
    // timer state
    enum class timer_state : std::uint8_t {
        kNoTimeout = 0,
        kTimeout
    };
    
    //ctor
    oneShotSyncTimer();

    //dtor
    virtual ~oneShotSyncTimer();

    // Start timer
    timer_state SyncWait(int msec);

    // Stop waiting
    void StopWait();
private:
    // conditional variable
    std::condition_variable cond_var_e;

    // locking critical section
    std::mutex mutex_e;
};

} // oneShotSyncTimer
} // client
} // diag


#endif // DIAG_CLIENT_ONE_SHOT_SYNC_TIMER_H
