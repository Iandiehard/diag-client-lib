/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H

#include "diagnostic_client_uds_message.h"
#include <cstdint>

namespace diag {
namespace client {
namespace conversation {


class DiagClientConversation {

public:   
    // Connect Errors
    enum class ConnectResult : std::uint8_t {
        kConnectSuccess = 0U,
        kConnectFailed,
        kConnectTimeout
    };

    // Disconnect Errors
    enum class DisconnectResult : std::uint8_t {
        kDisconnectSuccess = 0U,
        kDisconnectFailed
    };

    // Diagnostics Request Response Error
    enum class DiagResult : std::uint8_t {
        kDiagRequestSendFailed = 0U,
        kDiagFailed,
        kDiagAckTimeout,
        kDiagResponseTimeout,
        kDiagSuccess
    };
    
    // ctor
    inline DiagClientConversation() = default;

    // dtor
    inline ~DiagClientConversation() = default;
    
    // Description   : Function to start the Diagnostic Client Conversion
    // @param input  : Nothing
    // @return value : void
    virtual void Startup() = 0;
    
    // Description   : Function to shut down the Diagnostic Client Conversion
    // @param input  : Nothing
    // @return value : void
    virtual void Shutdown() = 0;

    // Description   : Function to send vehicle identification request
    // @param input  : Nothing
    // @return value : void
    virtual void SendVehicleIdentificationRequest() = 0;

    // Description   : Function to connect to Diagnostic Server
    // @param input  : Nothing
    // @return value : ConnectResult
    virtual ConnectResult 
            ConnectToDiagServer(uds_message::UdsRequestMessage::IpAddress host_ip_addr) = 0;

    // Description   : Function to disconnect from Diagnostic Server
    // @param input  : Nothing
    // @return value : DisconnectResult
    virtual DisconnectResult 
            DisconnectFromDiagServer() = 0;

    // Description   : Function to send Diagnostic Request and get Diagnostic Response
    // @param input  : UdsRequestMessageConstPtr
    // @return value : DiagResult, UdsResponseMessagePtr
    virtual std::pair<DiagResult, uds_message::UdsResponseMessagePtr>  
            SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) = 0;
};

} // conversation
} // client
} // diag

#endif // DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_CONVERSATION_H
