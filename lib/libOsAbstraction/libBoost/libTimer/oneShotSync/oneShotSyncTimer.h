/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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

class oneShotSyncTimer {
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
    timer_state Start(int msec);
    
    // Stop Timer
    void Stop();
    
    // Function to check whether timer is active or not
    bool IsActive();
private:
    // io contex 
    boost::asio::io_context io_e;

    // timer - next timeout
    std::unique_ptr<BoostTimer>  timer_ptr_e;

    // error
    boost::system::error_code error_e;

    // timeout function
    void Timeout(const boost::system::error_code& error);

    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(oneshotsync_timer_ctx);
};

} // oneShot
} // libTimer
} // libBoost
} // libOsAbstraction


#endif // ONESHOTSYNCTIMER_H
