/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "src/common/diagnostic_manager.h"

namespace diag {
namespace client {
namespace common {

DiagnosticManager::DiagnosticManager() noexcept : exit_requested_{false}, cond_var_{}, mutex_{} {}

DiagnosticManager::~DiagnosticManager() noexcept {
  {
    std::lock_guard<std::mutex> const lock{mutex_};
    exit_requested_ = true;
  }
  cond_var_.notify_all();
}

void DiagnosticManager::Main() noexcept {
  // Initialize the module
  Initialize();
  // Run the module
  Run();
  // Entering infinite loop
  while (!exit_requested_) {
    std::unique_lock<std::mutex> lck(mutex_);
    cond_var_.wait(lck, [this]() { return exit_requested_; });
    // Thread exited
  }
  // Shutdown module
  Shutdown();
}

void DiagnosticManager::SignalShutdown() noexcept {
  {
    std::lock_guard<std::mutex> lock{mutex_};
    exit_requested_ = true;
  }
  cond_var_.notify_all();
}
}  // namespace common
}  // namespace client
}  // namespace diag
