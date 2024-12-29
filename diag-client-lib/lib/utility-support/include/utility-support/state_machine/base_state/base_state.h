/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef UTILITY_SUPPORT_STATE_MACHINE_BASE_STATE_BASE_STATE_H_
#define UTILITY_SUPPORT_STATE_MACHINE_BASE_STATE_BASE_STATE_H_

namespace utility_support {
namespace state_machine {
namespace base_state {

/**
 * @brief  Base class for handling different state
 */
template<typename StateHandle>
class BaseState {
 public:
  /**
  * @brief  Construct the instance of base state
  */
  explicit BaseState(StateHandle state) : state_{state} {}

  /**
  * @brief  Deleted copy assignment and copy constructor
  */
  BaseState(const BaseState &other) noexcept = delete;
  BaseState &operator=(const BaseState &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  BaseState(BaseState &&other) noexcept = delete;
  BaseState &operator=(BaseState &&other) noexcept = delete;

  /**
   * @brief  Destruct the instance of base state
   */
  virtual ~BaseState() noexcept = default;

  /**
   * @brief     Function that is called when entering the state
   */
  virtual void OnEnter() noexcept = 0;

  /**
   * @brief     Function that is called when exiting the state
   */
  virtual void OnExit() noexcept = 0;

  /**
   * @brief     Get the current state
   * @return    The current stored state
   */
  auto GetState() const noexcept -> StateHandle { return state_; }

 private:
  /**
   * @brief     Store the current state handle
   */
  StateHandle state_;
};

}  // namespace base_state
}  // namespace state_machine
}  // namespace utility_support
#endif  // UTILITY_SUPPORT_STATE_MACHINE_BASE_STATE_BASE_STATE_H_
