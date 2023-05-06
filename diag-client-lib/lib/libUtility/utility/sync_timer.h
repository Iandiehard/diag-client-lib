/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_SYNC_TIMER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_SYNC_TIMER_H

namespace libUtility {
namespace sync_timer {

#include <chrono>
#include <condition_variable>
#include <mutex>

// Clock Type

template<typename ClockType>
class SyncTimer final {
public:
  using Clock = ClockType;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::duration<Clock>;

  // timer state
  enum class TimerState : std::uint8_t { kIdle = 0, kCancelRequested, kTimeout };

  SyncTimer() : cond_var_{}, mutex_lock_{}, exit_request_{false}, start_running_{false} {}

  ~SyncTimer() {
    std::unique_lock<std::mutex> lck(mutex_lock_);
    exit_request_ = false;
    start_running_ = false;
    cond_var_.notify_all();
  }

  auto Start(std::chrono::milliseconds timeout) noexcept -> TimerState {
    std::unique_lock<std::mutex> lck(mutex_lock_);
    TimerState timer_state{TimerState::kIdle};
    start_running_ = true;
    TimePoint expiry_timepoint{Clock::now() + timeout};
    if (cond_var_.wait_until(lck, expiry_timepoint, [this, &expiry_timepoint, &timer_state]() {
          bool ret_val{false};
          if (!exit_request_) {
            TimePoint current_time{Clock::now()};
            // check for expiry
            if (current_time > expiry_timepoint) {
              // timeout
              timer_state = TimerState::kTimeout;
              ret_val = true;
            } else {
              if (!start_running_) {
                timer_state = TimerState::kCancelRequested;
                ret_val = true;
              }
            }
          } else {
            ret_val = true;
          }
          return ret_val;
        })) {}
    return timer_state;
  }

  void Stop() noexcept {
    std::unique_lock<std::mutex> lck(mutex_lock_);
    start_running_ = false;
    cond_var_.notify_all();
  }

private:
  // conditional variable
  std::condition_variable cond_var_;

  // mutex for the conditional variable
  std::mutex mutex_lock_;

  // flag to terminate the wait
  bool exit_request_;

  // flag to stop the waiting
  bool start_running_;
};

}  // namespace sync_timer
}  // namespace libUtility

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_SYNC_TIMER_H