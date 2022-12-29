/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// includes
#include "oneShotAsyncTimer.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libTimer {
namespace oneShot {

//ctor
oneShotAsyncTimer::oneShotAsyncTimer()
            : exit_request_e(false)
            , running_e(false)
            , timer_ptr_e(std::make_unique<BoostTimer>(io_e)) {
    DLT_REGISTER_CONTEXT(oneshotasync_timer_ctx,"osasync","Oneshot timer Context");
    thread_e = std::thread(&oneShotAsyncTimer::Run, this);
}

//dtor
oneShotAsyncTimer::~oneShotAsyncTimer() {
    exit_request_e = true;
    running_e = false;
    timer_ptr_e->cancel();		// cancel all asynchronous wait before destruction 
    cond_var_e.notify_all();
    thread_e.join();
    DLT_UNREGISTER_CONTEXT(oneshotasync_timer_ctx);
}

// start the timer
void oneShotAsyncTimer::Start(int msec, TimerHandler timerHandler) {
    DLT_LOG(oneshotasync_timer_ctx, DLT_LOG_INFO, 
        DLT_CSTRING("Oneshot Timer start requested"));
    std::unique_lock<std::mutex> lck(mutex_e);
    timerHandler_e = timerHandler;
    timer_ptr_e->expires_after(msTime(msec));
    timer_ptr_e->async_wait(boost::bind(&oneShotAsyncTimer::Timeout, this, 
                            boost::placeholders::_1));
    running_e = true;
    cond_var_e.notify_all();
}

// stop the timer
void oneShotAsyncTimer::Stop() {
    DLT_LOG(oneshotasync_timer_ctx, DLT_LOG_INFO, 
        DLT_CSTRING("Oneshot Timer stop requested"));
    std::unique_lock<std::mutex> lck(mutex_e);
    timer_ptr_e->cancel();
    running_e = false;
}

// 
bool oneShotAsyncTimer::IsActive() {
    return(running_e);
}

// function called when time elapses
void oneShotAsyncTimer::Run() {
    std::unique_lock<std::mutex> lck(mutex_e);
    while (!exit_request_e) {
        if (!running_e) {
            cond_var_e.wait(lck);
        }
        if (running_e) {
            lck.unlock();
            DLT_LOG(oneshotasync_timer_ctx, DLT_LOG_INFO, 
                DLT_CSTRING("Oneshot Timer running"));
            io_e.run();
            lck.lock();
        }
    }
}

// function called when time elapses
void oneShotAsyncTimer::Timeout(const boost::system::error_code& error) {
    if(error != boost::asio::error::operation_aborted) {
        timerHandler_e();
        DLT_LOG(oneshotasync_timer_ctx, DLT_LOG_INFO, 
            DLT_CSTRING("Oneshot Timer Timed out"));
    }
    running_e = false;
}


} // oneShot
} // libTimer
} // libBoost
} // libOsAbstraction
