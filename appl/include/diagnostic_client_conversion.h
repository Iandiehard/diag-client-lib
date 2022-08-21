#ifndef _DIAGNOSTIC_CLIENT_CONVERSION_H_
#define _DIAGNOSTIC_CLIENT_CONVERSION_H_

#include "diagnostic_client_uds_message.h"

namespace diag {
namespace client {
namespace conversion {


class DiagClientConversion {

public:   
    // Connect Errors
    enum class ConnectResult : std::uint8_t {
        kConnectSuccess = 0,
        kConnectFailed,
        kConnectTimeout
    };

    // Disconnect Errors
    enum class DisconnectResult : std::uint8_t {
        kDisconnectSuccess = 0,
        kDisconnectFailed
    };

    // Diagnostics Request Response Error
    enum class DiagResult : std::uint8_t {
        kDiagRequestSendFailed = 0,
        kDiagFailed,
        kDiagAckTimeout,
        kDiagResponseTimeout,
        kDiagSuccess
    };
    
    // ctor
    inline DiagClientConversion() {}

    // dtor
    inline ~DiagClientConversion() = default;
    
    // startup
    virtual void Startup() = 0;
    
    // shutdown
    virtual void Shutdown() = 0;

    // function to send Vehicle Identification Request
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

    // Description   : Function to send Diagnostic Activation Request
    // @param input  : UdsRequestMessageConstPtr
    // @return value : DiagResult, UdsResponseMessagePtr
    virtual std::pair<DiagResult, uds_message::UdsResponseMessagePtr>  
            SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) = 0;
};


} // conversion
} // client
} // diag

#endif // _DIAGNOSTIC_CLIENT_CONVERSION_H_