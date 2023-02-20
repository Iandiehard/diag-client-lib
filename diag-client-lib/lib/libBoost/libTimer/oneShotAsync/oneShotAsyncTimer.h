/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef ONESHOTTIMER_H
#define ONESHOTTIMER_H

#include "oneShotTimer_Types.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libTimer {
namespace oneShot {
// Clock Type
using SteadyClock = boost::asio::steady_timer;
using HighResolutionClock = boost::asio::high_resolution_timer;
using BoostTimer = SteadyClock;
using msTime = boost::asio::chrono::milliseconds;

class oneShotAsyncTimer {
public:
  //ctor
  oneShotAsyncTimer();

  //dtor
  virtual ~oneShotAsyncTimer();

  // Start timer
  void Start(int msec, TimerHandler timerHandler);

  // Stop Timer
  void Stop();

  // Function to check whether timer is active or not
  bool IsActive();

private:
  // io contex
  boost::asio::io_context io_e;
  // flag to terminate the thread
  std::atomic_bool exit_request_e;
  // flag th start the thread
  std::atomic_bool running_e;
  // threading var
  std::thread thread_e;
  // timer - next timeout
  std::unique_ptr<BoostTimer> timer_ptr_e;
  // conditional variable to block the thread
  std::condition_variable cond_var_e;
  // locking critical section
  mutable std::mutex mutex_e;
  // handler
  TimerHandler timerHandler_e;

  // timeout funtion
  void Timeout(const boost::system::error_code &error);

  // function invoked in thread
  void Run();
};
}  // namespace oneShot
}  // namespace libTimer
}  // namespace libBoost
}  // namespace libOsAbstraction
#endif  // ONESHOTTIMER_H
