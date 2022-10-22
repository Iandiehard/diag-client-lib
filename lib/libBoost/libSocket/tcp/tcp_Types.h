/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TCP_TYPES_H
#define TCP_TYPES_H

// includes
#include "libCommon/Boost_Header.h"
#include "common_Header.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libSocket {
namespace tcp {

// tcp messaege type
class TcpMessageType
{
  public:
      enum class tcpSocketState : std::uint8_t
      {// Socket state
          kIdle                           = 0x00,
          kSocketOnline,
          kSocketOffline,
          kSocketRxMessageReceived,
          kSocketTxMessageSend,
          kSocketTxMessageConf,
          kSocketError
      };
      enum class tcpSocketError : std::uint8_t
      {// state
          kNone                           = 0x00
      };
      // buffer type
      using buffType = std::vector<uint8_t>;
      // ip addresstype
      using ipAddressType = std::string;
      // ctor
      TcpMessageType()
      {};
      // dtor
      virtual ~TcpMessageType()
      {};
  public:
      // socket state
      tcpSocketState tcpSocketState_e{tcpSocketState::kIdle};
      // socket error
      tcpSocketError tcpSocketError_e{tcpSocketError::kNone};
      // Receive buffer
      buffType rxBuffer;
      // Transmit buffer
      buffType txBuffer;
};

// unique pointer to const TcpMessage
using TcpMessageConstPtr = std::unique_ptr<const TcpMessageType>;
// 
using TcpMessagePtr = std::unique_ptr<TcpMessageType>;
// Tcp function template used for reception
using TcpHandlerRead = std::function<void(TcpMessagePtr )>;
// Doip HeaderSize
constexpr uint8_t kDoipheadrSize = 0x8;

} // tcp
} // libSocket
} // libBoost
} // libOsAbstraction

#endif // TCP_TYPES_H
