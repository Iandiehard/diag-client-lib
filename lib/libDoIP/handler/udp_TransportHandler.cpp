/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "handler/udp_TransportHandler.h"
#include "connection/connection_Manager.h"

namespace ara{
namespace diag{
namespace doip{
namespace udpTransport{

// ctor
udp_TransportHandler::udp_TransportHandler(kDoip_String &localIpaddress, uint16_t portNum,
                      connection::DoipConnection& doipConnection)
                    : doipConnection_e(doipConnection),
                      udp_channel(std::make_unique<ara::diag::doip::udpChannel::udpChannel>(localIpaddress, portNum, *this)) {
}

// dtor
udp_TransportHandler::~udp_TransportHandler() {
}

//Initialize the Udp Transport Handler
uds_transport::UdsTransportProtocolHandler::InitializationResult udp_TransportHandler::Initialize() {
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult RetVal =                    \
            ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeFailed;
    RetVal = udp_channel->Initialize();
    return RetVal;
}

// start handler
void udp_TransportHandler::Start() {
    udp_channel->Start();
}

// stop handler
void udp_TransportHandler::Stop() {
    udp_channel->Stop();
}

// Trigger vehicle identification requests
bool udp_TransportHandler::Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) {
    return(udp_channel->Transmit(vehicleInfo_Ref));
}

// function to indicate reception of vehicle announcement to doip transpot handler
ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult udp_TransportHandler::Indicate(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) {
   return (doipConnection_e.IndicateMessage(vehicleInfo_Ref));
}

void udp_TransportHandler::TransmitConfirmation(bool result) {
    doipConnection_e.TransmitConfirmation(result);
}

} // udpTransport
} // doip
} // diag
} // ara  
