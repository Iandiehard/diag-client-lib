/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "channel/tcp_channel.h"
#include "sockets/tcp_socket_handler.h"
#include "handler/tcp_transport_handler.h"

namespace ara{
namespace diag{
namespace doip{
namespace tcpChannel{

/**
 @ Class Name        : tcpChannel
 @ Class Description : Class used to handle Doip Tcp Channel
 **/

// Description   : Constructor
// @param input  : localIpaddress, hostportNum, tcpTransport_Handler
// @return value : void
tcpChannel::tcpChannel(kDoip_String& localIpaddress,
                        ara::diag::doip::tcpTransport::tcp_TransportHandler& tcpTransport_Handler)
           :tcp_transport_handler_(tcpTransport_Handler),
            tcp_socket_handler_(std::make_unique<ara::diag::doip::tcpSocket::tcp_SocketHandler>(localIpaddress, *this)),
            channel_id_e(0),
            tcp_channel_handler_{*(tcp_socket_handler_.get()), *this} {
    DLT_REGISTER_CONTEXT(doip_tcp_channel,"dtcp","DoipClient Tcp Channel Context");
}

// Description   : Destructor
// @param input  : void
// @return value : void
tcpChannel::~tcpChannel() {
    DLT_UNREGISTER_CONTEXT(doip_tcp_channel);
}

// Description   : Function to initialize the channel
// @param input  : void
// @return value : void
ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult
        tcpChannel::Initialize() {
    return (ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk);
}

// Description   : Function to start the channel
// @param input  : void
// @return value : void
void tcpChannel::Start() {
    tcp_socket_handler_->Start();
}

// Description   : Function to terminate the channel
// @param input  : void
// @return value : void
void tcpChannel::Stop() {
    tcp_socket_handler_->Stop();
}

// Function to connect to host
ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult    
    tcpChannel::ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) {
    
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
        ret_val{ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
    
    if(tcpSocketState_e == tcpSocketState::kSocketOffline) {
        // sync connect to change the socket state
        if(tcp_socket_handler_->ConnectToHost(message->GetHostIpAddress()
                                                ,message->GetHostPortNumber())) {
            // set socket state, tcp connection established
            tcpSocketState_e = tcpSocketState::kSocketOnline;
        }
        else {// failure
            DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR, 
                DLT_CSTRING("Doip tcp socket connect failed"));
        }
    }
    else {
        // socket already online
        DLT_LOG(doip_tcp_channel, DLT_LOG_WARN, 
            DLT_CSTRING("Doip tcp socket already online"));
    }
    
    // If socket online, send routing activation req and get response
    if(tcpSocketState_e == tcpSocketState::kSocketOnline) {
        // send routing activation request and get response
        ret_val = HandleRoutingActivationState(message);
    }
    return ret_val;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
    tcpChannel::DisconnectFromHost() {
    ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
        ret_val{ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed};
    if(tcpSocketState_e == tcpSocketState::kSocketOnline) {
        if(tcp_socket_handler_->DisconnectFromHost()) {
            tcpSocketState_e = tcpSocketState::kSocketOffline;
            if(tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState()
                == TcpChannelState ::kRoutingActivationSuccessful) {
                tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(TcpChannelState::kIdle);;
                DLT_LOG(doip_tcp_channel, DLT_LOG_INFO, 
                    DLT_CSTRING("RoutingActivation activated reseted "));
            }
            ret_val = ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionOk;
        }
    }
    else {
        DLT_LOG(doip_tcp_channel, DLT_LOG_WARN, 
            DLT_CSTRING("Doip tcp socket already offline"));  
    }
    return ret_val;
}

// Description   : Function to Hand over all the message received
// @param input  : TcpMessagePtr tcpRxMessage
// @return value : void
void tcpChannel::HandleMessage(TcpMessagePtr tcpRxMessage) {
    tcp_channel_handler_.HandleMessage(std::move(tcpRxMessage));
}

// Description   : Function to trigger diagnostic message request transmission
// @param input  : TcpMessage tcpTxMessage
// @return value : void
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
    tcpChannel::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) {

    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult retval {
        ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};

    if(tcpSocketState_e == tcpSocketState::kSocketOnline) {
        if(tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState() ==
                TcpChannelState::kRoutingActivationSuccessful) {
            retval = HandleDiagnosticRequestState(message);
        }
        else {
            // do nothing
            diag_state_e.state = diagnosticState::kIdle;
            DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR, 
                DLT_CSTRING("Routing Activation required, please connect to Server First"));
        }
    }
    else {
        // do nothing
        DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR, 
            DLT_CSTRING("Socket Offline, please connect to Server First"));
    }
    return retval;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
        tcpChannel::HandleRoutingActivationState(ara::diag::uds_transport::UdsMessageConstPtr& message) {
    
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult 
            result{ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
    
    if(tcp_channel_state_.
        GetRoutingActivationStateContext().GetActiveState().GetState() == TcpChannelState::kIdle) {
        if(tcp_channel_handler_.SendRoutingActivationRequest(message) ==
           ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
            tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                    TcpChannelState::kWaitForRoutingActivationRes);
            // here a sync wait will be performed until timeout or response received
        }
        else {
            // failed, do nothing
            tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                    TcpChannelState::kRoutingActivationFailed);
        }

        // check the state
        switch(tcp_channel_state_
            .GetRoutingActivationStateContext()
            .GetActiveState()
            .GetState()) {
            case TcpChannelState::kWaitForRoutingActivationRes : {
                // timeout happened
                result = ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionTimeout;
                tcp_channel_state_.
                    GetRoutingActivationStateContext().TransitionTo(TcpChannelState::kIdle);
                DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR,
                        DLT_CSTRING("RoutingActivation request timeout,no response received"));
            }
            break;

            case TcpChannelState::kRoutingActivationSuccessful : {
                // success
                result =
                        ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionOk;
                DLT_LOG(doip_tcp_channel, DLT_LOG_DEBUG,
                        DLT_CSTRING("RoutingActivation activated successfully"));
            }
            break;

            default:
                // failed
                tcp_channel_state_
                    .GetRoutingActivationStateContext().TransitionTo(TcpChannelState::kIdle);
                DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR,
                        DLT_CSTRING("RoutingActivation Failed"));
            break;
        }
    }
    else {
        // channel not free
    }
    return result;
}

