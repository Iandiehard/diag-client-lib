/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* includes */
#include "diagnostic_communication_manager.h"
#include "src/common/logger.h"

namespace diag {
namespace client {
namespace dcm{

/*
 @ Class Name        : DCM
 @ Class Description : Class to create Diagnostic Manager Client functionality                           
 */

// ctor
DCMClient::DCMClient(diag::client::common::property_tree &ptree)
  : DiagnosticManager(ptree),
    uds_transport_protocol_mgr(std::make_unique<uds_transport::UdsTransportProtocolManager>()),
    conversation_mgr{std::make_unique<conversation_manager::ConversationManager>(
      GetConversationConfig(ptree), *uds_transport_protocol_mgr)} {
}

// dtor
DCMClient::~DCMClient() {
}

// Initialize
void DCMClient::Initialize() {
  // start Conversation Manager
  conversation_mgr->Startup();
  // start all the udsTransportProtocol Layer
  uds_transport_protocol_mgr->Startup();
  
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
    __FILE__, __LINE__, __func__ , [](std::stringstream& msg) {
      msg << "Dcm Client Initialized";
    });
}

// Run
void DCMClient::Run() {
  // run udsTransportProtocol layer
  uds_transport_protocol_mgr->Run();
}

// shutdown DCM
void DCMClient::Shutdown() {
  // shutdown Conversation Manager
  conversation_mgr->Shutdown();
  // shutdown udsTransportProtocol layer
  uds_transport_protocol_mgr->Shutdown();
  
  logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
    __FILE__, __LINE__, __func__ , [](std::stringstream& msg) {
      msg << "Dcm Client Shutdown done";
    });
}

// Function to get the client Conversation
diag::client::conversation::DiagClientConversation&
  DCMClient::GetDiagnosticClientConversation(std::string conversation_name) {
  diag::client::conversation::DiagClientConversation* ret_conversation = nullptr;
  std::unique_ptr<diag::client::conversation::DiagClientConversation> 
    conversation{conversation_mgr->GetDiagnosticClientConversion(conversation_name)};
  if(conversation != nullptr) {
    diag_client_conversation_map.insert(
      std::pair<std::string, std::unique_ptr<diag::client::conversation::DiagClientConversation>>(
        conversation_name,
        std::move(conversation)
      ));
    ret_conversation = diag_client_conversation_map.at(conversation_name).get();
    
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
      __FILE__, __LINE__, __func__ , [&](std::stringstream& msg) {
        msg << "Requested Diagnostic Client conversation created with name: "
          << conversation_name;
      });
  }
  else {
    // error logging, no conversation found
    logger::DiagClientLogger::GetDiagClientLogger().GetLogger().LogError(
      __FILE__, __LINE__, __func__ , [&](std::stringstream& msg) {
        msg << "Requested Diagnostic Client conversation not found with name: "
            << conversation_name;
      });
  }
  return *ret_conversation;
}

// Function to get read from json tree and return the config structure
diag::client::config_parser::ConversationConfig
        DCMClient::GetConversationConfig(diag::client::common::property_tree & ptree) {
  diag::client::config_parser::ConversationConfig config;
  // get total number of conversation
  config.num_of_conversation =  ptree.get<uint8_t>("Conversation.NumberOfConversation");
  // loop through all the conversation
  for(diag::client::common::property_tree::value_type &conversation_ptr :
    ptree.get_child("Conversation.ConversationProperty")) {
      diag::client::config_parser::conversationType conversation;
    
      conversation.conversationName             = conversation_ptr.second.get<std::string>("ConversationName");
      conversation.p2ClientMax                  = conversation_ptr.second.get<uint16_t>("p2ClientMax");
      conversation.p2StarClientMax              = conversation_ptr.second.get<uint16_t>("p2StarClientMax");
      conversation.txBufferSize                 = conversation_ptr.second.get<uint16_t>("TxBufferSize");
      conversation.rxBufferSize                 = conversation_ptr.second.get<uint16_t>("RxBufferSize");
      conversation.sourceAddress                = conversation_ptr.second.get<uint16_t>("SourceAddress");
      conversation.targetAddress                = conversation_ptr.second.get<uint16_t>("TargetAddress");
      conversation.network.tcpIpAddress         = conversation_ptr.second.get<std::string>("Network.TcpIpAddress");
      conversation.network.udpIpAddress         = conversation_ptr.second.get<std::string>("Network.UdpIpAddress");
      conversation.network.udpBroadcastAddress  = conversation_ptr.second.get<std::string>("Network.UdpBroadcastAddress");
      conversation.network.portNum              = conversation_ptr.second.get<uint16_t>("Network.Port");
      config.conversations.emplace_back(conversation);
  }
  return config;
}

diag::client::vehicle_info::VehicleInfoMessageResponsePtr
  DCMClient::SendVehicleIdentificationRequest(
    diag::client::vehicle_info::VehicleInfoListRequestType vehicle_info_request) {

}

diag::client::vehicle_info::VehicleInfoMessageResponsePtr
  DCMClient::GetDiagnosticServerList() {

}

} // dcm
} // client
} // diag
