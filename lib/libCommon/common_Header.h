/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

// includes

#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <stdint.h>
#include <signal.h>
#include <future>
#include <chrono>
#include <ctime>
#include <dlt/dlt.h>

/* Magic numbers */
#define BYTE_POS_ZERO                   (uint8_t(0))
#define BYTE_POS_ONE                    (uint8_t(1))
#define BYTE_POS_TWO                    (uint8_t(2))
#define BYTE_POS_THREE                  (uint8_t(3))
#define BYTE_POS_FOUR                   (uint8_t(4))

typedef enum
{
   E_OK         = 0x00,
   E_NOT_OK,
   E_BUSY
}Std_ReturnType;



#endif // COMMON_HEADER_H
