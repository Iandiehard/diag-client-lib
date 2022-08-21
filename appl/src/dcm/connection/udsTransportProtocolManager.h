#ifndef _UDS_TRANSPORTPROTOCOLMANAGER_H_
#define _UDS_TRANSPORTPROTOCOLMANAGER_H_

#include "ara/diag/uds_transport/protocol_handler.h"

namespace diag {
namespace client {

// forward declaration
namespace conversion_manager{
class ConversionManager;
}

namespace uds_transport{

/*
 @ Class Name        : UdsTransportProtocolManager
 @ Class Description : This class must be instantiated by user for using the transport protocol functionalities.
                       This will inherit uds transport protocol handler
 */

class UdsTransportProtocolManager : public ::ara::diag::uds_transport::UdsTransportProtocolMgr {

public:
    //ctor
    UdsTransportProtocolManager(); 
    //dtor
    virtual ~UdsTransportProtocolManager();
    // initialize all the transport protocol handler
    void Startup();
    // start all the transport protocol handler
    void Run();
    // terminate all the transport protocol handler
    void Shutdown();

    // store doip transport handler
    std::unique_ptr<::ara::diag::uds_transport::UdsTransportProtocolHandler> doip_transport_handler;
    // handler id count
    ::ara::diag::uds_transport::UdsTransportProtocolHandlerID handler_id_count = 0;
};


} // uds_transport
} // client
} // diag

#endif // _UDS_TRANSPORTPROTOCOLMANAGER_H_
