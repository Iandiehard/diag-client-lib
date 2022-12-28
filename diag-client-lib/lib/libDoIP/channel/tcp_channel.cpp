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

tcpChannel::tcpChannel(kDoip_String& localIpaddress,
                        ara::diag::doip::tcpTransport::TcpTransportHandler& tcp_transport_handler)
           :tcp_socket_handler_{std::make_unique<ara::diag::doip::tcpSocket::TcpSocketHandler>(localIpaddress, *this)},
            tcp_socket_state_{tcpSocketState::kSocketOffline},
            tcp_channel_handler_{*(tcp_socket_handler_.get()), tcp_transport_handler, *this} {
    DLT_REGISTER_CONTEXT(doip_tcp_channel,"dtcp","DoipClient Tcp Channel Context");
}

tcpChannel::~tcpChannel() {
    DLT_UNREGISTER_CONTEXT(doip_tcp_channel);
}

ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult
        tcpChannel::Initialize() {
    return (ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk);
}

void tcpChannel::Start() {
    tcp_socket_handler_->Start();
}

void tcpChannel::Stop() {
    tcp_socket_handler_->Stop();
}

ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
    tcpChannel::ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) {
    
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
        ret_val{ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
    
    if(tcp_socket_state_ == tcpSocketState::kSocketOffline) {
        // sync connect to change the socket state
        if(tcp_socket_handler_->ConnectToHost(message->GetHostIpAddress()
                                                ,message->GetHostPortNumber())) {
            // set socket state, tcp connection established
            tcp_socket_state_ = tcpSocketState::kSocketOnline;
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
    if(tcp_socket_state_ == tcpSocketState::kSocketOnline) {
        // send routing activation request and get response
        ret_val = HandleRoutingActivationState(message);
    }
    return ret_val;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
    tcpChannel::DisconnectFromHost() {
    ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
        ret_val{ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed};
    if(tcp_socket_state_ == tcpSocketState::kSocketOnline) {
        if(tcp_socket_handler_->DisconnectFromHost()) {
            tcp_socket_state_ = tcpSocketState::kSocketOffline;
            if(tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState()
                == TcpRoutingActivationChannelState ::kRoutingActivationSuccessful) {
                tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(TcpRoutingActivationChannelState::kIdle);;
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

void tcpChannel::HandleMessage(TcpMessagePtr tcpRxMessage) {
    tcp_channel_handler_.HandleMessage(std::move(tcpRxMessage));
}

ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
    tcpChannel::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message) {

    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult retval {
        ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};

    if(tcp_socket_state_ == tcpSocketState::kSocketOnline) {
        // routing activation should be active before sending diag request
        if(tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState() ==
                TcpRoutingActivationChannelState::kRoutingActivationSuccessful) {
            retval = HandleDiagnosticRequestState(message);
        }
        else {
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

void tcpChannel::WaitForResponse(
    std::function<void()> timeout_func,
    std::function<void()> cancel_func,
    int msec) {
    if(sync_timer_.Start(msec) == SyncTimerState::kTimeout) {
        timeout_func();
    }
    else {
        cancel_func();
    }
}

void tcpChannel::WaitCancel() {
    sync_timer_.Stop();
}

ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
    tcpChannel::HandleRoutingActivationState(ara::diag::uds_transport::UdsMessageConstPtr& message) {
    
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult 
            result{ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
    
    if(tcp_channel_state_
        .GetRoutingActivationStateContext()
        .GetActiveState()
        .GetState() == TcpRoutingActivationChannelState::kIdle) {

        if(tcp_channel_handler_.SendRoutingActivationRequest(message) ==
            ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {

            tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                    TcpRoutingActivationChannelState::kWaitForRoutingActivationRes);
            WaitForResponse(
                [&]() {
                    result = ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionTimeout;
                    tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                            TcpRoutingActivationChannelState::kIdle);
                    DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR,
                            DLT_CSTRING("RoutingActivation request timeout,no response received"));
                    },
                [&]() {
                    if(tcp_channel_state_
                            .GetRoutingActivationStateContext()
                            .GetActiveState()
                            .GetState() == TcpRoutingActivationChannelState::kRoutingActivationSuccessful) {
                        // success
                        result = ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionOk;
                        DLT_LOG(doip_tcp_channel, DLT_LOG_DEBUG,
                                DLT_CSTRING("RoutingActivation activated successfully"));
                    }
                    else {
                        // failed
                        tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                                TcpRoutingActivationChannelState::kIdle);
                        DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR,
                                DLT_CSTRING("RoutingActivation Failed"));
                    }
                },
                kDoIPRoutingActivationTimeout);
        }
        else {
            // failed, do nothing
            tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                    TcpRoutingActivationChannelState::kIdle);
            DLT_LOG(doip_tcp_channel, DLT_LOG_ERROR,
                    DLT_CSTRING("RoutingActivation Failed"));
        }
    }
    else {
        // channel not free
    }
    return result;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
    tcpChannel::HandleDiagnosticRequestState(ara::diag::uds_transport::UdsMessageConstPtr& message) {

    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
        result{ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};

    if(tcp_channel_state_
        .GetDiagnosticMessageStateContext()
        .GetActiveState()
        .GetState() == TcpDiagnosticMessageChannelState::kDiagIdle) {

        if(tcp_channel_handler_.SendDiagnosticRequest(message) ==
            ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {

            tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                        TcpDiagnosticMessageChannelState::kWaitForDiagnosticAck);
            WaitForResponse(
                [&]() {
                    result = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNoTransmitAckRevd;
                    tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                            TcpDiagnosticMessageChannelState::kDiagIdle);
                    DLT_LOG(doip_tcp_channel, DLT_LOG_WARN,
                            DLT_CSTRING("Diagnostic Message Ack Request timed out"));
                },
                [&]() {
                    if(tcp_channel_state_
                        .GetDiagnosticMessageStateContext()
                        .GetActiveState()
                        .GetState() == tcpChannelStateImpl::diagnosticState::kDiagnosticPositiveAckRecvd) {
                        // success
                        result = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
                        tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                                TcpDiagnosticMessageChannelState::kWaitForDiagnosticResponse);
                        DLT_LOG(doip_tcp_channel, DLT_LOG_DEBUG,
                                DLT_CSTRING("Diagnostic Message Positive Ack received"));
                    }
                    else {
                        // failed
                        tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                                TcpDiagnosticMessageChannelState::kDiagIdle);
                        DLT_LOG(doip_tcp_channel, DLT_LOG_WARN,
                                DLT_CSTRING("Diagnostic Message Transmission Failed Neg Ack Received"));
                    }
                },
                kDoIPDiagnosticAckTimeout);
        }
        else {
            // failed, do nothing
            tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                    TcpDiagnosticMessageChannelState::kDiagIdle);
            DLT_LOG(doip_tcp_channel, DLT_LOG_WARN,
                    DLT_CSTRING("Diagnostic Message Transmission Failed"));
        }
    }
    else {
        // channel not in idle state
        DLT_LOG(doip_tcp_channel, DLT_LOG_WARN, 
            DLT_CSTRING("Diagnostic Message Transmission already in progress"));
    }

    return result;
}

} // tcpChannel
} // doip
} // diag
} // ara
