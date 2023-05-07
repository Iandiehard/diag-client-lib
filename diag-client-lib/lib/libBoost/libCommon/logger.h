/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBCOMMON_LOGGER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBCOMMON_LOGGER_H

#include "utility/logger.h"

namespace libBoost {
namespace logger {
using Logger = libUtility::logger::Logger;

class LibBoostLogger {
public:
  auto static GetLibBoostLogger() noexcept -> LibBoostLogger& {
    static LibBoostLogger lib_boost_logger_;
    return lib_boost_logger_;
  }

  auto GetLogger() noexcept -> Logger& { return logger_; }

private:
  LibBoostLogger() = default;

  // actual logger context
  Logger logger_{"dcbs"};
};
}  // namespace logger
}  // namespace libBoost
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBCOMMON_LOGGER_H
