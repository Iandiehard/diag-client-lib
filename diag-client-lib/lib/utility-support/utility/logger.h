/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
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

#define UNUSED_PARAM(expr) \
  do { (void) (expr); } while (0)

namespace utility {
namespace logger {

/**
 * @brief       Logger class that is used to log Dlt messages from the component
 * @details     This class uses COVESA DLT infrastructure to send message to DLT. Also the class does not log dlt message
 *              if "ENABLE_DLT_LOGGER" cmake flag is set to OFF
 */
class Logger final {
 public:
  /**
   * @brief       Log fatal message and abort
   * @tparam      Func
   *              The functor type invoked on log level set to fatal
   * @param[in]   file_name
   *              The file name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func_name
   *              The function name
   * @param[in]   func
   *              The functor which gets invoked on log level set to fatal
   */
  template<typename Func>
  auto LogFatal(const std::string_view file_name, int line_no, const std::string_view func_name,
                Func &&func) noexcept -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_FATAL, file_name, func_name, line_no, std::forward<Func>(func));
#else
    UNUSED_PARAM(file_name);
    UNUSED_PARAM(line_no);
    UNUSED_PARAM(func_name);
    UNUSED_PARAM(func);
#endif
    std::abort();  // abort in case of fatal issue
  }

  /**
   * @brief       Log error message
   * @tparam      Func
   *              The functor type invoked on log level set to error
   * @param[in]   file_name
   *              The file name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func_name
   *              The function name
   * @param[in]   func
   *              The functor which gets invoked on log level set to error
   */
  template<typename Func>
  auto LogError(const std::string_view file_name, int line_no, const std::string_view func_name,
                Func &&func) noexcept -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_ERROR, file_name, func_name, line_no, std::forward<Func>(func));
#else
    UNUSED_PARAM(file_name);
    UNUSED_PARAM(line_no);
    UNUSED_PARAM(func_name);
    UNUSED_PARAM(func);
#endif
  }

  /**
   * @brief       Log warning message
   * @tparam      Func
   *              The functor type invoked on log level set to warning
   * @param[in]   file_name
   *              The file name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func_name
   *              The function name
   * @param[in]   func
   *              The functor which gets invoked on log level set to warning
   */
  template<typename Func>
  auto LogWarn(const std::string_view file_name, int line_no, const std::string_view func_name,
               Func &&func) noexcept -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_WARN, file_name, func_name, line_no, std::forward<Func>(func));
#else
    UNUSED_PARAM(file_name);
    UNUSED_PARAM(line_no);
    UNUSED_PARAM(func_name);
    UNUSED_PARAM(func);
#endif
  }

  /**
   * @brief       Log info message
   * @tparam      Func
   *              The functor type invoked on log level set to info
   * @param[in]   file_name
   *              The file name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func_name
   *              The function name
   * @param[in]   func
   *              The functor which gets invoked on log level set to info
   */
  template<typename Func>
  auto LogInfo(const std::string_view file_name, int line_no, const std::string_view func_name,
               Func &&func) noexcept -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_INFO, file_name, func_name, line_no, std::forward<Func>(func));
#else
    UNUSED_PARAM(file_name);
    UNUSED_PARAM(line_no);
    UNUSED_PARAM(func_name);
    UNUSED_PARAM(func);
#endif
  }

  /**
   * @brief       Log debug message
   * @tparam      Func
   *              The functor type invoked on log level set to debug
   * @param[in]   file_name
   *              The file name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func_name
   *              The function name
   * @param[in]   func
   *              The functor which gets invoked on log level set to debug
   */
  template<typename Func>
  auto LogDebug(const std::string_view file_name, int line_no, const std::string_view func_name,
                Func &&func) noexcept -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_DEBUG, file_name, func_name, line_no, std::forward<Func>(func));
#else
    UNUSED_PARAM(file_name);
    UNUSED_PARAM(line_no);
    UNUSED_PARAM(func_name);
    UNUSED_PARAM(func);
#endif
  }

  /**
   * @brief       Log verbose message
   * @tparam      Func
   *              The functor type invoked on log level set to verbose
   * @param[in]   file_name
   *              The file name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func_name
   *              The function name
   * @param[in]   func
   *              The functor which gets invoked on log level set to verbose
   */
  template<typename Func>
  auto LogVerbose(const std::string_view file_name, int line_no, const std::string_view func_name,
                  Func &&func) noexcept -> void {
#ifdef ENABLE_DLT_LOGGER
    LogDltMessage(DLT_LOG_VERBOSE, file_name, func_name, line_no, std::forward<Func>(func));
#else
    UNUSED_PARAM(file_name);
    UNUSED_PARAM(line_no);
    UNUSED_PARAM(func_name);
    UNUSED_PARAM(func);
#endif
  }

 public:
  /**
   * @brief       Construct an instance of Logger
   * @param[in]   context_id
   *              The context id of the user
   */
  explicit Logger(std::string_view context_id);

  /**
   * @brief       Construct an instance of Logger
   * @param[in]   app_id
   *              The application id of the user
   * @param[in]   context_id
   *              The context id of the user
   */
  Logger(std::string_view app_id, std::string_view context_id);

  /**
   * @brief       Destruct an instance of Logger
   */
  ~Logger();

 private:
  /**
   * @brief       Function to create the final logging message
   * @tparam      Func
   *              The functor type
   * @param[in]   file_name
   *              The file name
   * @param[in]   func_name
   *              The function name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func
   *              The functor which gets invoked
   */
  template<typename Func>
  auto CreateLoggingMessage(const std::string_view file_name,
                            const std::string_view /* func_name */, int line_no,
                            Func &&func) noexcept -> std::stringstream {
    std::stringstream msg{};
    func(msg);
    msg << " [" << file_name << ":" << line_no << "]";
    return msg;
  }

  /**
   * @brief       Function to send the messages to dlt infrastructure
   * @tparam      log_level
   *              The log level
   * @param[in]   file_name
   *              The file name
   * @param[in]   func_name
   *              The function name
   * @param[in]   line_no
   *              The line number
   * @param[in]   func
   *              The functor which gets invoked
   */
#ifdef ENABLE_DLT_LOGGER
  template<typename Func>
  void LogDltMessage(DltLogLevelType log_level, const std::string_view file_name,
                     const std::string_view func_name, int line_no, Func &&func) {
    /*
    DLT_LOG(
        contxt_, log_level,
        DLT_CSTRING(CreateLoggingMessage(file_name, func_name, line_no, std::forward<Func>(func))
                        .str()
                        .c_str())); */
    std::cout << CreateLoggingMessage(file_name, func_name, line_no, std::forward<Func>(func)).str()
              << std::endl;
  }
#endif

#ifdef ENABLE_DLT_LOGGER
  // Declare the context
  DLT_DECLARE_CONTEXT(contxt_)
#else
  std::string contxt_;
#endif
  // Stores application id
  std::string app_id_;

  // Stores context id
  std::string context_id_;

  // store the information about registration with app id
  bool registration_with_app_id_{};
};
}  // namespace logger
}  // namespace utility
#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_UTILITY_UTILITY_LOGGER_H