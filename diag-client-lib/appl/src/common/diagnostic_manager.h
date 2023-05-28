/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_DIAGNOSTIC_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_DIAGNOSTIC_MANAGER_H
/* includes */
#include <condition_variable>
#include <mutex>

#include "core/result.h"
#include "include/diagnostic_client.h"
#include "include/diagnostic_client_uds_message_type.h"
#include "include/diagnostic_client_vehicle_info_message_type.h"

namespace diag {
namespace client {
// forward declaration
namespace conversation {
class DiagClientConversation;
}

namespace common {

/**
 * @brief    Parent class to create Diagnostic Manager
 */
class DiagnosticManager {
public:
  /**
   * @brief         Constructs an instance of DiagnosticManager
   */
  DiagnosticManager() noexcept;

  /**
   * @brief         Deleted copy assignment and copy constructor
   */
  DiagnosticManager(const DiagnosticManager &other) noexcept = delete;
  DiagnosticManager &operator=(const DiagnosticManager &other) noexcept = delete;

  /**
   * @brief         Deleted move assignment and move constructor
   */
  DiagnosticManager(DiagnosticManager &&other) noexcept = delete;
  DiagnosticManager &operator=(DiagnosticManager &&other) noexcept = delete;

  /**
   * @brief         Destructs an instance of DiagnosticManager
   */
  virtual ~DiagnosticManager() noexcept;

  /**
   * @brief         Function to manage the whole lifecycle of DiagnosticManager
   */
  virtual void Main() noexcept;

  /**
   * @brief         Function to initiate shutdown of DiagnosticManager
   */
  virtual void SignalShutdown() noexcept;

  /**
   * @brief         Function to initialize the DiagnosticManager
   */
  virtual void Initialize() noexcept = 0;

  /**
   * @brief         Function to run DiagnosticManager
   */
  virtual void Run() noexcept = 0;

  /**
   * @brief         Function to shutdown the DiagnosticManager
   */
  virtual void Shutdown() noexcept = 0;

  /**
   * @brief       Function to get required diag client conversation object based on conversation name
   * @param[in]   conversation_name
   *              Name of conversation configured as json parameter "ConversationName"
   * @return      Diag client conversation object as per passed conversation name
   * @implements  DiagClientLib-MultipleTester-Connection, DiagClientLib-Conversation-Construction
   */
  virtual conversation::DiagClientConversation GetDiagnosticClientConversation(
      std::string_view conversation_name) noexcept = 0;

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   * @implements  DiagClientLib-VehicleDiscovery
   */
  virtual core_type::Result<diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr,
                            DiagClient::VehicleInfoResponseErrorCode>
  SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept = 0;

private:
  /**
   * @brief         Flag to terminate the main thread
   */
  bool exit_requested_;

  /**
   * @brief         Conditional variable to block the thread
   */
  std::condition_variable cond_var_;

  /**
   * @brief         For locking critical section of code
   */
  std::mutex mutex_;
};
}  // namespace common
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_COMMON_DIAGNOSTIC_MANAGER_H
