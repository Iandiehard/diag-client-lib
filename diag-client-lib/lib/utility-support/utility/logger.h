/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_LOGGER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_LOGGER_H

#ifdef ENABLE_DLT_LOGGER
#include <dlt/dlt.h>
#endif
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace utility {
namespace logger {
class Logger {
public:
  template<typename Func>
  auto LogFatal(const std::string_view file_name, int line_no, const std::string_view func_name, Func &&func) noexcept
      -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_FATAL, file_name, func_name, line_no, std::forward<Func>(func));
#endif
  }

  template<typename Func>
  auto LogError(const std::string_view file_name, int line_no, const std::string_view func_name, Func &&func) noexcept
      -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_ERROR, file_name, func_name, line_no, std::forward<Func>(func));
#endif
  }

  template<typename Func>
  auto LogWarn(const std::string_view file_name, int line_no, const std::string_view func_name, Func &&func) noexcept
      -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_WARN, file_name, func_name, line_no, std::forward<Func>(func));
#endif
  }

  template<typename Func>
  auto LogInfo(const std::string_view file_name, int line_no, const std::string_view func_name, Func &&func) noexcept
      -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_INFO, file_name, func_name, line_no, std::forward<Func>(func));
#endif
  }

  template<typename Func>
  auto LogDebug(const std::string_view file_name, int line_no, const std::string_view func_name, Func &&func) noexcept
      -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_DEBUG, file_name, func_name, line_no, std::forward<Func>(func));
#endif
  }

  template<typename Func>
  auto LogVerbose(const std::string_view file_name, int line_no, const std::string_view func_name, Func &&func) noexcept
      -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_VERBOSE, file_name, func_name, line_no, std::forward<Func>(func));
#endif
  }

public:
  // ctor
  explicit Logger(std::string_view context_id);

  // ctor
  Logger(std::string_view app_id, std::string_view context_id);

  // dtor
  ~Logger();

private:
  template<typename Func>
  auto CreateLoggingMessage(const std::string_view file_name, const std::string_view /* func_name */, int line_no,
                            Func &&func) noexcept -> std::stringstream {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ":" << line_no << "]";
    return msg;
  }

#ifdef ENABLE_DLT_LOGGER
  template<typename Func>
  void LogDltMessage(DltLogLevelType log_level, const std::string_view file_name, const std::string_view func_name,
                     int line_no, Func &&func) {

    DLT_LOG(contxt_, log_level,
            DLT_CSTRING(CreateLoggingMessage(file_name, func_name, line_no, std::forward<Func>(func)).str().c_str()));
  }
#endif

#ifdef ENABLE_DLT_LOGGER
  // Declare the context
  DLT_DECLARE_CONTEXT(contxt_);
#endif
  // Stores application id
  std::string app_id_;

  // Stores context id
  std::string context_id_;

  // store the information about registration with app id
  bool registration_with_app_id_{false};
};
}  // namespace logger
}  // namespace utility
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_LOGGER_H