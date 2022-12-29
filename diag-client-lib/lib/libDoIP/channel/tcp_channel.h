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
#include "libTimer/oneShotSync/one_shotsync_timer.h"
#include "channel/tcp_channel_state_impl.h"
#include "channel/tcp_channel_handler_impl.h"

namespace ara{
namespace diag{
namespace doip{

// forward declaration
namespace tcpTransport{
class TcpTransportHandler;
}

namespace tcpChannel{

// typedefs
using TcpMessage         = ara::diag::doip::tcpSocket::TcpMessage;
using TcpMessagePtr      = ara::diag::doip::tcpSocket::TcpMessagePtr;
using TcpMessageConstPtr = ara::diag::doip::tcpSocket::TcpMessageConstPtr;
using TcpRoutingActivationChannelState    = tcpChannelStateImpl::routingActivationState;
using TcpDiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;
using SyncTimer = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer;
using SyncTimerState = libOsAbstraction::libBoost::libTimer::oneShot::oneShotSyncTimer::timer_state;

/*
 @ Class Name        : tcpChannel
 @ Class Description : Class used to handle Doip Tcp Channel                              
 @ Threads           : Per Tcp channel one threads will be spawned (one for tcp socket)
 */
class tcpChannel {
public:
    //  socket state
    enum class tcpSocketState : std::uint8_t {
        kSocketOffline = 0U,
        kSocketOnline
    };

    //ctor
    tcpChannel(kDoip_String& localIpaddress,
                ara::diag::doip::tcpTransport::TcpTransportHandler& tcpTransport_Handler);

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
    void HandleMessage(TcpMessagePtr tcp_rx_message);

    // Function to trigger transmission
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
        Transmit(ara::diag::uds_transport::UdsMessageConstPtr message);

    // Function to get the channel context
    auto GetChannelState() noexcept ->
        tcpChannelStateImpl::TcpChannelStateImpl& {
        return tcp_channel_state_;
    }

    // Function to wait for response
    void WaitForResponse(std::function<void()> timeout_func,
                         std::function<void()> cancel_func,
                         int msec);

    // Function to cancel the synchronous wait
    void WaitCancel();
private:
    // Function to handle the routing states
    ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
      HandleRoutingActivationState(ara::diag::uds_transport::UdsMessageConstPtr& message);

    // Function to handle the diagnostic request response state
    ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
      HandleDiagnosticRequestState(ara::diag::uds_transport::UdsMessageConstPtr& message);
private:
    // tcp socket handler
    std::unique_ptr<ara::diag::doip::tcpSocket::TcpSocketHandler> tcp_socket_handler_;

    // tcp socket state
    tcpSocketState tcp_socket_state_{tcpSocketState::kSocketOffline};

    // tcp channel state
    tcpChannelStateImpl::TcpChannelStateImpl tcp_channel_state_;

    // tcp channel handler
    tcpChannelHandlerImpl::TcpChannelHandlerImpl tcp_channel_handler_;

    // sync timer
    SyncTimer sync_timer_;

    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(doip_tcp_channel);
};

} // tcpChannel
} // doip
} // diag
} // ara  

#endif // DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_H
