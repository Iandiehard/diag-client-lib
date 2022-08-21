/* includes */
#include "dcm/conversion/dmconversion.h"
#include "dcm/service/dm_uds_message.h"

namespace diag{
namespace client{
namespace conversion{

//ctor
DmConversion::DmConversion(ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier)
                            :diag::client::conversion::DiagClientConversion()
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
                            ,dm_conversion_handler(std::make_shared<DmConversionHandler>(conversion_identifier.handler_id, *this)) {
    DLT_REGISTER_CONTEXT(dm_conversion,"dmcv","Dm Conversion Context");
    payload_rx_buffer.resize(rx_buffer_size);
}

//dtor
DmConversion::~DmConversion() {
    DLT_UNREGISTER_CONTEXT(dm_conversion);
}

// startup
void DmConversion::Startup() {
    // initialize the connection
    connection_ptr->Initialize();
    // start the connection
    connection_ptr->Start();
    // Change the state to Active
    activity_status = ActivityStatusType::kActive;
}

// shutdown
void DmConversion::Shutdown() {
    // shutdown connection
    connection_ptr->Stop();
    // Change the state to InActive
    activity_status = ActivityStatusType::kInactive;
}

// Function to Send Vehicle Identification Request 
void DmConversion::SendVehicleIdentificationRequest() {
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
}

DiagClientConversion::ConnectResult DmConversion::ConnectToDiagServer(
        uds_message::UdsRequestMessage::IpAddress host_ip_addr) {
   
    // create a uds message just to get the port number, -
    // source address required from Routing Acitvation
    ara::diag::uds_transport::ByteVector payload; // empty payload
    ara::diag::uds_transport::UdsMessagePtr uds_message =
                    std::make_unique<diag::client::uds_message::DmUdsMessage>(
                                                                            source_address,
                                                                            target_address,
                                                                            host_ip_addr,
                                                                            payload);    
    // Send Connect request to doip layer
    return (static_cast<DiagClientConversion::ConnectResult>(
            connection_ptr->ConnectToHost(std::move(uds_message))));
}

DiagClientConversion::DisconnectResult  
        DmConversion::DisconnectFromDiagServer() {
    // Send disconnect request to doip layer
    return (static_cast<DiagClientConversion::DisconnectResult>(
            connection_ptr->DisconnectFromHost()));
}

std::pair<DiagClientConversion::DiagResult, uds_message::UdsResponseMessagePtr> 
        DmConversion::SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) {

    std::pair<DiagClientConversion::DiagResult, uds_message::UdsResponseMessagePtr> 
        ret_val{DiagClientConversion::DiagResult::kDiagFailed, nullptr};
    
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
        // Diagnostic Request Send successful, wait for response
        conversion_state = ConversionStateType::kDiagWaitForRes;
        DLT_LOG(dm_conversion, DLT_LOG_INFO, 
            DLT_CSTRING("Diagnostic Request Sent & Positive Ack received"));
        while(conversion_state != ConversionStateType::kIdle) {
            DLT_LOG(dm_conversion, DLT_LOG_INFO, 
                DLT_CSTRING("Dm coversion state: "), DLT_UINT8(static_cast<std::uint8_t>(conversion_state)));
            switch (conversion_state) {
                case ConversionStateType::kDiagWaitForRes :
                    // start P6Max / P2ClientMax timer
                    if(timer_sync.SyncWait(p2_client_max) ==
                            one_shot_timer::oneShotSyncTimer::timer_state::kTimeout) {
                        // no response received withing P6Max / P2ClientMax
                        conversion_state = ConversionStateType::kDiagP2MaxTimeout;
                        DLT_LOG(dm_conversion, DLT_LOG_INFO, 
                            DLT_CSTRING("Diagnostic Response P2 Timeout happened"), DLT_INT16(static_cast<int>(p2_client_max)));
                        break;
                    }
                    else {
                        // response received, state will be handled in callback
                        break;
                    }
                case ConversionStateType::kDiagRecvdPendingRes:
                case ConversionStateType::kDiagRecvdFinalRes:
                    // do nothing
                    break;

                case ConversionStateType::kDiagStartP2StarTimer:
                    // start P6Star/ P2 star client time
                    if(timer_sync.SyncWait(p2_star_client_max) ==
                            one_shot_timer::oneShotSyncTimer::timer_state::kTimeout) {
                        // no response received within P6Start/ P2Star
                        conversion_state = ConversionStateType::kDiagP2StarMaxTimeout;
                        DLT_LOG(dm_conversion, DLT_LOG_INFO, 
                            DLT_CSTRING("Diagnostic Response P2 Star Timeout happened"), DLT_INT16(static_cast<int>(p2_star_client_max)));
                        break;
                    }
                    else {
                        // response received, state will be handled in callback
                        break;
                    }
                case ConversionStateType::kDiagP2MaxTimeout:
                case ConversionStateType::kDiagP2StarMaxTimeout:
                    // Timeout case
                    conversion_state = ConversionStateType::kIdle;
                    ret_val.first = DiagClientConversion::DiagResult::kDiagResponseTimeout;
                    break;

                case ConversionStateType::kDiagSuccess:
                    // change state to idle, form the uds response and return
                    ret_val.second = std::move(uds_resp_ptr);
                    ret_val.first = DiagClientConversion::DiagResult::kDiagSuccess;
                    conversion_state = ConversionStateType::kIdle;
                    break;

                default:
                    break;
            }
        }
    }
    else {
        // Request sending failed
        ret_val.first = DiagClientConversion::DiagResult::kDiagRequestSendFailed;
    }
    return ret_val;
}

// 
void DmConversion::RegisterConnection(std::shared_ptr<ara::diag::connection::Connection> connection) {
    connection_ptr = std::move(connection);
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, 
                        ara::diag::uds_transport::UdsMessagePtr> DmConversion::IndicateMessage(
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
        if(source_addr == target_address) {
            // Check for size, else kIndicationOverflow
            if(size <= rx_buffer_size) {
                // Check for pending response
                if(payloadInfo[2] == 0x78) {
                    conversion_state = ConversionStateType::kDiagRecvdPendingRes;
                    timer_sync.StopWait();
                    ret_val.first = 
                        ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending;
                    conversion_state = ConversionStateType::kDiagStartP2StarTimer;
                    DLT_LOG(dm_conversion, DLT_LOG_INFO, 
                        DLT_CSTRING("Diagnostic Pending response received in Conversion"));
                }
                else {
                    // positive or negative response, provide valid buffer
                    conversion_state = ConversionStateType::kDiagRecvdFinalRes;
                    timer_sync.StopWait();
                    ara::diag::uds_transport::UdsMessagePtr uds_message_rx =
                            std::make_unique<diag::client::uds_message::DmUdsMessage>(
                                                                                    source_address,
                                                                                    target_address,
                                                                                    "",
                                                                                    payload_rx_buffer);                    
                    ret_val.first = 
                        ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk;
                    ret_val.second = std::move(uds_message_rx);
                    DLT_LOG(dm_conversion, DLT_LOG_INFO, 
                        DLT_CSTRING("Diagnostic Final indication received in Conversion"));
                }
            }
            else {
                ret_val.first = 
                    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOverflow;
            }
        }
        else {
            ret_val.first = 
                ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationUnknownTargetAddress;
        }
    }
    else {
        // do nothing
    }
    return ret_val;
}

// Hands over a valid message to conversion
void DmConversion::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
    if(message != nullptr) {
        uds_resp_ptr.reset(nullptr);
        DLT_LOG(dm_conversion, DLT_LOG_INFO, 
            DLT_CSTRING("Total Message Length in Handle: "), DLT_INT16(static_cast<int>(message->GetPayload().size())));
        uds_resp_ptr = std::make_unique<diag::client::uds_message::DmUdsResponse>(message->GetPayload());
        conversion_state = ConversionStateType::kDiagSuccess;
        DLT_LOG(dm_conversion, DLT_LOG_INFO, 
            DLT_CSTRING("Diagnostic Handle message called in Conversion"));
    }
}

