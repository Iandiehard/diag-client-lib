/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "include/diagnostic_client.h"

#include <pthread.h>

#include <memory>
#include <string>
#include <thread>

#include "core/include/result.h"
#include "parser/json_parser.h"
#include "src/common/diagnostic_manager.h"
#include "src/common/logger.h"
#include "src/dcm/dcm_client.h"
#include "src/dcm/error_domain/dm_error_domain.h"

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
        __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient Initialization started"; });

    // read configuration
    boost_support::parser::boost_tree config{};
    return boost_support::parser::Read(diag_client_config_path_, config)
        .MapError([](boost_support::parser::ParsingErrorCode) noexcept {
          logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
              __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "DiagClient Initialization failed"; });
          return error_domain::MakeErrorCode(error_domain::DmErrorErrc::kInitializationFailed);
        })
        .AndThen([this, &config]() {
          // create single dcm instance and pass the configuration
          dcm_instance_ = std::make_unique<diag::client::dcm::DCMClient>(config_parser::ReadDcmClientConfig(config));
          // start dcm client thread
          dcm_thread_ = std::thread([this]() noexcept { this->dcm_instance_->Main(); });
          pthread_setname_np(dcm_thread_.native_handle(), "DCMClient_Main");
          logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
              __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "DiagClient Initialization completed"; });
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
        __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient De-Initialization started"; });
    // shutdown DCM module here
    return dcm_instance_->SignalShutdown()
        .AndThen([this]() {
          if (dcm_thread_.joinable()) { dcm_thread_.join(); }
          logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
              __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "DiagClient De-Initialization completed"; });
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
  conversation::DiagClientConversation GetDiagnosticClientConversation(std::string_view conversation_name) noexcept {
    if (!dcm_instance_) {
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatal(
          __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "DiagClient is not Initialized"; });
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
      logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogFatal(
          __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "DiagClient is not Initialized"; });
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
  std::thread dcm_thread_;

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
