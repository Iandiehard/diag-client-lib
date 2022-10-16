/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/* includes */
#include "dcm/conversion/dmconversation.h"
#include "dcm/service/dm_uds_message.h"

namespace diag{
namespace client{
namespace conversation{

//ctor
DmConversation::DmConversation(
        std::string conversion_name,
        ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier)
                            :diag::client::conversation::DiagClientConversation()
                            ,activity_status(ActivityStatusType::kInactive)
                            ,active_session(SessionControlType::kDefaultSession)
                            ,active_security(SecurityLevelType::kLocked)
                            ,tx_buffer_size(conversion_identifier.tx_buffer_size)
                            ,rx_buffer_size(conversion_identifier.rx_buffer_size)
                            ,p2_client_max(conversion_identifier.p2_client_max)
                            ,p2_star_client_max(conversion_identifier.p2_star_client_max)
                            ,source_address(conversion_identifier.source_address)
                            ,target_address(conversion_identifier.target_address)
                            ,port_num(conversion_identifier.port_num)
                            ,broadcast_address(conversion_identifier.udp_broadcast_address)
                            ,convrs_name(conversion_name)
                            ,dm_conversion_handler(std::make_shared<DmConversationHandler>(conversion_identifier.handler_id, *this)) {
    DLT_REGISTER_CONTEXT(dm_conversion,"dmcv","Dm Conversion Context");
}

//dtor
DmConversation::~DmConversation() {
    DLT_UNREGISTER_CONTEXT(dm_conversion);
}

// startup
void DmConversation::Startup() {
    // initialize the connection
    connection_ptr->Initialize();
    // start the connection
    connection_ptr->Start();
    // Change the state to Active
    activity_status = ActivityStatusType::kActive;

    DLT_LOG(dm_conversion, DLT_LOG_INFO,
        DLT_CSTRING("'"),
        DLT_CSTRING(convrs_name.c_str()),
        DLT_CSTRING("'"),
        DLT_CSTRING("->"),
        DLT_CSTRING("Startup completed"));
}

// shutdown
void DmConversation::Shutdown() {
    // shutdown connection
    connection_ptr->Stop();
    // Change the state to InActive
    activity_status = ActivityStatusType::kInactive;
    
    DLT_LOG(dm_conversion, DLT_LOG_INFO,
        DLT_CSTRING("'"),
        DLT_CSTRING(convrs_name.c_str()),
        DLT_CSTRING("'"),
        DLT_CSTRING("->"),
        DLT_CSTRING("Shutdown completed"));
}

// Function to Send Vehicle Identification Request 
void DmConversation ::SendVehicleIdentificationRequest() {

    DLT_LOG(dm_conversion, DLT_LOG_ERROR,
        DLT_CSTRING("'"),
        DLT_CSTRING(convrs_name.c_str()),
        DLT_CSTRING("'"),
        DLT_CSTRING("->"),
        DLT_CSTRING("Vehicle Discovery phase not supported yet"),
        DLT_CSTRING(":Check 'https://github.com/Mandarein/diag-client-lib/issues/5'"));
    
    /* [TODO] : https://github.com/Mandarein/diag-client-lib/issues/5
    ara::diag::doip::VehicleInfo vehicleInfo;
    // Veh Identification with VIN
    vehicleInfo.vehInfoType = ara::diag::doip::VehicleInfo::VehicleInfoType::kVehicleIdentificationReqVIN;
    // Assign broadcast ip & port number
    vehicleInfo.hostIpAddress = broadcast_address;
    vehicleInfo.hostPortNum = port_num;
    // Fill VIN
    for(uint8_t vin_indx = 0; vin_indx < ara::diag::doip::kDoip_VIN_Size; vin_indx ++) {
        vehicleInfo.vin[vin_indx] = 0x01;
    }
    // Trigger Transmit
    connection_ptr->Transmit(vehicleInfo);
    */
}

DiagClientConversation::ConnectResult DmConversation::ConnectToDiagServer(
        uds_message::UdsRequestMessage::IpAddress host_ip_addr) {
   
    // create an uds message just to get the port number
    // source address required from Routing Activation
    ara::diag::uds_transport::ByteVector payload; // empty payload  
      
    // Send Connect request to doip layer
    DiagClientConversation::ConnectResult ret_val =
            static_cast<DiagClientConversation::ConnectResult>(
                connection_ptr->ConnectToHost(std::move(
                        std::make_unique<diag::client::uds_message::DmUdsMessage>(
                                                                                source_address,
                                                                                target_address,
                                                                                host_ip_addr,
                                                                                payload))));
    
    if(ret_val ==
        DiagClientConversation::ConnectResult::kConnectSuccess) {
        remote_address = std::string(host_ip_addr);
        DLT_LOG(dm_conversion, DLT_LOG_INFO,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"),
            DLT_CSTRING("Successfully connected to Server with ip"),
            DLT_CSTRING(host_ip_addr.c_str()));
    }
    else {
        DLT_LOG(dm_conversion, DLT_LOG_INFO,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"),
            DLT_CSTRING("Failed connecting to Server with ip"),
            DLT_CSTRING(host_ip_addr.c_str()));
    }
    return ret_val;
}

DiagClientConversation::DisconnectResult
    DmConversation::DisconnectFromDiagServer() {
    // Send disconnect request to doip layer
    DiagClientConversation::DisconnectResult ret_val =
        static_cast<DiagClientConversation::DisconnectResult>(
            connection_ptr->DisconnectFromHost());

    if(ret_val ==
            DiagClientConversation::DisconnectResult::kDisconnectSuccess) {
        DLT_LOG(dm_conversion, DLT_LOG_INFO,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"),
            DLT_CSTRING("Successfully disconnected from Server with ip"),
            DLT_CSTRING(remote_address.c_str()));
    }
    else {
        DLT_LOG(dm_conversion, DLT_LOG_INFO,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"),
            DLT_CSTRING("Failed disconnecting from Server with ip"),
            DLT_CSTRING(remote_address.c_str()));
    }
    return ret_val;
}

std::pair<DiagClientConversation::DiagResult, uds_message::UdsResponseMessagePtr>
    DmConversation::SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) {

    std::pair<DiagClientConversation::DiagResult, uds_message::UdsResponseMessagePtr>
        ret_val{DiagClientConversation::DiagResult::kDiagFailed, nullptr};
    
    if(message != nullptr) {
        // fill the data
        ara::diag::uds_transport::ByteVector payload;
        payload.reserve(message->GetPayload().size());
        for(auto const byte: message->GetPayload()) {
            payload.emplace_back(byte);
        }

        // create the uds message
        ara::diag::uds_transport::UdsMessagePtr uds_message =
                        std::make_unique<diag::client::uds_message::DmUdsMessage>(
                                                                                source_address,
                                                                                target_address,
                                                                                message->GetHostIpAddress(),
                                                                                payload);
        if(connection_ptr->Transmit(std::move(uds_message)) != 
            ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed) {
            // Diagnostic Request Sent successful, wait for response
            conversion_state = ConversionStateType::kDiagWaitForRes;
            DLT_LOG(dm_conversion, DLT_LOG_INFO,
                DLT_CSTRING("'"),
                DLT_CSTRING(convrs_name.c_str()),
                DLT_CSTRING("'"),
                DLT_CSTRING("->"),
                DLT_CSTRING("Diagnostic Request Sent & Positive Ack received"));
            while(conversion_state != ConversionStateType::kIdle) {
                DLT_LOG(dm_conversion, DLT_LOG_VERBOSE, 
                    DLT_CSTRING("Dm coversion state:"), 
                    DLT_UINT8(static_cast<std::uint8_t>(conversion_state)));
                switch (conversion_state) {
                    case ConversionStateType::kDiagWaitForRes :
                        {// start P6Max / P2ClientMax timer
                            if(sync_timer.Start(p2_client_max) ==
                                    conv_sync_timer::timer_state::kTimeout) {
                                // no response received withing P6Max / P2ClientMax
                                conversion_state = ConversionStateType::kDiagP2MaxTimeout;
                                DLT_LOG(dm_conversion, DLT_LOG_WARN,
                                    DLT_CSTRING("'"),
                                    DLT_CSTRING(convrs_name.c_str()),
                                    DLT_CSTRING("'"),
                                    DLT_CSTRING("->"), 
                                    DLT_CSTRING("Diagnostic Response P2 Timeout happened"), 
                                    DLT_UINT16(p2_client_max));
                            }
                            else {
                                // response received, state will be handled in callback
                            }
                            break;
                        }
                    case ConversionStateType::kDiagRecvdPendingRes:
                    case ConversionStateType::kDiagRecvdFinalRes:
                        // do nothing
                        break;

                    case ConversionStateType::kDiagStartP2StarTimer:
                        {// start P6Star/ P2 star client time
                            if(sync_timer.Start(p2_star_client_max) ==
                                    conv_sync_timer::timer_state::kTimeout) {
                                // no response received within P6Start/ P2Star
                                conversion_state = ConversionStateType::kDiagP2StarMaxTimeout;
                                DLT_LOG(dm_conversion, DLT_LOG_WARN,
                                    DLT_CSTRING("'"),
                                    DLT_CSTRING(convrs_name.c_str()),
                                    DLT_CSTRING("'"),
                                    DLT_CSTRING("->"), 
                                    DLT_CSTRING("Diagnostic Response P2 Star Timeout happened"), 
                                    DLT_UINT16(p2_star_client_max));                     
                            }
                            else {
                                // response received, state will be handled in callback
                            }
                            break;
                        }
                    case ConversionStateType::kDiagP2MaxTimeout:
                    case ConversionStateType::kDiagP2StarMaxTimeout:
                        {// Timeout case
                            conversion_state = ConversionStateType::kIdle;
                            ret_val.first = DiagClientConversation::DiagResult::kDiagResponseTimeout;
                            break;
                        }
                    case ConversionStateType::kDiagSuccess:
                        {// change state to idle, form the uds response and return
                            ret_val.second = std::move(
                                    std::make_unique<diag::client::uds_message::DmUdsResponse>(payload_rx_buffer));
                            ret_val.first = DiagClientConversation::DiagResult::kDiagSuccess;
                            conversion_state = ConversionStateType::kIdle;
                            break;
                        }
                    default:
                        break;
                }
            }
        }
        else {
            // Request sending failed
            ret_val.first = DiagClientConversation::DiagResult::kDiagRequestSendFailed;
        }
    }
    else {
        DLT_LOG(dm_conversion, DLT_LOG_ERROR,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"), 
            DLT_CSTRING("Diagnostic Request is empty"), 
            DLT_UINT16(p2_star_client_max));         
    }

    return ret_val;
}

// 
void DmConversation::RegisterConnection(std::shared_ptr<ara::diag::connection::Connection> connection) {
    connection_ptr = std::move(connection);
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
                        ara::diag::uds_transport::UdsMessagePtr> DmConversation::IndicateMessage(
                                            ara::diag::uds_transport::UdsMessage::Address source_addr,
                                            ara::diag::uds_transport::UdsMessage::Address target_addr,
                                            ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                            ara::diag::uds_transport::ChannelID channel_id,
                                            std::size_t size,
                                            ara::diag::uds_transport::Priority priority,
                                            ara::diag::uds_transport::ProtocolKind protocol_kind,
                                            std::vector<uint8_t> payloadInfo) {
    std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
        ara::diag::uds_transport::UdsMessagePtr> ret_val{
                                ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationNOk,
                                nullptr};
    // Verify the payload received :-
    if(payloadInfo.size() != 0) {
        // Check for size, else kIndicationOverflow
        if(size <= rx_buffer_size) {
            // Check for pending response
            if(payloadInfo[2] == 0x78) {
                DLT_LOG(dm_conversion, DLT_LOG_INFO,
                    DLT_CSTRING("'"),
                    DLT_CSTRING(convrs_name.c_str()),
                    DLT_CSTRING("'"),
                    DLT_CSTRING("->"), 
                    DLT_CSTRING("Diagnostic Pending response received in Conversion"));
                conversion_state = ConversionStateType::kDiagRecvdPendingRes;
                sync_timer.Stop();
                ret_val.first = 
                    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending;
                conversion_state = ConversionStateType::kDiagStartP2StarTimer;
            }
            else {
                DLT_LOG(dm_conversion, DLT_LOG_DEBUG,
                    DLT_CSTRING("'"),
                    DLT_CSTRING(convrs_name.c_str()),
                    DLT_CSTRING("'"),
                    DLT_CSTRING("->"),  
                    DLT_CSTRING("Diagnostic Final response received in Conversion"));
                // positive or negative response, provide valid buffer
                conversion_state = ConversionStateType::kDiagRecvdFinalRes;
                sync_timer.Stop();
                payload_rx_buffer.resize(size);                  
                ret_val.first = 
                    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk;
                ret_val.second = std::move(
                                std::make_unique<diag::client::uds_message::DmUdsMessage>(
                                                                                        source_address,
                                                                                        target_address,
                                                                                        "",
                                                                                        payload_rx_buffer));
            }
        }
        else {
            DLT_LOG(dm_conversion, DLT_LOG_ERROR,
                DLT_CSTRING("'"),
                DLT_CSTRING(convrs_name.c_str()),
                DLT_CSTRING("'"),
                DLT_CSTRING("->"), 
                DLT_CSTRING("Diagnostic Conversion Error Indication Overflow"));
            ret_val.first = 
                ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOverflow;
        }
    }
    else {
        // do nothing
        DLT_LOG(dm_conversion, DLT_LOG_ERROR,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"), 
            DLT_CSTRING("Diagnostic Conversion Rx Payload size 0 received"));
    }
    return ret_val;
}

// Hands over a valid message to conversion
void DmConversation::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
    if(message != nullptr) {
        DLT_LOG(dm_conversion, DLT_LOG_INFO, 
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"), 
            DLT_CSTRING("Diagnostic Handle message received in Conversion"));
        
        DLT_LOG(dm_conversion, DLT_LOG_DEBUG,
            DLT_CSTRING("'"),
            DLT_CSTRING(convrs_name.c_str()),
            DLT_CSTRING("'"),
            DLT_CSTRING("->"),  
            DLT_CSTRING("Total Message Length in HandleMessage: "), DLT_INT16(static_cast<int>(payload_rx_buffer.size())));
        conversion_state = ConversionStateType::kDiagSuccess;
    }
}

// ctor
DmConversationHandler::DmConversationHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id,
                                         DmConversation &dm_conversion)
                    : ara::diag::conversion::ConversionHandler(handler_id),
                      dm_conversation_e(dm_conversion) {
}

// Indication of Vehicle Announcement/Identification Request
ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult
    DmConversationHandler::IndicateMessage(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) {
    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult result;
    if(vehicleInfo_Ref.size() != 0) {
        // Todo : Is vector required ????
        if(vehicleInfo_Ref[0].vehInfoType 
            == ara::diag::doip::VehicleInfo::VehicleInfoType::kVehicleAnnouncementRes) {

        }
    }
    return result;
}

// transmit confirmation of Vehicle Identification Request
void DmConversationHandler::TransmitConfirmation(bool result) {
    if(result) {
    // success
    }
    else {
    // failure
    }
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>
        DmConversationHandler::IndicateMessage(
                                        ara::diag::uds_transport::UdsMessage::Address source_addr,
                                        ara::diag::uds_transport::UdsMessage::Address target_addr,
                                        ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                        ara::diag::uds_transport::ChannelID channel_id,
                                        std::size_t size,
                                        ara::diag::uds_transport::Priority priority,
                                        ara::diag::uds_transport::ProtocolKind protocol_kind,
                                        std::vector<uint8_t> payloadInfo) {
    return (dm_conversation_e.IndicateMessage(source_addr,
                                            target_addr, 
                                            type, 
                                            channel_id, 
                                            size,
                                            priority, 
                                            protocol_kind, 
                                            payloadInfo));
}

// Hands over a valid message to conversion
void DmConversationHandler::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
    dm_conversation_e.HandleMessage(std::move(message));
}

} // conversion
} // client
} // diag