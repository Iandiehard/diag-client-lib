/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//includes
#include "connection/connection_manager.h"
#include "handler/tcp_transport_handler.h"
#include "handler/udp_TransportHandler.h"


namespace ara{
namespace diag{
namespace doip{

namespace connection {

/*
 @ Class Name        : DoipConnection
 @ Class Description : Class to create connection to tcp & udp handler                              
 */

// ctor
DoipTcpConnection::DoipTcpConnection(const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion,
                                kDoip_String& tcpIpaddress,
                                kDoip_String& udpIpaddress, 
                                uint16_t portNum)
                                :ara::diag::connection::Connection(1, conversion),
                                tcp_transport_handler_(std::make_unique<ara::diag::doip::tcpTransport::TcpTransportHandler>(tcpIpaddress,
                                                                                                                              portNum, 
                                                                                                                              1, 
                                                                                                                              *this)),
                                udp_transport_handler_(std::make_unique<ara::diag::doip::udpTransport::udp_TransportHandler>(udpIpaddress,
                                                                                                                              portNum, 
                                                                                                                              *this)) {
}

//dtor
DoipTcpConnection::~DoipTcpConnection() {
}

// Initialize
InitializationResult DoipTcpConnection::Initialize () {
    InitializationResult retVal = InitializationResult::kInitializeFailed;
    retVal = udp_transport_handler_->Initialize();
    retVal = tcp_transport_handler_->Initialize();
    return (retVal);
}

// Start the Tp Handlers
void DoipTcpConnection::Start() {
    udp_transport_handler_->Start();
    tcp_transport_handler_->Start();
}

// Stop the Tp handlers
void DoipTcpConnection::Stop() {
    udp_transport_handler_->Stop();
    tcp_transport_handler_->Stop();
}

// Indicate reception of Vehicle Announcement over UDP to User
ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult 
    DoipTcpConnection::IndicateMessage(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) {
    return conversion_e->IndicateMessage(vehicleInfo_Ref);
}

// Connect to host server
ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
        DoipTcpConnection::ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) {
    return(tcp_transport_handler_->ConnectToHost(std::move(message)));
}

// Disconnect from host server
ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
        DoipTcpConnection::DisconnectFromHost() {
    return(tcp_transport_handler_->DisconnectFromHost());
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
        ara::diag::uds_transport::UdsMessagePtr> DoipTcpConnection::IndicateMessage(
        ara::diag::uds_transport::UdsMessage::Address source_addr,
        ara::diag::uds_transport::UdsMessage::Address target_addr,
        ara::diag::uds_transport::UdsMessage::TargetAddressType type,
        ara::diag::uds_transport::ChannelID channel_id,
        std::size_t size,
        ara::diag::uds_transport::Priority priority,
        ara::diag::uds_transport::ProtocolKind protocol_kind,
        std::vector<uint8_t> payloadInfo) {
    // Send Indication to conversion
    return (conversion_e->IndicateMessage(source_addr, 
                                        target_addr, 
                                        type, 
                                        channel_id, 
                                        size,
                                        priority, 
                                        protocol_kind, 
                                        payloadInfo)); 
}

// Function to transmit the uds message
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
            DoipTcpConnection::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) {
    ara::diag::uds_transport::ChannelID channel_id = 0;
    return(tcp_transport_handler_->Transmit(std::move(message), channel_id));
}

// Transmit udp data
bool DoipTcpConnection::Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref) {
    return(udp_transport_handler_->Transmit(vehicleInfo_Ref));
}

// Transmit confirmation for udp message 
void DoipTcpConnection::TransmitConfirmation(bool result) {
    conversion_e->TransmitConfirmation(result);
}

// Hands over a valid message to conversion
void DoipTcpConnection::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
    // send full message to conversion
    conversion_e->HandleMessage(std::move(message));
}

// Function to create new connection to handle doip request and response
std::shared_ptr<DoipTcpConnection> DoipConnectionManager::FindOrCreateConnection(
        const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion, 
        kDoip_String& tcp_ip_address,
        kDoip_String& udp_ip_address,
        uint16_t port_num) {
    return (std::make_shared<DoipTcpConnection>(conversion,
                                             tcp_ip_address,
                                             udp_ip_address,
                                             port_num));
}

} // connection
} // doip
} // diag
} // ara  
