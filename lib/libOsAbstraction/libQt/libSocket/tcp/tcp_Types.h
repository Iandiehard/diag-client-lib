#ifndef TCP_TYPES_H
#define TCP_TYPES_H

// includes
#include "Qt_Header.h"

namespace libOsAbstraction {
namespace libQt {
namespace libSocket {
namespace tcp {

// tcp messaege type
class TcpMessage
{
    public:
        enum class tcpSocketState : std::uint8_t
        {
            // Socket state
            kIdle                           = 0x00,
            kSocketOnline,
            kSocketOffline,
            kSocketRxMessageReceived,
            kSocketTxMessageSend,
            kSocketError
        };
        enum class tcpSocketError : std::uint8_t
        {
            // state
            kNone                           = 0x00
        };
        // buffer type
        using buffType = std::vector<uint8_t> ;
        // ip addresstype
        using ipAddressType = std::string;
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

// unique pointer to TcpMessage
using TcpMessagePtr = std::unique_ptr<TcpMessage>;

// Tcp function template used for reception
using TcpHandlerRead = std::function<void(TcpMessage )>;

// Doip HeaderSize
constexpr uint8_t kDoipheadrSize = 0x8;

} // tcp
} // libSocket
} // libQt
} // libOsAbstraction

#endif // TCP_TYPES_H
