// includes
#include "oneShotSyncTimer.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libTimer {
namespace oneShot {

//ctor
oneShotSyncTimer::oneShotSyncTimer() {
}

//dtor
oneShotSyncTimer::~oneShotSyncTimer() {
}

// start the timer
void oneShotSyncTimer::SyncWait(int msec) {
    std::unique_ptr<BoostTimer>  timer_ptr_e;
    timer_ptr_e = std::make_unique<BoostTimer>(io_e, msTime(msec));
	timer_ptr_e->wait();
}

} // oneShot
} // libTimer
} // libBoost
} // libOsAbstraction
