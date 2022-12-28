/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "handler/udp_transport_handler.h"
#include "connection/connection_manager.h"

namespace ara{
namespace diag{
namespace doip{
namespace udpTransport{

// ctor
UdpTransportHandler::UdpTransportHandler(kDoip_String &localIpaddress, uint16_t portNum,
                      connection::DoipUdpConnection& doip_connection)
                    : doip_connection_{doip_connection},
                      udp_channel(std::make_unique<ara::diag::doip::udpChannel::udpChannel>(localIpaddress, portNum, *this)) {
}

// dtor
UdpTransportHandler::~UdpTransportHandler() {
}

//Initialize the Udp Transport Handler
uds_transport::UdsTransportProtocolHandler::InitializationResult UdpTransportHandler::Initialize() {
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult RetVal =                    \
            ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeFailed;
    RetVal = udp_channel->Initialize();
    return RetVal;
}

// start handler
void UdpTransportHandler::Start() {
    udp_channel->Start();
}

// stop handler
void UdpTransportHandler::Stop() {
    udp_channel->Stop();
}

// Transmit 
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
        UdpTransportHandler::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message,
                                            ara::diag::uds_transport::ChannelID channel_id) {
    // find the corresponding channel

    // Trigger transmit
    return(udp_channel->Transmit(std::move(message)));
}

} // udpTransport
} // doip
} // diag
} // ara  
