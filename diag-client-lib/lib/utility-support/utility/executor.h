/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_EXECUTOR_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_EXECUTOR_H

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace utility {
namespace executor {

template<typename ExecutorHandler>
class Executor {
public:
  // ctor
  Executor() : exit_request_{false}, running_{false} {
    thread_ = std::thread([&]() {
      std::unique_lock<std::mutex> exit_lck(exit_mutex_lock_);
      while (!exit_request_.load()) {
        if (!running_.load()) { cond_var_.wait(exit_lck); }
        if (running_) {
          std::unique_lock<std::mutex> lck(mutex_lock_);
          while (!queue_.empty()) {
            auto func = queue_.front();
            lck.unlock();
            func();
            lck.lock();
            queue_.pop();
          }
          running_.store(false);
        }
      }
    });
  }

  // dtor
  ~Executor() {
    exit_request_ = true;
    running_ = false;
    cond_var_.notify_one();
    thread_.join();
  }

  // function to add job to executor
  void AddExecute(ExecutorHandler executor_handler) {
    std::unique_lock<std::mutex> lck(mutex_lock_);
    queue_.push(std::move(executor_handler));
    running_.store(true);
    cond_var_.notify_one();
  }

private:
  // queue to store the elements
  std::queue<ExecutorHandler> queue_;
  // mutex to lock the critical section
  std::mutex mutex_lock_;
  // mutex to lock the critical section
  std::mutex exit_mutex_lock_;
  // threading var
  std::thread thread_;
  // conditional variable to block the thread
  std::condition_variable cond_var_;
  // flag to terminate the thread
  std::atomic<bool> exit_request_;
  // flag th start the thread
  std::atomic<bool> running_;
};
}  // namespace executor
}  // namespace utility
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_EXECUTOR_H
