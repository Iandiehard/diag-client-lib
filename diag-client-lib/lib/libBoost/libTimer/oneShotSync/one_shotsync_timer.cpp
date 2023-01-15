/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// includes
#include "one_shotsync_timer.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libTimer {
namespace oneShot {
//ctor
oneShotSyncTimer::oneShotSyncTimer()
  : timer_ptr_(std::make_unique<BoostTimer>(io_e)) {
  DLT_REGISTER_CONTEXT(oneshotsync_timer_ctx, "osyt", "Oneshot timer Context");
}

//dtor
oneShotSyncTimer::~oneShotSyncTimer() {
  DLT_UNREGISTER_CONTEXT(oneshotsync_timer_ctx);
}

// start the timer
auto oneShotSyncTimer::Start(int msec) noexcept -> timer_state {
  DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG,
          DLT_CSTRING("One shot Timer start requested"));
  timer_ptr_->expires_after(msTime(msec));
  // Register completion handler triggered from async_wait
  timer_ptr_->async_wait([&](const boost::system::error_code &error) {
    error_ = error;
    // timer stop was requested
    if (error_ == boost::asio::error::operation_aborted) { io_e.stop(); }
  });
  auto start = std::chrono::system_clock::now();
  // blocking io call
  io_e.restart();
  io_e.run();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG,
          DLT_CSTRING("Elapsed time: "),
          DLT_FLOAT64(elapsed_seconds.count()),
          DLT_CSTRING("seconds"));
  return (error_ !=
          boost::asio::error::operation_aborted
          ? timer_state::kTimeout
          : timer_state::kCancelRequested);
}

// stop the timer
auto oneShotSyncTimer::Stop() noexcept -> void {
  DLT_LOG(oneshotsync_timer_ctx, DLT_LOG_DEBUG,
          DLT_CSTRING("Oneshot Timer stop requested"));
  timer_ptr_->cancel();
}
}  // namespace oneShot
}  // namespace libTimer
}  // namespace libBoost
}  // namespace libOsAbstraction
