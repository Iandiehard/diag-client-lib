/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_LOGGER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_LOGGER_H

#include "utility/logger.h"

namespace doip_client {
namespace logger {

using Logger = utility::logger::Logger;

class DoipClientLogger {
public:
  auto static GetDiagClientLogger() noexcept -> DoipClientLogger& {
    static DoipClientLogger diag_client_logger_;
    return diag_client_logger_;
  }

  auto GetLogger() noexcept -> Logger& { return logger_; }

private:
  DoipClientLogger() = default;

  // actual logger context
  Logger logger_{"doip"};
};

}  // namespace logger
}  // namespace doip_client

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_COMMON_LOGGER_H
