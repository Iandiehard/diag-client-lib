/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "channel/tcp_channel.h"

#include "common/logger.h"
#include "handler/tcp_transport_handler.h"
#include "sockets/tcp_socket_handler.h"

namespace ara {
namespace diag {
namespace doip {
namespace tcpChannel {
tcpChannel::tcpChannel(kDoip_String &localIpaddress,
                       ara::diag::doip::tcpTransport::TcpTransportHandler &tcp_transport_handler)
    : tcp_socket_handler_{std::make_unique<ara::diag::doip::tcpSocket::TcpSocketHandler>(localIpaddress, *this)},
      tcp_socket_state_{tcpSocketState::kSocketOffline},
      tcp_channel_handler_{*(tcp_socket_handler_), tcp_transport_handler, *this} {}

tcpChannel::~tcpChannel() = default;

ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult tcpChannel::Initialize() {
  return (ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult::kInitializeOk);
}

void tcpChannel::Start() { tcp_socket_handler_->Start(); }

void tcpChannel::Stop() { tcp_socket_handler_->Stop(); }

ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult tcpChannel::ConnectToHost(
    ara::diag::uds_transport::UdsMessageConstPtr message) {
  ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult ret_val{
      ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
  if (tcp_socket_state_ == tcpSocketState::kSocketOffline) {
    // sync connect to change the socket state
    if (tcp_socket_handler_->ConnectToHost(message->GetHostIpAddress(), message->GetHostPortNumber())) {
      // set socket state, tcp connection established
      tcp_socket_state_ = tcpSocketState::kSocketOnline;
    } else {  // failure
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__, [&message](std::stringstream &msg) {
            msg << "Doip Tcp socket connect failed for remote endpoints : "
                << "<Ip: " << message->GetHostIpAddress() << ", Port: " << message->GetHostPortNumber() << ">";
          });
    }
  } else {
    // socket already online
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, __func__, [&message](std::stringstream &msg) { msg << "Doip Tcp socket already online"; });
  }
  // If socket online, send routing activation req and get response
  if (tcp_socket_state_ == tcpSocketState::kSocketOnline) {
    // send routing activation request and get response
    ret_val = HandleRoutingActivationState(message);
  }
  return ret_val;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult tcpChannel::DisconnectFromHost() {
  ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult ret_val{
      ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionFailed};
  if (tcp_socket_state_ == tcpSocketState::kSocketOnline) {
    if (tcp_socket_handler_->DisconnectFromHost()) {
      tcp_socket_state_ = tcpSocketState::kSocketOffline;
      if (tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState() ==
          TcpRoutingActivationChannelState::kRoutingActivationSuccessful) {
        tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(TcpRoutingActivationChannelState::kIdle);
        logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
            __FILE__, __LINE__, __func__,
            [](std::stringstream &msg) { msg << "RoutingActivation set to unactivated again"; });
      }
      ret_val = ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult::kDisconnectionOk;
    }
  } else {
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogDebug(
        __FILE__, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "Doip Tcp socket already in not connected state"; });
  }
  return ret_val;
}

void tcpChannel::HandleMessage(TcpMessagePtr tcp_rx_message) {
  tcp_channel_handler_.HandleMessage(std::move(tcp_rx_message));
}

ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult tcpChannel::Transmit(
    ara::diag::uds_transport::UdsMessageConstPtr message) {
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult ret_val{
      ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  if (tcp_socket_state_ == tcpSocketState::kSocketOnline) {
    // routing activation should be active before sending diag request
    if (tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState() ==
        TcpRoutingActivationChannelState::kRoutingActivationSuccessful) {
      ret_val = HandleDiagnosticRequestState(message);
    } else {
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, __func__,
          [](std::stringstream &msg) { msg << "Routing Activation required, please connect to server first"; });
    }
  } else {
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [](std::stringstream &msg) { msg << "Socket Offline, please connect to server first"; });
  }
  return ret_val;
}

void tcpChannel::WaitForResponse(std::function<void()> &&timeout_func, std::function<void()> &&cancel_func, int msec) {
  if (sync_timer_.Start(msec) == SyncTimerState::kTimeout) {
    timeout_func();
  } else {
    cancel_func();
  }
}

void tcpChannel::WaitCancel() { sync_timer_.Stop(); }

ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult tcpChannel::HandleRoutingActivationState(
    ara::diag::uds_transport::UdsMessageConstPtr &message) {
  ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult result{
      ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionFailed};
  if (tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState() ==
      TcpRoutingActivationChannelState::kIdle) {
    if (tcp_channel_handler_.SendRoutingActivationRequest(message) ==
        ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
          TcpRoutingActivationChannelState::kWaitForRoutingActivationRes);
      WaitForResponse(
          [&]() {
            result = ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionTimeout;
            tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(TcpRoutingActivationChannelState::kIdle);
            logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
                __FILE__, __LINE__, "", [](std::stringstream &msg) {
                  msg << "RoutingActivation response timeout, no response received in: "
                      << kDoIPRoutingActivationTimeout << "seconds";
                });
          },
          [&]() {
            if (tcp_channel_state_.GetRoutingActivationStateContext().GetActiveState().GetState() ==
                TcpRoutingActivationChannelState::kRoutingActivationSuccessful) {
              // success
              result = ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult::kConnectionOk;
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "RoutingActivation successful with remote server"; });
            } else {  // failed
              tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(
                  TcpRoutingActivationChannelState::kIdle);
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "RoutingActivation Failed with remote server"; });
            }
          },
          kDoIPRoutingActivationTimeout);
    } else {
      // failed, do nothing
      tcp_channel_state_.GetRoutingActivationStateContext().TransitionTo(TcpRoutingActivationChannelState::kIdle);
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, "",
          [](std::stringstream &msg) { msg << "RoutingActivation Request send failed with remote server"; });
    }
  } else {
    // channel not free
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, "", [](std::stringstream &msg) { msg << "RoutingActivation channel not free"; });
  }
  return result;
}

ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult tcpChannel::HandleDiagnosticRequestState(
    ara::diag::uds_transport::UdsMessageConstPtr &message) {
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult result{
      ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitFailed};
  if (tcp_channel_state_.GetDiagnosticMessageStateContext().GetActiveState().GetState() ==
      TcpDiagnosticMessageChannelState::kDiagIdle) {
    if (tcp_channel_handler_.SendDiagnosticRequest(message) ==
        ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk) {
      tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
          TcpDiagnosticMessageChannelState::kWaitForDiagnosticAck);
      WaitForResponse(
          [&]() {
            result = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNoTransmitAckReceived;
            tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                TcpDiagnosticMessageChannelState::kDiagIdle);
            logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
                __FILE__, __LINE__, "", [](std::stringstream &msg) {
                  msg << "Diagnostic Message Ack Request timed out, no response received in: "
                      << kDoIPDiagnosticAckTimeout << "seconds";
                });
          },
          [&]() {
            if (tcp_channel_state_.GetDiagnosticMessageStateContext().GetActiveState().GetState() ==
                tcpChannelStateImpl::diagnosticState::kDiagnosticPositiveAckRecvd) {
              // success
              result = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kTransmitOk;
              tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                  TcpDiagnosticMessageChannelState::kWaitForDiagnosticResponse);
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "Diagnostic Message Positive Ack received"; });
            } else {
              // failed with neg acknowledgement from server
              result = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kNegTransmitAckReceived;
              tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(
                  TcpDiagnosticMessageChannelState::kDiagIdle);
              logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogInfo(
                  __FILE__, __LINE__, "",
                  [](std::stringstream &msg) { msg << "Diagnostic Message Transmission Failed Neg Ack Received"; });
            }
          },
          kDoIPDiagnosticAckTimeout);
    } else {
      // failed, do nothing
      tcp_channel_state_.GetDiagnosticMessageStateContext().TransitionTo(TcpDiagnosticMessageChannelState::kDiagIdle);
      logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogError(
          __FILE__, __LINE__, "",
          [](std::stringstream &msg) { msg << "Diagnostic Request Message Transmission Failed"; });
    }
  } else {
    // channel not in idle state
    result = ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult::kBusyProcessing;
    logger::DoipClientLogger::GetDiagClientLogger().GetLogger().LogVerbose(
        __FILE__, __LINE__, "",
        [](std::stringstream &msg) { msg << "Diagnostic Message Transmission already in progress"; });
  }
  return result;
}
}  // namespace tcpChannel
}  // namespace doip
}  // namespace diag
}  // namespace ara
