/* MANDAREIN Diagnostic Client library
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