// Description   : Function to trigger Diagnostic request
// @param input  : void
// @return value : void
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
    tcpChannel::SendDiagnosticRequest(ara::diag::uds_transport::UdsMessageConstPtr& message) {

    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
        retval{ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
    
    TcpMessagePtr doipDiagReq = std::make_unique<TcpMessage>();
    // reserve bytes in vector
    doipDiagReq->txBuffer.reserve(kDoipheadrSize + kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
    // create header
    CreateDoIPGenericHeader(doipDiagReq->txBuffer, 
                            kDoip_DiagMessage_Type, 
                            kDoip_DiagMessage_ReqResMinLen + message->GetPayload().size());
    // Add source address
    doipDiagReq->txBuffer.push_back((uint8_t)((message->GetSa() & 0xFF00) >> 8));
    doipDiagReq->txBuffer.push_back((uint8_t)(message->GetSa() & 0x00FF));
    // Add target address
    doipDiagReq->txBuffer.push_back((uint8_t)((message->GetTa() & 0xFF00) >> 8));
    doipDiagReq->txBuffer.push_back((uint8_t)(message->GetTa() & 0x00FF));
    // Add data bytes
    for (std::size_t i = 0; i < message->GetPayload().size(); i ++) {
        doipDiagReq->txBuffer.push_back(message->GetPayload().at(i));
    }

    // transmit
    if(!(tcp_socket_handler_->Transmit(std::move(doipDiagReq)))){
        DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR, 
            DLT_CSTRING("Diagnostic Request sending failed"));
    }
    else {
        retval = 
            ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
        DLT_LOG(doip_tcp_channel, DLT_LOG_INFO, 
            DLT_CSTRING("Diagnostic Request Sent:"),
            DLT_HEX16(message->GetSa()),
            DLT_CSTRING("->"),
            DLT_HEX16(message->GetTa()));
    }
    return retval;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
    tcpChannel::HandleDiagnosticRequestState(ara::diag::uds_transport::UdsMessageConstPtr& message) {

    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
        retval{ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};

    if(diag_state_e.state == diagnosticState::kIdle){
        // send diagnostic request
        diag_state_e.state = diagnosticState::kSendDiagnosticReq;
        while (diag_state_e.state != diagnosticState::kIdle) {
            if(diag_state_e.state == diagnosticState::kSendDiagnosticReq) {
                // Initiate diagnostic request request
                if(SendDiagnosticRequest(message) == 
                    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
                    diag_state_e.state = diagnosticState::kWaitForDiagnosticAck;                
                }
                else {
                    // failed, do nothing
                    diag_state_e.state = diagnosticState::kSendDiagnosticReqFailed;
                }
            }
            else if(diag_state_e.state == diagnosticState::kWaitForDiagnosticAck) {
                // wait for diagnostic acknowledgement till kDoIPDiagnosticAckTimeout
                //if(timer_sync.Start(kDoIPDiagnosticAckTimeout) ==
                //         TcpChanlSyncTimer::timer_state::kTimeout)
                timer_sync.Start(kDoIPDiagnosticAckTimeout);
                {
                    // no diagnostic ack received
                    diag_state_e.state = diagnosticState::kDiagnosticAckTimeout;
                    DLT_LOG(doip_tcp_channel, DLT_LOG_WARN, 
                        DLT_CSTRING("Diagnostic Message Ack timed out"));
                }
                //else
                {
                    // response received, 
                    // state kDiagnosticPositiveAckRecvd, kDiagnosticNegativeAckRecvd 
                    // will be handled in callback
                }
                break;
            }
            else {
                // kSendDiagnosticReqFailed
                // kDiagnosticAckTimeout
                break;
            }            
        }

        if(diag_state_e.state == diagnosticState::kDiagnosticPositiveAckRecvd) {
            // wait for Diagnostic response
            diag_state_e.state = diagnosticState::kWaitForDiagnosticResponse;
            retval = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
            DLT_LOG(doip_tcp_channel, DLT_LOG_DEBUG, 
                DLT_CSTRING("Diagnostic Message Positive Ack received"));
        }
        else {
            // change to idle, as diagnostic req was not acknowleged or sending failed
            diag_state_e.state = diagnosticState::kIdle;
            DLT_LOG(doip_tcp_channel, DLT_LOG_WARN, 
                DLT_CSTRING("Diagnostic Message Transmission Failed"));
        }
    }
    else {
        // channel not in idle state
        DLT_LOG(doip_tcp_channel, DLT_LOG_WARN, 
            DLT_CSTRING("Diagnostic Message Transmission already in progress"));
    }

    return retval;
}

// Description   : Function to create Generic Header
// @param input  : doipHeader, payloadType, payloadLen
// @return value : void
void tcpChannel::CreateDoIPGenericHeader(std::vector<uint8_t> &doipHeader,
                                         uint16_t payloadType, 
                                         uint32_t payloadLen) {
    doipHeader.push_back(kDoip_ProtocolVersion);
    doipHeader.push_back(~((uint8_t)kDoip_ProtocolVersion));
    doipHeader.push_back((uint8_t)((payloadType & 0xFF00) >> 8));
    doipHeader.push_back((uint8_t)(payloadType & 0x00FF));
    doipHeader.push_back((uint8_t)((payloadLen & 0xFF000000) >> 24));
    doipHeader.push_back((uint8_t)((payloadLen & 0x00FF0000) >> 16));
    doipHeader.push_back((uint8_t)((payloadLen & 0x0000FF00) >> 8));
    doipHeader.push_back((uint8_t)(payloadLen & 0x000000FF));
}

// Description   : Function to send NACK
// @param input  : nackType
// @return value : boolean true/false
bool tcpChannel::SendDoIPNACKMessage(uint8_t nackType)
{
    bool retval = true;
    TcpMessagePtr doipNackBuff = std::make_unique<TcpMessage>();
    //reserve 9 bytes in vector
    doipNackBuff->txBuffer.reserve(kDoipheadrSize + kDoip_GenericHeader_NackLen);
    // create header
    CreateDoIPGenericHeader(doipNackBuff->txBuffer, kDoip_GenericHeadr_NackType, kDoip_GenericHeader_NackLen);
    // fill the nack code
    doipNackBuff->txBuffer.push_back(nackType);
    // transmit
    if(!(tcp_socket_handler_->Transmit(std::move(doipNackBuff))))
    {// error
        retval = false;
    }
    return retval;
}

// Description   : Function to process Diag Ack Message
// @param input  : void
// @return value : void
void tcpChannel::ProcessDoIPDiagnosticAckMessageResponse(std::vector<uint8_t> &payload, uint16_t ackType) {
    if(diag_state_e.state == diagnosticState::kWaitForDiagnosticAck) {
        // check the logical address of Server
        uint16_t server_address = (uint16_t)(((payload[BYTE_POS_ZERO] & 0xFF) << 8) | 
                                    (payload[BYTE_POS_ONE] & 0xFF));
        // check the logical address of client
        uint16_t client_address = (uint16_t)(((payload[BYTE_POS_TWO] & 0xFF) << 8) | 
                                    (payload[BYTE_POS_THREE] & 0xFF));
        // get the ack code
        uint8_t ack_code    = payload[BYTE_POS_FOUR];
        // check ack type and look for positive :D
        if(ackType == kDoip_DiagMessagePosAck_Type)  {
            // Check for ack code
            if(ack_code == kDoip_DiagnosticMessage_PosAckCode_Confirm) {
                // wait for Diag Response 
                diag_state_e.state = diagnosticState::kDiagnosticPositiveAckRecvd;
                DLT_LOG(doip_tcp_channel, DLT_LOG_INFO, 
                    DLT_CSTRING("Diagnostic Message Pos ACK received"),
                    DLT_HEX16(server_address),
                    DLT_CSTRING("->"),
                    DLT_HEX16(client_address));
            }
            diag_state_e.ack_code = ack_code;
        }
        else if(ackType == kDoip_DiagMessageNegAck_Type) {
            // Send Tx Confirmation to Upper Layer about failed transmission acknowledgement of request
            diag_state_e.state = diagnosticState::kDiagnosticNegativeAckRecvd;
            diag_state_e.ack_code = ack_code;
            DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR, 
                DLT_CSTRING("Diagnostic Message NACK received: "), 
                DLT_HEX8(ack_code),
                DLT_CSTRING(","),
                DLT_HEX16(server_address),
                DLT_CSTRING("->"),
                DLT_HEX16(client_address));
        }
        else {
            // do nothing            
        }
        timer_sync.Stop();
    }
    else {
        // ignore
    }
}

// Description   : Function to process Diag Response Message
// @param input  : void
// @return value : void
void tcpChannel::ProcessDoIPDiagnosticMessageResponse(std::vector<uint8_t> &payload) {
    if(diag_state_e.state == diagnosticState::kWaitForDiagnosticResponse) {
        // create the payload to send to upper layer
        std::vector<uint8_t> payloadinfo;
        // check the logical address of Server
        uint16_t server_address = (uint16_t)(((payload[BYTE_POS_ZERO] & 0xFF) << 8) | 
                                (payload[BYTE_POS_ONE] & 0xFF));
        // check the logical address of client
        uint16_t client_address = (uint16_t)(((payload[BYTE_POS_TWO] & 0xFF) << 8) | 
                                (payload[BYTE_POS_THREE] & 0xFF));
        // payload except the address
        payloadinfo.resize(payload.size() - 4);
        // copy to application buffer
        std::copy(payload.begin() + 4, payload.end(), payloadinfo.begin());
        // Indicate upper layer about incoming data
        std::pair<uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr> 
                retval = tcp_transport_handler_.IndicateMessage(ara::diag::uds_transport::UdsMessage::Address(server_address),
                                                               ara::diag::uds_transport::UdsMessage::Address(client_address),
                                                               ara::diag::uds_transport::UdsMessage::TargetAddressType::kPhysical,
                                                               channel_id_e, 
                                                               std::size_t(payload.size() - 4), 
                                                               0, 
                                                               "DoIP", 
                                                               payloadinfo);

        // Check result and udsMessagePtr
        if((retval.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationOk) &&
            (retval.second != nullptr)) {
            diag_state_e.state = diagnosticState::kDiagnosticFinalResRecvd;
            // copy to application buffer
            std::copy(payloadinfo.begin(), payloadinfo.end(), retval.second->GetPayload().begin());
            tcp_transport_handler_.HandleMessage(std::move(retval.second));
            DLT_LOG(doip_tcp_channel, DLT_LOG_DEBUG, 
                DLT_CSTRING("Diagnostic Message response reception completed"));
            // make channel idle, since reception complete
            diag_state_e.state = diagnosticState::kIdle;
        }
        else if(retval.first == uds_transport::UdsTransportProtocolMgr::IndicationResult::kIndicationPending) {
            // keep channel alive since pending request received, do not change channel state
            DLT_LOG(doip_tcp_channel, DLT_LOG_DEBUG, 
                DLT_CSTRING("Diagnostic Message Pending response indicated to Convrsn"));
        }
        else { // other errors
            diag_state_e.state = diagnosticState::kIdle;
            DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR, 
                DLT_CSTRING("Undefined Error occured in Conversion"));
        }
    }
    else {
        // ignore
    }
}

// Description   : Function called during General Inactivity timeout
// @param input  : void
// @return value : void
void tcpChannel::TCP_GeneralInactivity_Timeout() {
    // disconnect from host
    tcp_socket_handler_->DisconnectFromHost();
    //tcpChannelState_e = tcpChannelState::kIdle;
    tcpSocketState_e  = tcpSocketState::kSocketOffline;
}

} // tcpChannel
} // doip
} // diag
} // ara
