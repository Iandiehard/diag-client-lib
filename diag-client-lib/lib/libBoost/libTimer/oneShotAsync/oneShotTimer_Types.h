/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ONESHOTTIMER_TYPES_H
#define ONESHOTTIMER_TYPES_H

#include "libCommon/Boost_Header.h"
#include "common_Header.h"

//Typedef

// Function template for callback during timer elapse
using TimerHandler = std::function<void(void )>;

#endif // ONESHOTTIMER_TYPES_H
