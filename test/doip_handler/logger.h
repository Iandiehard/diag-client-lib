/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAG_CLIENT_LOGGER_H
#define DIAG_CLIENT_LOGGER_H

#include "utility/logger.h"

namespace ara {
namespace diag {
namespace doip {
namespace logger {
using Logger = libUtility::logger::Logger;

class LibGtestLogger {
public:
  auto static GetLibGtestLogger() noexcept -> LibGtestLogger& {
    static LibGtestLogger lib_boost_logger_;
    return lib_boost_logger_;
  }

  auto GetLogger() noexcept -> Logger& { return logger_; }

private:
  LibGtestLogger() = default;

  // actual logger context
  Logger logger_{"CTST", "gtst"};
};
}  // logger
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // DIAG_CLIENT_LOGGER_H
