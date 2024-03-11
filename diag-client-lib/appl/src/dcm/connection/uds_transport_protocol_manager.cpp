/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "src/dcm/connection/uds_transport_protocol_manager.h"

#include "doip_transport_protocol_handler.h"

namespace diag {
namespace client {
namespace uds_transport {
//ctor
UdsTransportProtocolManager::UdsTransportProtocolManager(
    /* pass the protocol kind */)
    : doip_transport_handler{std::make_unique<doip_client::transport_protocol_handler::DoipTransportProtocolHandler>(
          handler_id_count, *this)} {}

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

::uds_transport::UdsTransportProtocolHandler& UdsTransportProtocolManager::GetTransportProtocolHandler() {
  return *doip_transport_handler;
}
}  // namespace uds_transport
}  // namespace client
}  // namespace diag
