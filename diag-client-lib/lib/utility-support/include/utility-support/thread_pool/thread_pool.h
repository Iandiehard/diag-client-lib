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
#include <future>
#include <mutex>
#include <string_view>
#include <vector>

#include "utility-support/protected_queue.h"
#include "utility-support/thread.h"
#include "utility-support/thread_pool/task_wrapper.h"

namespace utility_support {
namespace thread_pool {

/**
 * @brief  Thread pool implementation
 */
class ThreadPool final {
 public:
  explicit ThreadPool(std::string_view worker_thread_name_prefix,
                      std::uint32_t num_of_worker_threads) noexcept;

  ~ThreadPool() noexcept;

  void Initialize() noexcept;

  void Shutdown() noexcept;
  /*
   * @brief  Function to submit task for processing
   */
  template<typename FunctionType,
           typename ResultType = typename std::result_of_t<FunctionType()>::type>
  auto SubmitTask(FunctionType&& task) noexcept -> std::future<ResultType>;

 private:
  void Run() noexcept;

 private:
  /**
   * @brief  Flag to terminate the thread
   */
  std::atomic_bool exit_request_;

  /**
   * @brief  Conditional variable to block the thread
   */
  std::condition_variable cond_var_;

  std::mutex mutex_;

  /**
   * @brief  Storage for all submitted tasks
   */
  ProtectedQueue<TaskWrapper> task_queue_;

  /**
   * @brief  Storage for all worker threads in the pool
   */
  std::vector<thread::Thread> threads_;

  std::string thread_name_prefix_;

  std::uint32_t num_of_worker_threads_;
};

}  // namespace thread_pool
}  // namespace utility_support

#endif  // UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_THREAD_POOL_H_
