/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _DOIP_CONNECTION_MANAGER_H_
#define _DOIP_CONNECTION_MANAGER_H_

//includes
#include "common/common_doIP_Types.h"

namespace ara{
namespace diag{
namespace doip{

//forward declaration
namespace tcpTransport{
  class tcp_TransportHandler;
}
namespace udpTransport{
  class udp_TransportHandler;
}

using InitializationResult = ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult;

namespace connection {

/*
 @ Class Name        : DoipConnection
 @ Class Description : Class to create connection to tcp & udp handler                              
 */

class DoipConnection : public ara::diag::connection::Connection {

public:
    // ctor
    DoipConnection(const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion, 
                    kDoip_String& tcpIpaddress, 
                    kDoip_String& udpIpaddress, 
                    uint16_t portNum);
    
    // dtor
    virtual ~DoipConnection();
    
    // Initialize
    InitializationResult Initialize () override;
    
    // Start the connection
    void Start () override;
    
    // Stop the connection
    void Stop () override;

    // Indicate reception of Vehicel Announcement over UDP to User
    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult 
            IndicateMessage(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) override;

    // Transmit udp
    bool Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) override;
    
    // Transmit confirmation udp
    void TransmitConfirmation(bool result);

    // Connect to host using the ip address
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
        ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) override;

    // Disconnect from Host Server
    ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
        DisconnectFromHost() override;    
    
    // Indicate message Diagnostic message reception over TCP to user
    std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
        ara::diag::uds_transport::UdsMessagePtr> IndicateMessage(
                ara::diag::uds_transport::UdsMessage::Address source_addr,
                ara::diag::uds_transport::UdsMessage::Address target_addr,
                ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                ara::diag::uds_transport::ChannelID channel_id,
                std::size_t size,
                ara::diag::uds_transport::Priority priority,
                ara::diag::uds_transport::ProtocolKind protocol_kind,
                std::vector<uint8_t> payloadInfo) override;
    
    // Transmit tcp
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
        Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) override;
    
    // Hands over a valid message to conversion
    void HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) override;
private:
    // Tcp Transport Handler
    std::unique_ptr<ara::diag::doip::tcpTransport::tcp_TransportHandler> tcp_transport_handler_e;
    // Udp Transport Handler
    std::unique_ptr<ara::diag::doip::udpTransport::udp_TransportHandler> udp_transport_handler_e;

};

/*
 @ Class Name        : DoipConnectionManager
 @ Class Description : Class manages Doip Connection                              
 */

class DoipConnectionManager
{

public:
    // ctor
    DoipConnectionManager();
    
    // dtor
    ~DoipConnectionManager();
    
    // Function to create new connection to handle doip request and response 
    std::shared_ptr<DoipConnection> FindOrCreateConnection(const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion, 
                                                            kDoip_String& tcpIpaddress, 
                                                            kDoip_String& udpIpaddress, 
                                                            uint16_t portNum);
private:
    // store all the connection created
    std::vector<std::shared_ptr<DoipConnection>> connection_box; 
    
};

} // connection
} // doip
} // diag
} // ara  


#endif // _DOIP_CONNECTION_MANAGER_H_