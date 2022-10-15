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
oneShotSyncTimer::oneShotSyncTimer() 
            :timer_ptr_e(std::make_unique<BoostTimer>(io_e)) {
    DLT_REGISTER_CONTEXT(oneshotsync_timer_ctx,"osyt","Oneshot timer Context");
}

//dtor
oneShotSyncTimer::~oneShotSyncTimer() {
    DLT_UNREGISTER_CONTEXT(oneshotsync_timer_ctx);
}

// start the timer
oneShotSyncTimer::timer_state 
    oneShotSyncTimer::Start(int msec) {

    timer_state retval = timer_state::kNoTimeout;
    
    DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG, 
        DLT_CSTRING("Oneshot Timer start requested"));

    timer_ptr_e->expires_after(msTime(msec));
    timer_ptr_e->async_wait(boost::bind(&oneShotSyncTimer::Timeout, this, 
                            boost::placeholders::_1));
    auto start = std::chrono::system_clock::now();
    // blocking io call
    io_e.restart();
    io_e.run();

    // check the error code
    if(error_e != boost::asio::error::operation_aborted) {
        DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Oneshot Timer Timed out"));

        retval = timer_state::kTimeout;
    }
    else {
        DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG, 
            DLT_CSTRING("Oneshot Timer stop requested"));       
    }
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG, 
        DLT_CSTRING("Elapsed time: "), 
        DLT_FLOAT64(elapsed_seconds.count()),
        DLT_CSTRING("seconds"));
    return retval;
}

// stop the timer
void oneShotSyncTimer::Stop() {
    DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG, 
        DLT_CSTRING("Oneshot Timer stop requested"));
    timer_ptr_e->cancel();
}

// Is the timer active ?
bool oneShotSyncTimer::IsActive() {
    return(!io_e.stopped());
}
// function called when time elapses
void oneShotSyncTimer::Timeout(const boost::system::error_code& error) {
    error_e = error;
    if(error_e == boost::asio::error::operation_aborted) {
        io_e.stop();
    }
}

} // oneShot
} // libTimer
} // libBoost
} // libOsAbstraction
