#ifndef _UDP_TRANSPORT_HANDLER_H_
#define _UDP_TRANSPORT_HANDLER_H_

//includes
#include "common/common_doIP_Header.h"
#include "channel/udp_Channel.h"

namespace ara{
namespace diag{
namespace doip{

//forward declaration
namespace  connection{
class DoipConnection;
}

namespace udpTransport{


/*
 @ Class Name        : udp_TransportHandler
 @ Class Description : Class used to create a udp transport handler to initiate transmission
                       and reception of udp message from/to user                            
*/
class udp_TransportHandler
{
public:
    // ctor
    udp_TransportHandler(kDoip_String& localIpaddress, 
                         uint16_t portNum,
                         connection::DoipConnection& doipConnection);
    // dtor
    ~udp_TransportHandler();
    // Initialize
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize ();
    // Start
    void Start();
    // Stop
    void Stop();
    // Transmit
    bool Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref);
    // Indicate Message
    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult Indicate(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref);
    // Transmit confirmation
    void TransmitConfirmation(bool result);
private:
    // reference to doip Connection 
    connection::DoipConnection& doipConnection_e;
    // Udp channel responsible for transmitting and reception of UDP messages
    std::unique_ptr<ara::diag::doip::udpChannel::udpChannel> udp_channel;
};

} // udpTransport
} // doip
} // diag
} // ara  

#endif // _UDP_TRANSPORT_HANDLER_H_
