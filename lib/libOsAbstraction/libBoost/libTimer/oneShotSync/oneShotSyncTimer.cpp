/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
