/* Diagnostic Client library
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
        :timer_ptr_(std::make_unique<BoostTimer>(io_e)) {
    DLT_REGISTER_CONTEXT(oneshotsync_timer_ctx,"osyt","Oneshot timer Context");
}

//dtor
oneShotSyncTimer::~oneShotSyncTimer() {
    DLT_UNREGISTER_CONTEXT(oneshotsync_timer_ctx);
}

// start the timer
auto oneShotSyncTimer::Start(int msec)
    noexcept -> timer_state {

    timer_state retval = timer_state::kIdle;

    DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG,
            DLT_CSTRING("Oneshot Timer start requested"));

    timer_ptr_->expires_after(msTime(msec));
    timer_ptr_->async_wait(boost::bind(&oneShotSyncTimer::Timeout, this,
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
        retval = timer_state::kCancelRequested;
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG,
            DLT_CSTRING("Elapsed time: "),
            DLT_FLOAT64(elapsed_seconds.count()),
            DLT_CSTRING("seconds"));

    return retval;
}

// stop the timer
auto oneShotSyncTimer::Stop() noexcept -> void {
    DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG, 
        DLT_CSTRING("Oneshot Timer stop requested"));
    timer_ptr_->cancel();
}

// Completion handler triggered from async_wait
void oneShotSyncTimer::Timeout(const boost::system::error_code& error) {
    error_e = error;
    if(error_e == boost::asio::error::operation_aborted) {
        // timer stop was requested
        io_e.stop();
    }
}

} // oneShot
} // libTimer
} // libBoost
} // libOsAbstraction
