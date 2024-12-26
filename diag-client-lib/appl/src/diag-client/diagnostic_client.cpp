/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "diag-client/diagnostic_client.h"

#include <pthread.h>

#include <memory>
#include <string>

#include "boost-support/parser/json_parser.h"
#include "core/include/result.h"
#include "diag-client/common/diagnostic_manager.h"
#include "diag-client/common/logger.h"
#include "diag-client/dcm/dcm_client.h"
#include "diag-client/dcm/error_domain/dm_error_domain.h"
#include "utility/thread.h"

namespace diag {
namespace client {

/**
 * @brief    Class to provide implementation of diag client 
 */
class DiagClient::DiagClientImpl final {
 public:
  /**
   * @brief         Constructs an instance of DiagClient
   * @param[in]     diag_client_config_path
   *                The path to diag client config file
   * @implements    DiagClientLib-Construction
   */
  explicit DiagClientImpl(std::string_view diag_client_config_path) noexcept
      : dcm_instance_{},
        dcm_thread_{},
        diag_client_config_path_{diag_client_config_path} {}

  /**
   * @brief  Deleted copy assignment and copy constructor
   */
  DiagClientImpl(const DiagClientImpl &other) noexcept = delete;
  DiagClientImpl &operator=(const DiagClientImpl &other) noexcept = delete;

  /**
   * @brief  Deleted move assignment and move constructor
   */
  DiagClientImpl(DiagClientImpl &&other) noexcept = delete;
  DiagClientImpl &operator=(DiagClientImpl &&other) noexcept = delete;

  /**
   * @brief         Destruct an instance of DiagClient
   * @implements    DiagClientLib-Destruction
   */
  ~DiagClientImpl() noexcept = default;

  /**
   * @brief        Function to initialize the already created instance of DiagClient
   * @details      Must be called once and before using any other functions of DiagClient
   * @return       Result with void in case of success, otherwise error is returned
   * @implements   DiagClientLib-Initialization
   */
  Result<void> Initialize() noexcept {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
        FILE_NAME, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "DiagClient Initialization started"; });

    // read configuration
    return boost_support::parser::Read(diag_client_config_path_)
        .AndThen([this](boost_support::parser::boost_tree config) {
          // Create single dcm instance and pass the configuration
          dcm_instance_ = std::make_unique<diag::client::dcm::DCMClient>(
              config_parser::ReadDcmClientConfig(config));
          // Start dcm client main thread
          dcm_thread_ = utility::thread::Thread{"DcmClientMain",
                                                [this]() noexcept { dcm_instance_->Main(); }};
          logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
              FILE_NAME, __LINE__, "",
              [](std::stringstream &msg) { msg << "DiagClient Initialization completed"; });
          return Result<void, boost_support::parser::ParsingErrorCode>::FromValue();
        })
        .MapError([](boost_support::parser::ParsingErrorCode const &) noexcept {
          logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
              FILE_NAME, __LINE__, "",
              [](std::stringstream &msg) { msg << "DiagClient Initialization failed"; });
          return error_domain::MakeErrorCode(error_domain::DmErrorErrc::kInitializationFailed);
        });
  }

  /**
   * @brief        Function to de-initialize the already initialized instance of DiagClient
   * @details      Must be called during shutdown phase, no further processing of any
   *               function will be allowed after this call
   * @return       Result with void in case of success, otherwise error is returned
   * @implements   DiagClientLib-DeInitialization
   */
  Result<void> DeInitialize() noexcept {
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
        FILE_NAME, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "DiagClient De-Initialization started"; });
    // shutdown DCM module here
    return dcm_instance_->SignalShutdown()
        .AndThen([this]() {
          dcm_thread_.Join();
          logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
              FILE_NAME, __LINE__, "",
              [](std::stringstream &msg) { msg << "DiagClient De-Initialization completed"; });
        })
        .OrElse([](core_type::ErrorCode const &) {
          return error_domain::MakeErrorCode(error_domain::DmErrorErrc::kDeInitializationFailed);
        });
  }

  /**
   * @brief       Function to get required diag client conversation object based on conversation name
   * @param[in]   conversation_name
   *              Name of conversation configured as json parameter "ConversationName"
   * @return      Diag client conversation object as per passed conversation name
   * @implements  DiagClientLib-MultipleTester-Connection, DiagClientLib-Conversation-Construction
   */
  conversation::DiagClientConversation GetDiagnosticClientConversation(
      std::string_view conversation_name) noexcept {
    if (!dcm_instance_) {
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatalAndTerminate(
          FILE_NAME, __LINE__, "",
          [](std::stringstream &msg) { msg << "DiagClient is not Initialized"; });
    }
    return dcm_instance_->GetDiagnosticClientConversation(conversation_name);
  }

  /**
   * @brief       Function to send vehicle identification request and get the Diagnostic Server list
   * @param[in]   vehicle_info_request
   *              Vehicle information sent along with request
   * @return      Result containing available vehicle information response on success, VehicleResponseErrorCode on error
   * @implements  DiagClientLib-VehicleDiscovery
   */
  Result<vehicle_info::VehicleInfoMessageResponseUniquePtr, DiagClient::VehicleInfoResponseError>
  SendVehicleIdentificationRequest(
      diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept {
    if (!dcm_instance_) {
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatalAndTerminate(
          FILE_NAME, __LINE__, "",
          [](std::stringstream &msg) { msg << "DiagClient is not Initialized"; });
    }
    return dcm_instance_->SendVehicleIdentificationRequest(std::move(vehicle_info_request));
  }

 private:
  /**
   * @brief    Unique pointer to dcm client instance
   */
  std::unique_ptr<diag::client::common::DiagnosticManager> dcm_instance_;

  /**
   * @brief    Thread to handle dcm client lifecycle
   */
  utility::thread::Thread dcm_thread_;

  /**
   * @brief    Store the diag client config path
   */
  std::string diag_client_config_path_;
};

DiagClient::DiagClient(std::string_view diag_client_config_path) noexcept
    : diag_client_impl_{std::make_unique<DiagClientImpl>(diag_client_config_path)} {}

DiagClient::~DiagClient() noexcept = default;

Result<void> DiagClient::Initialize() noexcept { return diag_client_impl_->Initialize(); }

Result<void> DiagClient::DeInitialize() noexcept { return diag_client_impl_->DeInitialize(); }

Result<vehicle_info::VehicleInfoMessageResponseUniquePtr, DiagClient::VehicleInfoResponseError>
DiagClient::SendVehicleIdentificationRequest(
    diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) noexcept {
  return diag_client_impl_->SendVehicleIdentificationRequest(std::move(vehicle_info_request));
}

conversation::DiagClientConversation DiagClient::GetDiagnosticClientConversation(
    std::string_view conversation_name) noexcept {
  return diag_client_impl_->GetDiagnosticClientConversation(conversation_name);
}

std::unique_ptr<DiagClient> CreateDiagnosticClient(std::string_view diag_client_config_path) {
  return (std::make_unique<DiagClient>(diag_client_config_path));
}

}  // namespace client
}  // namespace diag
