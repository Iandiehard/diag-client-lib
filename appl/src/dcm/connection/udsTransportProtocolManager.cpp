#include "doIP_TransportProtocolHandler.h"
#include "dcm/connection/udsTransportProtocolManager.h"
#include "dcm/conversion/conversionManager.h"

namespace diag {
namespace client {
namespace uds_transport{


//ctor
UdsTransportProtocolManager::UdsTransportProtocolManager(/* pass the protocol kind */)
                            :doip_transport_handler(std::make_unique<ara::diag::doip::transportProtocolHandler::DoipTransportProtocolHandler>(handler_id_count, *this)) {
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
