#ifndef UDP_TYPES_H
#define UDP_TYPES_H

// includes
#include "Qt_Header.h"

namespace libOsAbstraction {
namespace libQt {
namespace libSocket {

// message type
// udp, buffer & local/host ip address
class UdpMessage
{
    public:
        // buffer type
        using buffType = std::vector<uint8_t> ;
        // ip addresstype
        using ipAddressType = std::string;
    public:
        // Receive buffer
        buffType rxBuffer;
        // Transmit buffer
        buffType txBuffer;
        // Local ipaddress
        ipAddressType localIpAddress;
        // host ipaddress
        ipAddressType hostIpAddress;
};
// unique pointer to UdpMessage
using UdpMessagePtr = std::unique_ptr<UdpMessage>;
// Udp function template used for reception
using UdpHandlerRead = std::function<void(UdpMessage )>;

} // libSocket
} // libQt
} // libOsAbstraction

#endif // UDP_TYPES_H
