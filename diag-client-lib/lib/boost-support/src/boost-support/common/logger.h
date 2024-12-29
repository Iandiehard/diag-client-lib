/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_COMMON_LOGGER_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_COMMON_LOGGER_H_

#include "utility-support/logger/logger.h"

namespace boost_support {
namespace common {
namespace logger {
using Logger = utility_support::logger::Logger;

class LibBoostLogger final {
 public:
  auto static GetLibBoostLogger() noexcept -> LibBoostLogger& {
    static LibBoostLogger boost_logger{};
    return boost_logger;
  }

  auto GetLogger() noexcept -> Logger& { return logger_; }

 private:
  LibBoostLogger() = default;

  // actual logger context
  Logger logger_{"dcbs"};
};
}  // namespace logger
}  // namespace common
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_COMMON_LOGGER_H_
