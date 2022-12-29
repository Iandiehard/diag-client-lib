/* Diagnostic Client library
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
#include <cstdint>
#include <csignal>
#include <future>
#include <chrono>
#include <ctime>
#include <dlt/dlt.h>

/* Magic numbers */
constexpr uint8_t BYTE_POS_ZERO                   = 0x00;
constexpr uint8_t BYTE_POS_ONE                    = 0x01;
constexpr uint8_t BYTE_POS_TWO                    = 0x02;
constexpr uint8_t BYTE_POS_THREE                  = 0x03;
constexpr uint8_t BYTE_POS_FOUR                   = 0x04;
constexpr uint8_t BYTE_POS_FIVE                   = 0x05;
constexpr uint8_t BYTE_POS_SIX                    = 0x06;
constexpr uint8_t BYTE_POS_SEVEN                  = 0x07;

typedef enum
{
   E_OK         = 0x00,
   E_NOT_OK,
   E_BUSY
}Std_ReturnType;



#endif // COMMON_HEADER_H
