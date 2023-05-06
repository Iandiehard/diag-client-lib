/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// includes
#include "one_shotsync_timer.h"

#include "libCommon/logger.h"

namespace libBoost {
namespace libTimer {
namespace oneShot {
//ctor
oneShotSyncTimer::oneShotSyncTimer() : timer_ptr_(std::make_unique<BoostTimer>(io_e)) {}

//dtor
oneShotSyncTimer::~oneShotSyncTimer() = default;

// start the timer
auto oneShotSyncTimer::Start(int timeout) noexcept -> timer_state {
  logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [timeout](std::stringstream &msg) {
        msg << "[OneShotSyncTimer] started with timeout: " << std::to_string(timeout) << " milliseconds";
      });
  timer_ptr_->expires_after(msTime(timeout));
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

  logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [elapsed_seconds](std::stringstream &msg) {
        msg << "[OneShotSyncTimer] Elapsed time: " << std::to_string(elapsed_seconds.count()) << " seconds";
      });
  return (error_ != boost::asio::error::operation_aborted ? timer_state::kTimeout : timer_state::kCancelRequested);
}

// stop the timer
auto oneShotSyncTimer::Stop() noexcept -> void {
  logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "[OneShotSyncTimer] stopped"; });
  timer_ptr_->cancel();
}

}  // namespace oneShot
}  // namespace libTimer
}  // namespace libBoost
