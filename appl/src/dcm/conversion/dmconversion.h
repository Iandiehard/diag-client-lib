/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _DCM_CONVERSION_H
#define _DCM_CONVERSION_H

/* includes */
#include "common_Header.h"
#include "ara/diag/uds_transport/connection.h"
#include "ara/diag/uds_transport/protocol_types.h"
#include "ara/diag/uds_transport/conversion_handler.h"
#include "diagnostic_client_conversion.h"
#include "dcm/timer/oneShotSyncTimer.h"
#include "libTimer/oneShotSync/oneShotSyncTimer.h"

namespace diag{
namespace client{
namespace conversion{

using conv_sync_timer = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer;
/*
 @ Class Name        : DmConversion
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class DmConversion : public  ::diag::client::conversion::DiagClientConversion {

public:
    // ctor
    explicit DmConversion(ara::diag::conversion_manager::ConversionIdentifierType conversion_identifier);
    
    // dtor
    ~DmConversion();
    
    // Get connection
    std::shared_ptr<ara::diag::connection::Connection> GetConversionName();

    // startup
    void Startup() override;
    
    // shutdown
    void Shutdown() override;

    // function to send Vehicle Identification Request
    void SendVehicleIdentificationRequest() override;

    // Description   : Function to connect to Diagnostic Server
    // @param input  : Nothing
    // @return value : ConnectResult
    ConnectResult ConnectToDiagServer(
            uds_message::UdsRequestMessage::IpAddress host_ip_addr) override;

    // Description   : Function to disconnect from Diagnostic Server
    // @param input  : Nothing
    // @return value : DisconnectResult
    DisconnectResult 
            DisconnectFromDiagServer() override;

    // function to send Diagnostic Activation Request
    std::pair<DiagResult, uds_message::UdsResponseMessagePtr> 
            SendDiagnosticRequest(uds_message::UdsRequestMessageConstPtr message) override;

    // Register Connection
    void RegisterConnection(std::shared_ptr<ara::diag::connection::Connection> connection);

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
                                std::vector<uint8_t> payloadInfo) ;

    // Hands over a valid message to conversion
    void HandleMessage (ara::diag::uds_transport::UdsMessagePtr message);

    // shared pointer to store the conversion handler
    std::shared_ptr<ara::diag::conversion::ConversionHandler> dm_conversion_handler;
    
private:  
    // Type for active diagnostic session
    enum class SessionControlType : uint8_t {
        kDefaultSession      = 0x01,
        kProgrammingSession  = 0x02,
        kExtendedSession     = 0x03,
        kSystemSafetySession = 0x04
    };
    
    // Type for active security level
    enum class SecurityLevelType : uint8_t {
        kLocked             = 0x00,
        kUnLocked           = 0x01,
    };
    
    // Type of current activity status 
    enum class ActivityStatusType : uint8_t {
        kActive             = 0x00,
        kInactive           = 0x01
    };

    enum class ConversionStateType : uint8_t {
        kIdle               = 0x00,
        kDiagReqSent,
        kDiagWaitForRes,
        kDiagStartP2StarTimer,
        kDiagP2MaxTimeout,
        kDiagP2StarMaxTimeout,
        kDiagRecvdPendingRes,
        kDiagRecvdFinalRes,
        kDiagSuccess,
        kDiagIncorrectRes,
    };

    // Conversion activity Status
    ActivityStatusType activity_status;
    
    // Dcm session
    SessionControlType active_session;
    
    // Dcm Security
    SecurityLevelType active_security;
    
    // Transmition buffer
    uint32_t tx_buffer_size;
    
    // Reception buffer
    uint32_t rx_buffer_size;
    
    // p2 client time
    uint16_t p2_client_max;
    
    // p2 star Client time 
    uint16_t p2_star_client_max;

    // logical Source address
    uint16_t source_address;

    // logical target address
    uint16_t target_address;

    // port number
    uint16_t port_num;

    // Vehicle broadcast address
    std::string broadcast_address;

    // remote Ip Address
    std::string remote_address;

    // state of conversion
    ConversionStateType conversion_state;

    // Tp connection
    std::shared_ptr<ara::diag::connection::Connection> connection_ptr;

    // timer
    one_shot_timer::oneShotSyncTimer timer_sync;
    
    conv_sync_timer sync_timer;

    // rx buffer to store the uds response
    ara::diag::uds_transport::ByteVector payload_rx_buffer;

    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(dm_conversion);
};

/*
 @ Class Name        : DmConversionHandler
 @ Class Description : Class to establish connection with Diagnostic Server                           
 */
class DmConversionHandler : public ara::diag::conversion::ConversionHandler {

public:
    // ctor
    explicit DmConversionHandler(ara::diag::conversion_manager::ConversionHandlerID handler_id,
                                DmConversion &dm_conversion);

    // dtor
    ~DmConversionHandler();

    // Indication of Vehicle Announcement/Identification Response
    ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult IndicateMessage(
                        std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref) override;
       
    // transmit confirmation of Vehicle Identification Request
    void TransmitConfirmation(bool result);
    
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

    // Hands over a valid message to conversion
    void HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) override;
private:
    DmConversion &dm_coversion_e;
};


} // conversion
} // client
} // diag


#endif // _DCM_CONVERSION_H
