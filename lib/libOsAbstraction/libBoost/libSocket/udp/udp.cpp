// includes
#include "udp.h"

namespace libOsAbstraction {
namespace libBoost {
namespace libSocket {
namespace udp {

// ctor
createUdpSocket::createUdpSocket(Boost_String &localIpaddress, uint16_t localportNum, createUdpSocket::PortType portType, UdpHandlerRead udpHandlerRead)
                :localIpaddress_e(localIpaddress),
                 localportNum_e(localportNum),
                 portType_e(portType),
                 udpHandlerRead_e(udpHandlerRead) {
    udpSocket_e = std::make_unique<UdpSocket::socket>(io_context);
}

// dtor
createUdpSocket::~createUdpSocket() {
}

bool createUdpSocket::Open() {
    UdpErrorCodeType ec;
    bool retVal = true;
    // Open the socket
    udpSocket_e->open(UdpSocket::v4(), ec);
    if(ec) {
        // Socket Open failed
        std::cerr << ec.message() << "\n";
        retVal = false;
    }
    else {
        // check the port type
        if(portType_e == PortType::kUdp_Broadcast) {
            // set broadcast option
            boost::asio::socket_base::broadcast broadcast_option(true);
            udpSocket_e->set_option(broadcast_option);
        }
        // reuse address
        boost::asio::socket_base::reuse_address reuseaddress_option(true);
        udpSocket_e->set_option(reuseaddress_option);
    }
    //bind to local address and random port
    udpSocket_e->bind(UdpSocket::endpoint(UdpIpAddress::from_string(localIpaddress_e), localportNum_e), ec);
    if(ec) {
        // Socket binding failed
        boost::system::errc::address_family_not_supported;
        std::cerr << ec.message() << "\n";
        retVal = false;
    }
    // start async receive
    udpSocket_e->async_receive_from(boost::asio::buffer(rxbuffer_e, kDoipUdpResSize),
                                    remote_endpoint_e, 
                                    boost::bind(&createUdpSocket::HandleMessage, this, 
                                    boost::placeholders::_1, boost::placeholders::_2));

    return retVal;
}

// function to transmit udp messages
bool createUdpSocket::Transmit(UdpMessageConstPtr udpMessage) {
    bool retVal = false;
    try {
        // Transmit to remote endpoints
        std::size_t send_size = udpSocket_e->send_to(boost::asio::buffer(udpMessage->txBuffer, std::size_t(udpMessage->txBuffer.size())), 
                                    UdpSocket::endpoint(UdpIpAddress::from_string(udpMessage->hostIpAddress), udpMessage->hostportNum));
        if(send_size == udpMessage->txBuffer.size()) {
            // successful
            retVal = true;
            // start async receive
            udpSocket_e->async_receive_from(boost::asio::buffer(rxbuffer_e, kDoipUdpResSize),
                                            remote_endpoint_e, 
                                            boost::bind(&createUdpSocket::HandleMessage, this, 
                                            boost::placeholders::_1, boost::placeholders::_2));
        }
    }
    catch (boost::system::system_error const& ec) {
        UdpErrorCodeType error = ec.code();
        std::cerr << error.message() << "\n";
    }
    return retVal;
}

// Function to destroy the socket
bool createUdpSocket::Destroy() {
    // destroy the socket
    udpSocket_e->close();
    return true;
}

// function invoked when datagram is received
void createUdpSocket::HandleMessage(const UdpErrorCodeType &error, std::size_t bytes_recvd) {
    if(error.value() == boost::system::errc::success) {
        UdpMessagePtr udpRxMessage = std::make_unique<UdpMessageType>();
        // reserve the buffer
        udpRxMessage->rxBuffer.reserve(bytes_recvd);
        // Copy the data
        for(uint8_t i = 0; i < bytes_recvd; i++)
        {
            udpRxMessage->rxBuffer[i] = rxbuffer_e[i];
        }
        // fill the remote endpoints
        udpRxMessage->hostIpAddress = remote_endpoint_e.address().to_string();
        udpRxMessage->hostportNum   = remote_endpoint_e.port();
        // send data to upper layer
        udpHandlerRead_e(std::move(udpRxMessage));
        // start next async receive
        udpSocket_e->async_receive_from(boost::asio::buffer(rxbuffer_e, kDoipUdpResSize),
                                        remote_endpoint_e, 
                                        boost::bind(&createUdpSocket::HandleMessage, this, 
                                        boost::placeholders::_1, boost::placeholders::_2));
    }
}

} // udp
} // libSocket
} // libBoost
} // libOsAbstraction
