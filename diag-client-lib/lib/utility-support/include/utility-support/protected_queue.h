/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_INCLUDE_UTILITY_PROTECTED_QUEUE_H_
#define UTILITY_SUPPORT_INCLUDE_UTILITY_PROTECTED_QUEUE_H_

#include <deque>
#include <mutex>
#include <optional>

namespace utility_support {

template<typename T>
class ProtectedQueue {
 public:
  ProtectedQueue() = default;
  ~ProtectedQueue() = default;


  void Push(T value) noexcept {
    std::lock_guard const lock{mutex_};
    queue_.emplace_back(std::move(value));
  }

  auto TryPop() noexcept -> std::optional<T> {
    std::optional<T> value{};
    std::lock_guard const lock{mutex_};
    if (!queue_.empty()) {
      value.emplace(std::move(queue_.front()));
      queue_.pop_front();
    }
    return value;
  }

  bool IsEmpty() noexcept {
    bool is_empty{false};
    {
      std::lock_guard const lock{mutex_};
      is_empty = queue_.empty();
    }
    return is_empty;
  }

 private:
  std::mutex mutex_;
  std::deque<T> queue_;
};

}  // namespace utility_support

#endif  // UTILITY_SUPPORT_INCLUDE_UTILITY_PROTECTED_QUEUE_H_
