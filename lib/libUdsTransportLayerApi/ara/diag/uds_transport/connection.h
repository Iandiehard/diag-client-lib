#ifndef _CONNECTION_H
#define _CONNECTION_H

/* includes */
#include "protocol_types.h"
#include "protocol_mgr.h"
#include "protocol_handler.h"

namespace ara{
namespace diag{
namespace connection{

/*
 @ Class Name        : Connection
 @ Class Description : Class to creating connection                           
 */

using InitializationResult = uds_transport::UdsTransportProtocolHandler::InitializationResult;

class Connection
{
public:
    // ctor
    inline Connection(ConnectionId connection_id, const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion)
                        : conversion_e(conversion)
                        , connection_id_e(connection_id) {
    }
    
    // dtor
    inline ~Connection() = default;
    
    // Initialize
    virtual InitializationResult Initialize () = 0;
    
    // Start the connection
    virtual void Start () = 0;
    
    // Stop the connection
    virtual void Stop () = 0;
    
    // Indicate reception of Vehicel Announcement over UDP to User
    virtual ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult IndicateMessage(
                                    std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) = 0;

    // Transmit udp data
    virtual bool Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) = 0;
    
    // Transmit confirmation udp
    virtual void TransmitConfirmation(bool result) = 0;

    // Connect to Host Server
    virtual ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
                ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) = 0;
    
    // Disconnect from Host Server
    virtual ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
                DisconnectFromHost() = 0;

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
    
    // Transmit tcp data
    virtual ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
                Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) = 0;

    // Hands over a valid message to conversion
    virtual void HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) = 0;
protected:
    // Store the conversion
    const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion_e;    
private:
    // store the connection id
    ConnectionId connection_id_e;
};


}   // connection 
}   // diag
}   // ara


#endif // _CONNECTION_H