/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H

//includes
#include "common_Header.h"
#include "common/common_doip_types.h"
#include "channel/tcp_channel_state_impl.h"
#include "common/doip_payload_type.h"
#include "sockets/tcp_socket_handler.h"


namespace ara{
namespace diag{
namespace doip{

// forward declaration
namespace tcpChannel{
class tcpChannel;
}
namespace tcpTransport{
    class tcp_TransportHandler;
}

namespace tcpChannelHandlerImpl {

using RoutingActivationChannelState = tcpChannelStateImpl::routingActivationState;
using DiagnosticMessageChannelState = tcpChannelStateImpl::diagnosticState;
using TcpMessage                    = tcpSocket::TcpMessage;
using TcpMessagePtr                 = tcpSocket::TcpMessagePtr;

/*
 @ Class Name        : RoutingActivationHandler
 @ Class Description : Class used as a handler to process routing activation messages
 */
class RoutingActivationHandler {
public:
    // ctor
    RoutingActivationHandler(tcpSocket::tcp_SocketHandler& tcp_socket_handler, tcpChannel::tcpChannel &channel) :
        tcp_socket_handler_{tcp_socket_handler},
        channel_{channel} {}

    // dtor
    ~RoutingActivationHandler() = default;

    // Function to process Routing activation response
    auto ProcessDoIPRoutingActivationResponse(DoipMessage &doip_payload) noexcept -> void;

    // Function to send Routing activation request
    auto SendRoutingActivationRequest(uds_transport::UdsMessageConstPtr& message)
        noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;
private:
    auto CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader,
                                 uint16_t payloadType,
                                 uint32_t payloadLen) noexcept -> void;

    // socket reference
    tcpSocket::tcp_SocketHandler& tcp_socket_handler_;

    // channel reference
    tcpChannel::tcpChannel &channel_;
};

/*
 @ Class Name        : DiagnosticMessageHandler
 @ Class Description : Class used as a handler to process diagnostic messages
 */
class DiagnosticMessageHandler {
public:
    // ctor
    DiagnosticMessageHandler(tcpSocket::tcp_SocketHandler& tcp_socket_handler,
                             tcpTransport::tcp_TransportHandler& tcp_transport_handler,
                             tcpChannel::tcpChannel &channel) :
        tcp_socket_handler_{tcp_socket_handler},
        tcp_transport_handler_{tcp_transport_handler},
        channel_{channel} {}

    // dtor
    ~DiagnosticMessageHandler() = default;

    // Function to process Routing activation response
    auto ProcessDoIPDiagnosticAckMessageResponse(DoipMessage &doip_payload) noexcept -> void;

    auto ProcessDoIPDiagnosticMessageResponse(DoipMessage &doip_payload) noexcept -> void;

    // Function to send Diagnostic request
    auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr& message)
        noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;
private:
    auto CreateDoipGenericHeader(std::vector<uint8_t> &doipHeader,
                                 uint16_t payloadType,
                                 uint32_t payloadLen) noexcept -> void;

    // socket reference
    tcpSocket::tcp_SocketHandler& tcp_socket_handler_;

    // transport handler reference
    tcpTransport::tcp_TransportHandler& tcp_transport_handler_;

    // channel reference
    tcpChannel::tcpChannel &channel_;
};

/*
 @ Class Name        : TcpChannelHandlerImpl
 @ Class Description : Class to handle received messages from lower layer
 */
class TcpChannelHandlerImpl {
public:
    // ctor
    TcpChannelHandlerImpl(tcpSocket::tcp_SocketHandler& tcp_socket_handler,
                          tcpTransport::tcp_TransportHandler& tcp_transport_handler,
                          tcpChannel::tcpChannel &channel) :
        routing_activation_handler_{tcp_socket_handler, channel},
        diagnostic_message_handler_{tcp_socket_handler, tcp_transport_handler, channel}
        {}

    // dtor
    ~TcpChannelHandlerImpl() = default;

    // Function to trigger Routing activation request
    auto SendRoutingActivationRequest(ara::diag::uds_transport::UdsMessageConstPtr& message)
        noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

    // Function to send Diagnostic request
    auto SendDiagnosticRequest(uds_transport::UdsMessageConstPtr& message)
        noexcept -> uds_transport::UdsTransportProtocolMgr::TransmissionResult;

    // process message
    auto HandleMessage(TcpMessagePtr tcp_rx_message) noexcept -> void;
private:
    // Function to process DoIP Header
    auto ProcessDoIPHeader(DoipMessage& doip_rx_message, uint8_t &nackCode) noexcept -> bool;

    // Function to verify payload length of various payload type
    auto ProcessDoIPPayloadLength(uint32_t payloadLen, uint16_t payloadType) noexcept -> bool;

    // Function to get payload type
    auto GetDoIPPayloadType(std::vector<uint8_t> payload) noexcept -> uint16_t;

    // Function to get payload length
    auto GetDoIPPayloadLength(std::vector<uint8_t> payload) noexcept -> uint32_t;

    // Function to process DoIP payload responses
    auto ProcessDoIPPayload(DoipMessage &doip_payload) noexcept -> void;

    // handler to process routing activation req/ resp
    RoutingActivationHandler routing_activation_handler_;

    // handler to process diagnostic message req/ resp
    DiagnosticMessageHandler diagnostic_message_handler_;
};

} // tcpChannelHandlerImpl
} // doip
} // diag
} // ara

#endif //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_HANDLER_IMPL_H
