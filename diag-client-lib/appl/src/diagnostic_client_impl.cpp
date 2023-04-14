/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "src/diagnostic_client_impl.h"

#include <pthread.h>

#include <memory>
#include <string>

#include "common/logger.h"
#include "include/diagnostic_client.h"

namespace diag {
namespace client {
// ctor
DiagClientImpl::DiagClientImpl(std::string_view dm_client_config) : diag::client::DiagClient(), ptree{}, dcm_instance_ptr{} {
  // start parsing the config json file
  libOsAbstraction::libBoost::jsonparser::createJsonParser json_parser{};
  json_parser.GetJsonPtree(dm_client_config, ptree);

  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient instance creation started"; });
  // create single dcm instance and pass the config tree
  dcm_instance_ptr = std::make_unique<diag::client::dcm::DCMClient>(ptree);
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient instance creation completed"; });
}

// Initialize all the resources and load the configs
void DiagClientImpl::Initialize() {
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient Initialization started"; });
  // start DCM thread here
  dcm_thread_ = std::thread(&diag::client::dcm::DCMClient::Main, std::ref(*dcm_instance_ptr));
  pthread_setname_np(dcm_thread_.native_handle(), "DCMClient_Main");
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient Initialization completed"; });
}

// De-initialize all the resource and free memory
void DiagClientImpl::DeInitialize() {
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient De-Initialization started"; });
  // shutdown DCM module here
  dcm_instance_ptr->SignalShutdown();
  if (dcm_thread_.joinable()) { dcm_thread_.join(); }
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "DiagClient De-Initialization completed"; });
}

diag::client::conversation::DiagClientConversation &DiagClientImpl::GetDiagnosticClientConversation(
    std::string_view conversation_name) {
  return (dcm_instance_ptr->GetDiagnosticClientConversation(conversation_name));
}

std::pair<diag::client::DiagClient::VehicleResponseResult, diag::client::vehicle_info::VehicleInfoMessageResponseUniquePtr>
DiagClientImpl::SendVehicleIdentificationRequest(
    diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) {
  return (dcm_instance_ptr->SendVehicleIdentificationRequest(vehicle_info_request));
}

}  // namespace client
}  // namespace diag
