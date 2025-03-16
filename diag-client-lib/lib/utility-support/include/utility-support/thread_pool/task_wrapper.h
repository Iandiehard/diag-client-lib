/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_TASK_WRAPPER_H_
#define UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_TASK_WRAPPER_H_

namespace utility_support {
namespace thread_pool {

class TaskWrapper final {
 public:
  TaskWrapper() noexcept = default;

  template<typename FunctionType>
  explicit TaskWrapper(FunctionType&& callable) noexcept
      : task_{std::make_unique<TaskImpl>(std::forward<FunctionType>(callable))} {}

  void operator()() const noexcept { task_->InvokeTask(); }

 private:
  class Task {
   public:
    virtual ~Task() = default;
    virtual void InvokeTask() const noexcept = 0;
  };

  std::unique_ptr<Task> task_;

  template<typename FunctionType>
  class TaskImpl final : Task {
   public:
    explicit TaskImpl(FunctionType&& callable) noexcept
        : callable_{std::forward<FunctionType>(callable)} {}

    void InvokeTask() const noexcept override { callable_(); }

   private:
    FunctionType callable_;
  };
};

}  // namespace thread_pool
}  // namespace utility_support

#endif  // UTILITY_SUPPORT_INCLUDE_UTILITY_SUPPORT_THREAD_POOL_TASK_WRAPPER_H_
