#ifndef _CONVERSION_H
#define _CONVERSION_H

/* includes */
 #include "connection.h"
 #include "protocol_types.h"

namespace ara{
namespace diag{
namespace conversion{

/*
 @ Class Name        : Conversion
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class ConversionHandler
{
public:
    // ctor
    inline ConversionHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id) : handler_id_e(handler_id)
    {}
    
    // dtor
    ~ConversionHandler() = default;
    
    // Indication of Vehicle Announcement/Identification Response
    virtual ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult IndicateMessage(
                                    std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) = 0;
    
    // Indicate message Diagnostic message reception over TCP to user
    virtual std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
                      ara::diag::uds_transport::UdsMessagePtr> IndicateMessage(
                                            ara::diag::uds_transport::UdsMessage::Address source_addr,
                                            ara::diag::uds_transport::UdsMessage::Address target_addr,
                                            ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                            ara::diag::uds_transport::ChannelID channel_id,
                                            std::size_t size,
                                            ara::diag::uds_transport::Priority priority,
                                            ara::diag::uds_transport::ProtocolKind protocol_kind,
                                            std::vector<uint8_t> payloadInfo) = 0;
       
    // transmit confirmation of Vehicle Identification Request
    virtual void TransmitConfirmation(bool result) = 0;
    
    // Hands over a valid message to conversion
    virtual void HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) = 0;

protected:
    //
    ara::diag::conversion_manager::ConversionHandlerID handler_id_e;
};

} // conversion
} // diag
} // ara


#endif // _CONVERSION_H
