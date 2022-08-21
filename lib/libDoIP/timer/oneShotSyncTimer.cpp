/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "oneShotSyncTimer.h"

namespace ara{
namespace diag{
namespace doip{
namespace one_shot_timer{

//ctor
oneShotSyncTimer::oneShotSyncTimer() {
}

//dtor
oneShotSyncTimer::~oneShotSyncTimer() {
}

// start the timer
oneShotSyncTimer::timer_state oneShotSyncTimer::SyncWait(int msec) {
    oneShotSyncTimer::timer_state ret_val{oneShotSyncTimer::timer_state::kNoTimeout};
    std::unique_lock<std::mutex> lck(mutex_e);
    if(cond_var_e.wait_until(lck, Clock::now() + msTime(msec)) 
        == std::cv_status::timeout){
        ret_val = oneShotSyncTimer::timer_state::kTimeout;
    }
    else {
        // no timeout
    }
    return ret_val;
}

// stop the timer
void oneShotSyncTimer::StopWait() {
    cond_var_e.notify_all();
}

} // oneShot
} // doip
} // diag
} // ara  