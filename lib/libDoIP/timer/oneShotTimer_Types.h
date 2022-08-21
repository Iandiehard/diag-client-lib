#ifndef ONE_SHOT_SYNC_TIMER_TYPES_H
#define ONE_SHOT_SYNC_TIMER_TYPES_H

#include "common_Header.h"

//Typedef

// Function template for callback during timer elapse
using TimerHandler = std::function<void(void )>;

#endif // ONESHOTTIMER_TYPES_H
