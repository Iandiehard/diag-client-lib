/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef TEST_COMPONENT_COMMON_LOGGER_H_
#define TEST_COMPONENT_COMMON_LOGGER_H_

#include "utility/logger.h"

namespace test {
namespace component {
namespace common {

class ComponentTestLogger {
 public:
  using Logger = utility::logger::Logger;

  auto static GetComponentTestLogger() noexcept -> ComponentTestLogger& {
    static ComponentTestLogger logger{};
    return logger;
  }

  auto GetLogger() noexcept -> Logger& { return logger_; }

 private:
  ComponentTestLogger() = default;

  // actual logger context
  Logger logger_{"CTST", "ctst"};
};
}  // namespace common
}  // namespace component
}  // namespace test
#endif  // TEST_COMPONENT_COMMON_LOGGER_H_
