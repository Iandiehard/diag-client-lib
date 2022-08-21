/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _TCP_TRANSPORT_HANDLER_H_
#define _TCP_TRANSPORT_HANDLER_H_

//includes
#include "common/common_doIP_Types.h"
#include "channel/tcp_Channel.h"

namespace ara{
namespace diag{
namespace doip{

//forward declaration
namespace  connection{
class DoipConnection;
}

namespace tcpTransport{

/*
 @ Class Name        : tcp_TransportHandler
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver                              
 */

class tcp_TransportHandler
{
public:
    // ctor
    tcp_TransportHandler(kDoip_String& localIpaddress, 
                         uint16_t portNum, 
                         uint8_t total_tcpChannelReq,
                         connection::DoipConnection& doipConnection);
    
    // dtor
    ~tcp_TransportHandler();
    
    // Initialize
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize();    
    
    // Start
    void Start();
    
    // Stop
    void Stop();
    
    // Connect to remote Host
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult 
            ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message);

    // Disconnect from remote Host
    ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
            DisconnectFromHost();

    // Transmit
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
            Transmit (ara::diag::uds_transport::UdsMessageConstPtr message, ara::diag::uds_transport::ChannelID channel_id);
    
    // Indicate message Diagnostic message reception over TCP to user
    std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>
                                        IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                                                        ara::diag::uds_transport::UdsMessage::Address target_addr,
                                                        ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                                        ara::diag::uds_transport::ChannelID channel_id,
                                                        std::size_t size, ara::diag::uds_transport::Priority priority,
                                                        ara::diag::uds_transport::ProtocolKind protocol_kind,
                                                        std::vector<uint8_t> payloadInfo);
    
    // Hands over a valid received Uds message (currently this is only a request type) from transport
    // layer to session layer                
    void HandleMessage (ara::diag::uds_transport::UdsMessagePtr message);
private:
    // reference to doip Connection 
    connection::DoipConnection& doipConnection_e;
    
    // Tcp channel responsible for transmitting and reception of TCP messages
    std::unique_ptr<ara::diag::doip::tcpChannel::tcpChannel> tcp_channel;
    
    // Max number of doip channel
    uint8_t max_tcpChannel;    
};

} // tcpTransport
} // doip
} // diag
} // ara  

#endif // _TCP_TRANSPORT_HANDLER_H_
