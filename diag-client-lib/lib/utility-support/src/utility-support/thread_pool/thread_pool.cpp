/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_TASK_H_
#define UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_TASK_H_

#include "utility-support/thread_pool/thread_pool.h"

#include <optional>

#include "utility-support/thread_pool/task_wrapper.h"

namespace utility_support {
namespace thread_pool {

ThreadPool::ThreadPool() noexcept
    : exit_request_{false},
      cond_var_{},
      mutex_{},
      task_queue_{},
      threads_{} {}

ThreadPool::~ThreadPool() noexcept { Shutdown(); }

void ThreadPool::Initialize(std::string_view const worker_thread_name_prefix,
                            std::uint32_t const num_of_worker_threads) noexcept {
  threads_.reserve(num_of_worker_threads);
  for (std::uint32_t thread_count = 0u; thread_count < num_of_worker_threads; thread_count++) {
    std::string thread_name{worker_thread_name_prefix};
    thread_name.append("_");
    thread_name.append(std::to_string(thread_count));
    threads_.emplace_back(thread::Thread(thread_name, [this]() noexcept { this->Run(); }));
  }
}

void ThreadPool::Shutdown() noexcept {
  if (!exit_request_) {
    {
      std::lock_guard lck(mutex_);
      exit_request_ = true;
    }
    // Join all thread
    for (thread::Thread& thread: threads_) { thread.Join(); }
  }
}

template<typename FunctionType, typename ResultType>
auto ThreadPool::SubmitTask(FunctionType&& task) noexcept -> std::future<ResultType> {
  std::packaged_task<ResultType()> packaged_task{std::forward<FunctionType>(task)};
  std::future<ResultType> result{packaged_task.get_future()};
  task_queue_.Push(std::move(packaged_task));
  cond_var_.notify_all();
  return result;
}

void ThreadPool::Run() noexcept {
  while (!exit_request_) {
    std::optional<TaskWrapper> task{};
    {
      std::unique_lock lck(mutex_);
      cond_var_.wait(lck, [this]() { return !task_queue_.IsEmpty() || exit_request_; });
      if (!exit_request_) { task = std::move(task_queue_.TryPop()); }
    }
    if (task.has_value()) { task->operator()(); }
  }
}

}  // namespace thread_pool
}  // namespace utility_support

#endif  // UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_TASK_H_
