/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* includes */
#include "dcm_client.h"

#include "src/common/logger.h"

namespace diag {
namespace client {
namespace dcm {

// string representing vehicle discovery conversation name
std::string VehicleDiscoveryConversation{"VehicleDiscovery"};

/*
 @ Class Name        : DCM
 @ Class Description : Class to create Diagnostic Manager Client functionality                           
 */
DCMClient::DCMClient(diag::client::common::property_tree &ptree)
    : DiagnosticManager(ptree),
      uds_transport_protocol_mgr(std::make_unique<uds_transport::UdsTransportProtocolManager>()),
      conversation_mgr{std::make_unique<conversation_manager::ConversationManager>(GetConversationConfig(ptree),
                                                                                   *uds_transport_protocol_mgr)},
      diag_client_vehicle_discovery_conversation{
          conversation_mgr->GetDiagnosticClientVehicleDiscoveryConversation(VehicleDiscoveryConversation)} {}

// dtor
DCMClient::~DCMClient() = default;

// Initialize
void DCMClient::Initialize() {
  // start Vehicle Discovery
  diag_client_vehicle_discovery_conversation->Startup();
  // start Conversation Manager
  conversation_mgr->Startup();
  // start all the udsTransportProtocol Layer
  uds_transport_protocol_mgr->Startup();
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client Initialized"; });
}

// Run
void DCMClient::Run() {
  // run udsTransportProtocol layer
  uds_transport_protocol_mgr->Run();
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client is ready to serve"; });
}

// shutdown DCM
void DCMClient::Shutdown() {
  // shutdown Vehicle Discovery
  diag_client_vehicle_discovery_conversation->Shutdown();
  // shutdown Conversation Manager
  conversation_mgr->Shutdown();
  // shutdown udsTransportProtocol layer
  uds_transport_protocol_mgr->Shutdown();
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
      __FILE__, __LINE__, __func__, [](std::stringstream &msg) { msg << "Dcm Client Shutdown completed"; });
}

// Function to get the client Conversation
diag::client::conversation::DiagClientConversation &DCMClient::GetDiagnosticClientConversation(
    std::string conversation_name) {
  diag::client::conversation::DiagClientConversation *ret_conversation{nullptr};
  std::unique_ptr<diag::client::conversation::DiagClientConversation> conversation{
      conversation_mgr->GetDiagnosticClientConversion(conversation_name)};
  if (conversation != nullptr) {
    diag_client_conversation_map.insert(
        std::pair<std::string, std::unique_ptr<diag::client::conversation::DiagClientConversation>>(
            conversation_name, std::move(conversation)));
    ret_conversation = diag_client_conversation_map.at(conversation_name).get();
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
          msg << "Requested Diagnostic Client conversation created with name: " << conversation_name;
        });
  } else {
    // no conversation found
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__, [&](std::stringstream &msg) {
          msg << "Requested Diagnostic Client conversation not found with name: " << conversation_name;
        });
    assert(ret_conversation);
  }
  return *ret_conversation;
}

// Function to get read from json tree and return the config structure
diag::client::config_parser::ConversationConfig DCMClient::GetConversationConfig(
    diag::client::common::property_tree &ptree) {
  diag::client::config_parser::ConversationConfig config;
  // get the udp info for vehicle discovery
  config.udp_ip_address = ptree.get<std::string>("UdpIpAddress");
  config.udp_broadcast_address = ptree.get<std::string>("UdpBroadcastAddress");
  // get total number of conversation
  config.num_of_conversation = ptree.get<uint8_t>("Conversation.NumberOfConversation");
  // loop through all the conversation
  for (diag::client::common::property_tree::value_type &conversation_ptr:
       ptree.get_child("Conversation.ConversationProperty")) {
    diag::client::config_parser::conversationType conversation;
    conversation.conversationName = conversation_ptr.second.get<std::string>("ConversationName");
    conversation.p2ClientMax = conversation_ptr.second.get<uint16_t>("p2ClientMax");
    conversation.p2StarClientMax = conversation_ptr.second.get<uint16_t>("p2StarClientMax");
    conversation.txBufferSize = conversation_ptr.second.get<uint16_t>("TxBufferSize");
    conversation.rxBufferSize = conversation_ptr.second.get<uint16_t>("RxBufferSize");
    conversation.sourceAddress = conversation_ptr.second.get<uint16_t>("SourceAddress");
    conversation.targetAddress = conversation_ptr.second.get<uint16_t>("TargetAddress");
    conversation.network.tcpIpAddress = conversation_ptr.second.get<std::string>("Network.TcpIpAddress");
    conversation.network.portNum = conversation_ptr.second.get<uint16_t>("Network.Port");
    config.conversations.emplace_back(conversation);
  }
  return config;
}

std::pair<diag::client::DiagClient::VehicleResponseResult, diag::client::vehicle_info::VehicleInfoMessageResponsePtr>
DCMClient::SendVehicleIdentificationRequest(
    diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) {
  return diag_client_vehicle_discovery_conversation->SendVehicleIdentificationRequest(vehicle_info_request);
}

}  // namespace dcm
}  // namespace client
}  // namespace diag