// ctor
DmConversionHandler::DmConversionHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id,
                                        DmConversion &dm_conversion)
                    : ara::diag::conversion::ConversionHandler(handler_id),
                      dm_coversion_e(dm_conversion) {
}

// dtor
DmConversionHandler::~DmConversionHandler() {
}

// Indication of Vehicle Announcement/Identification Request
ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult 
    DmConversionHandler::IndicateMessage(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) {
    if(vehicleInfo_Ref.size() != 0) {
        // Todo : Is vector required ????
        if(vehicleInfo_Ref[0].vehInfoType 
            == ara::diag::doip::VehicleInfo::VehicleInfoType::kVehicleAnnouncementRes) {

        }
    }
}

// transmit confirmation of Vehicle Identification Request
void DmConversionHandler::TransmitConfirmation(bool result) {
    if(result != false) {
    // success
        
    }
    else {
    // failure

    }
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr> 
                    DmConversionHandler::IndicateMessage(
                                        ara::diag::uds_transport::UdsMessage::Address source_addr,
                                        ara::diag::uds_transport::UdsMessage::Address target_addr,
                                        ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                        ara::diag::uds_transport::ChannelID channel_id,
                                        std::size_t size,
                                        ara::diag::uds_transport::Priority priority,
                                        ara::diag::uds_transport::ProtocolKind protocol_kind,
                                        std::vector<uint8_t> payloadInfo) {
    return (dm_coversion_e.IndicateMessage(source_addr, 
                                            target_addr, 
                                            type, 
                                            channel_id, 
                                            size,
                                            priority, 
                                            protocol_kind, 
                                            payloadInfo));
}

// Hands over a valid message to conversion
void DmConversionHandler::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
    dm_coversion_e.HandleMessage(std::move(message));
}

} // conversion
} // client
} // diag