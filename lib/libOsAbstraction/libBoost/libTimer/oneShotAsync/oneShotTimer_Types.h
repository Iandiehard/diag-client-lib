#ifndef ONESHOTTIMER_TYPES_H
#define ONESHOTTIMER_TYPES_H

#include "libCommon/Boost_Header.h"
#include "common_Header.h"

//Typedef

// Function template for callback during timer elapse
using TimerHandler = std::function<void(void )>;

#endif // ONESHOTTIMER_TYPES_H
