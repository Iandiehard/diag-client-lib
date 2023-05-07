/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_LOGGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_LOGGER_H

#include "utility/logger.h"

namespace diag {
namespace client {
namespace logger {
using Logger = libUtility::logger::Logger;

class DiagClientLogger {
public:
  auto static GetDiagClientLogger() noexcept -> DiagClientLogger& {
    static DiagClientLogger diag_client_logger_;
    return diag_client_logger_;
  }

  auto GetLogger() noexcept -> Logger& { return logger_; }

private:
  DiagClientLogger() = default;

  // actual logger context
  Logger logger_{"dcap"};
};
}  // namespace logger
}  // namespace client
}  // namespace diag
#endif
