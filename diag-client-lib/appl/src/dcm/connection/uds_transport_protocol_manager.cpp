/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "doip_transport_protocol_handler.h"
#include "dcm/connection/uds_transport_protocol_manager.h"
#include "dcm/conversion/conversation_manager.h"

namespace diag {
namespace client {
namespace uds_transport{


//ctor
UdsTransportProtocolManager::UdsTransportProtocolManager(/* pass the protocol kind */)
                            :doip_transport_handler(
                                std::make_unique<ara::diag::doip::transportProtocolHandler::DoipTransportProtocolHandler>(handler_id_count, *this)) {
}

//dtor
UdsTransportProtocolManager::~UdsTransportProtocolManager() {
}

// initialize all the transport protocol handler
void UdsTransportProtocolManager::Startup() {
    //Initialize all the handlers in box
    doip_transport_handler->Initialize();
}
// start all the transport protocol handler
void UdsTransportProtocolManager::Run() {
    //Start all the handlers in box
    doip_transport_handler->Start();
}
// terminate all the transport protocol handler
void UdsTransportProtocolManager::Shutdown() {
    //Stop all the handlers in box
    doip_transport_handler->Stop();
}

} // uds_transport
} // client
} // diag
