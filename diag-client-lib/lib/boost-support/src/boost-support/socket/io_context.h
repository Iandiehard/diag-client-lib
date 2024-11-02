/* Diagnostic Client library
* Copyright (C) 2024  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_IO_CONTEXT_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_IO_CONTEXT_H_

#include <boost/asio.hpp>
#include <string_view>

#include "utility/thread.h"

namespace boost_support {
namespace socket {

/**
* @brief       Wrapper class to hold boost io context required for io object( sockets)
*/
class IoContext final {
 public:
  /**
  * @brief  Type alias for boost context
  */
  using Context = boost::asio::io_context;

 public:
  /**
  * @brief         Default constructs an instance of IoContext
  */
  IoContext() noexcept;

  /**
  * @brief         Constructs an instance of IoContext
  */
  IoContext(std::string_view context_name) noexcept;

  /**
  * @brief  Deleted copy assignment and copy constructor
  */
  IoContext(const IoContext &other) noexcept = delete;
  IoContext &operator=(const IoContext &other) noexcept = delete;

  /**
  * @brief  Deleted move assignment and move constructor
  */
  IoContext(IoContext &&other) noexcept = delete;
  IoContext &operator=(IoContext &&other) noexcept = delete;

  /**
  * @brief         Destruct an instance of IoContext
  */
  ~IoContext() noexcept;

  /**
   * @brief         Initialize the context
   */
  void Initialize() noexcept;

  /**
   * @brief         De-initialize the context
   */
  void DeInitialize() noexcept;

  /**
  * @brief         Function to get the io context reference
  * @return        The reference to io context
  */
  Context &GetContext() noexcept;

 private:
  /**
  * @brief  boost io context
  */
  Context io_context_;

  /**
   * @brief  Flag to terminate the thread
   */
  std::atomic_bool exit_request_;

  /**
   * @brief  Flag to start the thread
   */
  std::atomic_bool running_;

  /**
   * @brief  Conditional variable to block the thread
   */
  std::condition_variable cond_var_;

  /**
   * @brief  The name of context
   */
  std::string context_name_;

  /**
   * @brief  The thread itself
   */
  utility::thread::Thread thread_;

  /**
   * @brief  mutex to lock critical section
   */
  std::mutex mutex_;
};
}  // namespace socket
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_SOCKET_IO_CONTEXT_H_
