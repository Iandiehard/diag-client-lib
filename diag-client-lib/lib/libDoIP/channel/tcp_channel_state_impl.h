/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H

#include <cstdint>
#include <vector>

#include "common/common_doip_types.h"
#include "utility/state.h"

namespace ara {
namespace diag {
namespace doip {
namespace tcpChannelStateImpl {
using namespace libUtility::state;
using namespace ara::diag::uds_transport;
// routing activation state
enum class routingActivationState : std::uint8_t {
  kIdle = 0U,
  kWaitForRoutingActivationRes,
  kRoutingActivationSuccessful,
  kRoutingActivationFailed
};
// Diagnostic state
enum class diagnosticState : std::uint8_t {
  kDiagIdle = 0U,
  kSendDiagnosticReqFailed,
  kWaitForDiagnosticAck,
  kDiagnosticPositiveAckRecvd,
  kDiagnosticNegativeAckRecvd,
  kWaitForDiagnosticResponse,
  kDiagnosticFinalResRecvd
};

class TcpChannelStateImpl {
public:
  // ctor
  TcpChannelStateImpl();

  // dtor
  ~TcpChannelStateImpl() = default;

  // Function to get the Routing Activation State context
  auto GetRoutingActivationStateContext() noexcept -> StateContext<routingActivationState> &;

  // Function to get Diagnostic Message State context
  auto GetDiagnosticMessageStateContext() noexcept -> StateContext<diagnosticState> &;

private:
  // routing activation state
  std::unique_ptr<StateContext<routingActivationState>> routing_activation_state_context_;
  // diagnostic state
  std::unique_ptr<StateContext<diagnosticState>> diagnostic_message_state_context_;
};

class kIdle final : public State<routingActivationState> {
public:
  // ctor
  kIdle(routingActivationState state);

  // dtor
  ~kIdle() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kWaitForRoutingActivationRes final : public State<routingActivationState> {
public:
  // ctor
  kWaitForRoutingActivationRes(routingActivationState state);

  // dtor
  ~kWaitForRoutingActivationRes() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kRoutingActivationSuccessful final : public State<routingActivationState> {
public:
  // ctor
  kRoutingActivationSuccessful(routingActivationState state);

  // dtor
  ~kRoutingActivationSuccessful() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kRoutingActivationFailed final : public State<routingActivationState> {
public:
  // ctor
  kRoutingActivationFailed(routingActivationState state);

  // dtor
  ~kRoutingActivationFailed() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagIdle final : public State<diagnosticState> {
public:
  // ctor
  kDiagIdle(diagnosticState state);

  // dtor
  ~kDiagIdle() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kWaitForDiagnosticAck final : public State<diagnosticState> {
public:
  // ctor
  kWaitForDiagnosticAck(diagnosticState state);

  // dtor
  ~kWaitForDiagnosticAck() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kSendDiagnosticReqFailed final : public State<diagnosticState> {
public:
  // ctor
  kSendDiagnosticReqFailed(diagnosticState state);

  // dtor
  ~kSendDiagnosticReqFailed() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagnosticPositiveAckRecvd final : public State<diagnosticState> {
public:
  // ctor
  kDiagnosticPositiveAckRecvd(diagnosticState state);

  // dtor
  ~kDiagnosticPositiveAckRecvd() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kDiagnosticNegativeAckRecvd final : public State<diagnosticState> {
public:
  // ctor
  kDiagnosticNegativeAckRecvd(diagnosticState state);

  // dtor
  ~kDiagnosticNegativeAckRecvd() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};

class kWaitForDiagnosticResponse final : public State<diagnosticState> {
public:
  // ctor
  kWaitForDiagnosticResponse(diagnosticState state);

  // dtor
  ~kWaitForDiagnosticResponse() = default;

  // start the state
  void Start() override;

  // Stop the state
  void Stop() override;

  // Handle invoked asynchronously
  void HandleMessage() override;
};
}  // namespace tcpChannelStateImpl
}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  //DIAGNOSTIC_CLIENT_LIB_LIB_LIBDOIP_CHANNEL_TCP_CHANNEL_STATE_IMPL_H
