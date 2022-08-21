#ifndef UDP_H
#define UDP_H

// includes
#include "udp_Types.h"

namespace libOsAbstraction {
namespace libQt {
namespace libSocket {
namespace udp {

/*
 @ Class Name        : Create Udp Socket
 @ Class Description : Class used to create a udp socket for handling transmission
                       and reception of udp message from driver
*/

class createUdpSocket : public QObject
{
    public:
        // Port Type
        enum class PortType : std::uint8_t
        {
            kUdp_Broadcast = 0x00,
            kUdp_Unicast   = 0x01
        };
        // ctor
        createUdpSocket(Qt_String& localIpaddress, uint16_t portNum, PortType portType, UdpHandlerRead udpHandlerRead);
        // dtor
        virtual ~createUdpSocket();
        // Transmit
        bool Transmit(const UdpMessage udpMessage);
        // Function to destroy the socket
        bool Destroy();
    private:
        // local Ip address
        QHostAddress localIpaddress_e;
        // Host Ip address
        QHostAddress hostIpaddress_e;
        // local port number
        uint16_t localportNum_e;
        // udp socket
        std::unique_ptr<QUdpSocket> udpSocket_e;
        // Handler invoked during read operation
        UdpHandlerRead udpHandlerRead_e;
    private:
        // function to handle read
        void HandleMessage();
};

} // udp
} // libSocket
} // libQt
} // libOsAbstraction

#endif // UDP_H
