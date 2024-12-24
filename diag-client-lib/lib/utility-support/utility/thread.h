/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_THREAD_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_THREAD_H

#include <pthread.h>

#include <string>
#include <thread>

namespace utility {
namespace thread {

class Thread final {
 public:
  /**
   * @brief     Default constructor
   */
  Thread() noexcept = default;

  /**
   * @tparam    Callable
   *            The callable that is invoked by created thread
   * @tparam    Args
   *            The arguments passed to the callable
   * @brief     Constructor to start the thread with passed callable and arguments
   *
   */
  template<typename Callable, typename... Args>
  Thread(std::string const &thread_name, Callable &&callable, Args &&...args) noexcept
      : thread_{std::forward<Callable>(callable), std::forward<Args>(args)...} {
    // Set thread name
    pthread_setname_np((pthread_t)thread_.native_handle(), thread_name.c_str());
  }

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  Thread(const Thread &other) noexcept = delete;
  Thread &operator=(const Thread &other) noexcept = delete;

  /**
   * @brief         Defaulted move assignment and move constructor
   */
  Thread(Thread &&other) noexcept = default;
  Thread &operator=(Thread &&other) noexcept = default;

  /**
   * @brief         Destructor
   */
  ~Thread() noexcept { Join(); }

  /**
   * @brief         Function to join the running thread
   */
  void Join() noexcept {
    if (thread_.joinable()) { thread_.join(); }
  }

 private:
  std::thread thread_{};
};

}  // namespace thread
}  // namespace utility
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_THREAD_H
