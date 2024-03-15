/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONNECTION_UDS_TRANSPORT_PROTOCOL_MANAGER_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONNECTION_UDS_TRANSPORT_PROTOCOL_MANAGER_H

#include "uds_transport/protocol_handler.h"

namespace diag {
namespace client {
namespace uds_transport {
/*
 @ Class Name        : UdsTransportProtocolManager
 @ Class Description : This class must be instantiated by user for using the transport protocol functionalities.
                       This will inherit uds transport protocol handler
 */
class UdsTransportProtocolManager final : public ::uds_transport::UdsTransportProtocolMgr {
 public:
  //ctor
  UdsTransportProtocolManager();

  //dtor
  ~UdsTransportProtocolManager() override = default;

  // initialize all the transport protocol handler
  void Startup() override;

  // start all the transport protocol handler
  void Run() override;

  // terminate all the transport protocol handler
  void Shutdown() override;

  ::uds_transport::UdsTransportProtocolHandler& GetTransportProtocolHandler();

 private:
  // store doip transport handler
  std::unique_ptr<::uds_transport::UdsTransportProtocolHandler> doip_transport_handler;

  // handler id count
  ::uds_transport::UdsTransportProtocolHandler::UdsTransportProtocolHandlerId handler_id_count = 0;
};
}  // namespace uds_transport
}  // namespace client
}  // namespace diag
#endif  // DIAGNOSTIC_CLIENT_LIB_APPL_SRC_DCM_CONNECTION_UDS_TRANSPORT_PROTOCOL_MANAGER_H
