/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/socket/io_context.h"

namespace boost_support {
namespace socket {

IoContext::IoContext() noexcept
    : io_context_{},
      exit_request_{false},
      running_{false},
      cond_var_{},
      mutex_{} {}

IoContext::IoContext(std::string_view context_name) noexcept
    : io_context_{},
      exit_request_{false},
      running_{false},
      cond_var_{},
      context_name_{context_name},
      thread_{},
      mutex_{} {
  // start thread to execute async tasks
  thread_ = utility_support::thread::Thread(context_name_, [this]() {
    std::unique_lock<std::mutex> lck(mutex_);
    while (!exit_request_) {
      if (!running_) {
        cond_var_.wait(lck, [this]() { return exit_request_ || running_; });
      }
      if (!exit_request_) {
        if (running_) {
          lck.unlock();
          io_context_.restart();
          io_context_.run();
          lck.lock();
        }
      }
    }
  });
}

IoContext::~IoContext() noexcept {
  exit_request_ = true;
  running_ = false;
  cond_var_.notify_all();
  thread_.Join();
}

void IoContext::Initialize() noexcept {
  std::lock_guard<std::mutex> lock{mutex_};
  running_ = true;
  cond_var_.notify_all();
}

void IoContext::DeInitialize() noexcept {
  std::lock_guard<std::mutex> lock{mutex_};
  running_ = false;
  io_context_.stop();
  cond_var_.notify_all();
}

IoContext::Context &IoContext::GetContext() noexcept { return io_context_; }

}  // namespace socket
}  // namespace boost_support
