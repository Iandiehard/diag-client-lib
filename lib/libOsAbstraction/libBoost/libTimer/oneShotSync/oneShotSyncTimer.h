#ifndef ONESHOTSYNCTIMER_H
#define ONESHOTSYNCTIMER_H

#include "oneShotTimer_Types.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libTimer {
namespace oneShot {

// Clock Type
using SteadyClock           = boost::asio::steady_timer;
using HighResolutionClock   = boost::asio::high_resolution_timer;

using BoostTimer            = SteadyClock;
using msTime                = boost::asio::chrono::milliseconds;

class oneShotSyncTimer
{
    public:
        //ctor
        oneShotSyncTimer();
        //dtor
        virtual ~oneShotSyncTimer();
        // Start timer
        void SyncWait(int msec);
    private:
        // io contex 
	    boost::asio::io_context io_e;
};

} // oneShot
} // libTimer
} // libBoost
} // libOsAbstraction


#endif // ONESHOTSYNCTIMER_H
