/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_LOGGER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_LOGGER_H
#include <dlt/dlt.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace libUtility {
namespace logger {
class Logger {
public:
  template<typename Func>
  auto LogFatal(
    const std::string& file_name,
    int line_no,
    const std::string& func_name,
    Func func) noexcept -> void {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ": " << line_no << "]";
    DLT_LOG(contxt, DLT_LOG_FATAL,
            DLT_CSTRING(msg.str().c_str()));
    std::cout << "[FATAL]:   " << msg.str() << std::endl;
  }

  template<typename Func>
  auto LogError(
    const std::string& file_name,
    int line_no,
    const std::string& func_name,
    Func func) noexcept -> void {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ": " << line_no << "]";
    DLT_LOG(contxt, DLT_LOG_ERROR,
            DLT_CSTRING(msg.str().c_str()));
    std::cout << "[ERROR]:   " << msg.str() << std::endl;
  }

  template<typename Func>
  auto LogWarn(
    const std::string& file_name,
    int line_no,
    const std::string& func_name,
    Func func) noexcept -> void {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ": " << line_no << "]";
    DLT_LOG(contxt, DLT_LOG_WARN,
            DLT_CSTRING(msg.str().c_str()));
    std::cout << "[WARN]:    " << msg.str() << std::endl;
  }

  template<typename Func>
  auto LogInfo(
    const std::string& file_name,
    int line_no,
    const std::string& func_name,
    Func func) noexcept -> void {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ": " << line_no << "]";
    DLT_LOG(contxt, DLT_LOG_INFO,
            DLT_CSTRING(msg.str().c_str()));
    std::cout << "[INFO]:    " << msg.str() << std::endl;
  }

  template<typename Func>
  auto LogDebug(
    const std::string& file_name,
    int line_no,
    const std::string& func_name,
    Func func) noexcept -> void {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ": " << line_no << "]";
    DLT_LOG(contxt, DLT_LOG_DEBUG,
            DLT_CSTRING(msg.str().c_str()));
    std::cout << "[DEBUG]:   " << msg.str() << std::endl;
  }

  template<typename Func>
  auto LogVerbose(
    const std::string& file_name,
    int line_no,
    const std::string& func_name,
    Func func) noexcept -> void {
    std::stringstream msg;
    func(msg);
    msg << " [" << file_name << ": " << line_no << "]";
    DLT_LOG(contxt, DLT_LOG_VERBOSE,
            DLT_CSTRING(msg.str().c_str()));
    std::cout << "[VERBOSE]: " << msg.str() << std::endl;
  }

public:
  explicit Logger(const std::string& context_id) {
    DLT_REGISTER_CONTEXT(contxt, context_id.c_str(), "Application Context");
  }

  Logger(const std::string& app_id, const std::string& context_id) {
    DLT_REGISTER_APP(app_id.c_str(), "Application Id");
    DLT_REGISTER_CONTEXT(contxt, context_id.c_str(), "Application Context");
  }

  ~Logger() {
    DLT_UNREGISTER_CONTEXT(contxt);
    DLT_UNREGISTER_APP();
  }

private:
  // Declare the context
  DLT_DECLARE_CONTEXT(contxt);
};
}  // namespace logger
}  // namespace libUtility
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_LIBUTILITY_UTILITY_LOGGER_H