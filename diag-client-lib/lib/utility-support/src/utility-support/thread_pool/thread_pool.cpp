/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_TASK_H_
#define UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_TASK_H_

namespace utility_support {
namespace thread_pool {

class ThreadTask final {
 public:
  explicit ThreadTask(Callable&& callable) noexcept : callable_{std::forward<Callable>(callable)} {}

  void operator()() const noexcept { callable_(); }

 private:
  Callable callable_;
};

}  // namespace thread_pool
}  // namespace utility_support

#endif  // UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_TASK_H_
