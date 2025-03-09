/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_POOL_H_
#define UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_POOL_H_

#include <cstdint>
#include <string_view>
#include <thread>
#include <vector>

#include "utility-support/thread_pool/thread_task.h"

namespace utility_support {
namespace thread_pool {

class ThreadPool final {
 public:
  explicit ThreadPool(std::string_view thread_name, std::uint32_t num_threads) noexcept;

  void SubmitTask(ThreadTask&& task) noexcept;

 private:
  std::vector<std::thread> threads_;
};

}  // namespace thread_pool
}  // namespace utility_support

#endif  // UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_POOL_H_
