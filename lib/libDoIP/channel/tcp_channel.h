/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H

//includes
#include "common_Header.h"
#include "common/common_doip_types.h"
#include "sockets/tcp_socket_handler.h"
#include "libTimer/oneShotSync/oneShotSyncTimer.h"
#include "channel/tcp_channel_state_impl.h"

namespace ara{
namespace diag{
namespace doip{

// forward declaration
namespace tcpTransport{
class tcp_TransportHandler;
}

namespace tcpChannel{

// typedefs
using TcpMessage         = ara::diag::doip::tcpSocket::TcpMessage;
using TcpMessagePtr      = ara::diag::doip::tcpSocket::TcpMessagePtr;
using TcpMessageConstPtr = ara::diag::doip::tcpSocket::TcpMessageConstPtr;
using TcpChanlSyncTimer  = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer;
using TcpChannelState    = tcpChannelStateImpl::routingActivationState;

/*
 @ Class Name        : tcpChannel
 @ Class Description : Class used to handle Doip Tcp Channel                              
 @ Threads           : Per Tcp channel one threads will be spawned (one for tcp socket)
 */
class tcpChannel {
public:
    // Diagnostic state
    enum class diagnosticState: std::uint8_t {
        kIdle = 0,
        kSendDiagnosticReq,
        kSendDiagnosticReqFailed,
        kWaitForDiagnosticAck,
        kDiagnosticAckTimeout,
        kDiagnosticPositiveAckRecvd,
        kDiagnosticNegativeAckRecvd,
        kWaitForDiagnosticResponse,
        kDiagnosticFinalResRecvd
    };

    //  socket state
    enum class tcpSocketState : std::uint8_t {
        kIdle                           = 0,
        kSocketOnline,
        kSocketOffline
    };

    // diagnostic message structure
    struct diagnosticStateType {
        diagnosticState state{diagnosticState::kIdle};
        uint8_t ack_code;
    };

    //ctor
    tcpChannel(kDoip_String& localIpaddress,
                ara::diag::doip::tcpTransport::tcp_TransportHandler& tcpTransport_Handler);

    //dtor
    ~tcpChannel();

    // Initialize
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize ();

    //Start
    void Start();

    // Stop
    void Stop();

    // Function to connect to host
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
            ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message);

    // Function to disconnect from host
    ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
            DisconnectFromHost();

    // Function to Hand over all the message received
    void HandleMessage(TcpMessagePtr tcpRxMessage);

    // Function to trigger transmission
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
            Transmit(ara::diag::uds_transport::UdsMessageConstPtr message);
private:
    // Function to trigger Routing activation request
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
            SendRoutingActivationRequest(ara::diag::uds_transport::UdsMessageConstPtr& message);

    // Function to handle the routing states
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
            HandleRoutingActivationState(ara::diag::uds_transport::UdsMessageConstPtr& message);

    // Function to trigger Diagnostic Request
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
            SendDiagnosticRequest(ara::diag::uds_transport::UdsMessageConstPtr& message);

    // Function to handle the diagnostic request response state
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
            HandleDiagnosticRequestState(ara::diag::uds_transport::UdsMessageConstPtr& message);

    // Function to send Doip Generic NACK message
    bool SendDoIPNACKMessage(uint8_t nackType);

    // Function to create Generic Header
    void CreateDoIPGenericHeader(std::vector<uint8_t> &doipHeader,
                                uint16_t payloadType,
                                uint32_t payloadLen);

    // Function to process DoIP Header
    bool ProcessDoIPHeader(std::vector<uint8_t> &doipHeader, uint8_t &nackCode);

    // Function to verify payload length of various payload type
    bool ProcessDoIPPayloadLength(uint32_t payloadLen, uint16_t payloadType);

    // Function to process DoIP payload responses
    void ProcessDoIPPayload(uint16_t payloadType, std::vector<uint8_t> &payload);

    // Function to process Routing activation response
    void ProcessDoIPRoutingActivationResponse(std::vector<uint8_t> &payload);

    // Function to process Diagnostic Message Acknowledgement message
    void ProcessDoIPDiagnosticAckMessageResponse(std::vector<uint8_t> &payload, uint16_t ackType);

    // Function to process Diagnostic Message response
    void ProcessDoIPDiagnosticMessageResponse(std::vector<uint8_t> &payload);

    // Function called during General Inactivity timeout
    void TCP_GeneralInactivity_Timeout();

    // function to get payload type
    uint16_t GetDoIPPayloadType(std::vector<uint8_t> payload);
private:
    // tcp transport handler ref
    ara::diag::doip::tcpTransport::tcp_TransportHandler& tcp_transport_handler_;

    // tcp socket handler
    std::unique_ptr<ara::diag::doip::tcpSocket::tcp_SocketHandler> tcp_socket_handler_;

    // diagnostic state
    diagnosticStateType diag_state_e;

    // tcp socket state
    tcpSocketState tcpSocketState_e{tcpSocketState::kSocketOffline};

    // message request
    ara::diag::uds_transport::UdsMessageConstPtr message_e;

    // store channel number
    ara::diag::uds_transport::ChannelID channel_id_e;

    // timer
    TcpChanlSyncTimer timer_sync;

    // tcp channel state
    tcpChannelStateImpl::TcpChannelStateImpl tcp_channel_state;

    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(doip_tcp_channel);
};


} // tcpChannel
} // doip
} // diag
} // ara  

#endif // DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H
