/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_SYNC_TIMER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_SYNC_TIMER_H

namespace utility {
namespace sync_timer {

#include <chrono>
#include <condition_variable>
#include <mutex>

/**
 * @brief       Timer class for timeout monitoring
 * @tparam      ClockType
 *              The type of clock to be used for time monitoring
 */
template<typename ClockType>
class SyncTimer final {
 public:
  /**
   * @brief  Type alias for the clock type
   */
  using Clock = ClockType;

  /**
   * @brief  Type alias for the clock time point
   */
  using TimePoint = std::chrono::time_point<Clock>;

  /**
   * @brief  Definition of different timer state during timeout monitoring
   */
  enum class TimerState : std::uint8_t { kIdle = 0, kCancelRequested, kTimeout };

  /**
   * @brief       Construct an instance of SyncTimer
   */
  SyncTimer() : cond_var_{}, mutex_lock_{}, exit_request_{false}, start_running_{false} {}

  /**
   * @brief       Destruct an instance of SyncTimer
   */
  ~SyncTimer() {
    std::lock_guard<std::mutex> const lck(mutex_lock_);
    exit_request_ = false;
    start_running_ = false;
    cond_var_.notify_all();
  }

  /**
   * @brief       Helper function to wait for response with timeout monitoring
   * @tparam      TimeoutCallback
   *              The callback functor type for timeout notification
   * @tparam      CancelCallback
   *              The callback functor type for cancellation notification
   * @param[in]   timeout_func
   *              The functor to be called when timeout occurs
   * @param[in]   cancel_func
   *              The functor to be called when expected event occurs within timeout
   * @param[in]   timeout
   *              The timeout in milliseconds
   */
  template<typename TimeoutCallback, typename CancelCallback>
  void WaitForTimeout(TimeoutCallback &&timeout_func, CancelCallback &&cancellation_func,
                      std::chrono::milliseconds const timeout) {
    if (Start(timeout) == TimerState::kTimeout) {
      timeout_func();
    } else {
      cancellation_func();
    }
  }

  /**
   * @brief       Function to cancel the synchronous wait
   */
  void CancelWait() { Stop(); }

 private:
  /**
   * @brief       Function to start the timeout monitoring
   * @param[in]   timeout
   *              The timeout value in milliseconds after which timeout happens
   * @return      TimerState
   *              "kTimeout" in case of timeout or "kCancelRequested" when timeout monitoring was cancelled
   */
  auto Start(std::chrono::milliseconds const timeout) noexcept -> TimerState {
    TimerState timer_state{TimerState::kIdle};
    std::unique_lock<std::mutex> lck(mutex_lock_);
    start_running_ = true;
    TimePoint const expiry_time_point{Clock::now() + timeout};
    if (cond_var_.wait_until(lck, expiry_time_point, [this, &expiry_time_point, &timer_state]() {
          bool do_exit_wait{false};
          // check if exit was requested
          if (!exit_request_) {
            TimePoint const current_time{Clock::now()};
            // check for expiry
            if (current_time >= expiry_time_point) {
              // timeout
              timer_state = TimerState::kTimeout;
              do_exit_wait = true;
            } else {
              // check for cancellation request
              if (!start_running_) {
                timer_state = TimerState::kCancelRequested;
                do_exit_wait = true;
              }  // else - spurious wake-up, do nothing
            }
          } else {
            do_exit_wait = true;
          }
          return do_exit_wait;
        })) {}
    return timer_state;
  }

  /**
   * @brief       Function to stop the current timeout monitoring
   */
  void Stop() noexcept {
    std::lock_guard<std::mutex> const lck(mutex_lock_);
    start_running_ = false;
    cond_var_.notify_all();
  }

 private:
  /**
   * @brief       The conditional variable needed for synchronizing between start and stop of running timer
   */
  std::condition_variable cond_var_;

  /**
   * @brief       The mutex for the conditional variable
   */
  std::mutex mutex_lock_;

  /**
   * @brief       The flag to terminate the wait
   */
  bool exit_request_;

  /**
   * @brief       The flag to stop the current running timer
   */
  bool start_running_;
};

}  // namespace sync_timer
}  // namespace utility

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_SYNC_TIMER_H