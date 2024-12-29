/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_LOGGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_LOGGER_H

#include "utility-support/logger/logger.h"

namespace diag {
namespace client {
namespace logger {

/**
 * @brief    Class to create a singleton logger for diag-client
 */
class DiagClientLogger final {
 public:
  /**
   * @brief    Type alias of logger
   */
  using Logger = utility_support::logger::Logger;

  /**
   * @brief       Get the diag client logger instance
   * @return      DiagClientLogger
   *              The singleton instance
   */
  auto static GetDiagClientLogger() noexcept -> DiagClientLogger& {
    static DiagClientLogger diag_client_logger_;
    return diag_client_logger_;
  }

  /**
   * @brief       Get the logger instance
   * @return      Logger
   *              The logger instance
   */
  auto GetLogger() noexcept -> Logger& { return logger_; }

 private:
  /**
   * @brief       Construct an instance of DiagClientLogger
   */
  DiagClientLogger() = default;

  /**
   * @brief       Store the logger instance with context id
   */
  Logger logger_{"dcap"};
};
}  // namespace logger
}  // namespace client
}  // namespace diag
#endif
