
// includes
#include "udp.h"
#include "Qt_Templates.h"

namespace libOsAbstraction {
namespace libQt {
namespace libSocket {
namespace udp {

// ctor
createUdpSocket::createUdpSocket(Qt_String &localIpaddress, uint16_t portNum,
                                 createUdpSocket::PortType portType,
                                 UdpHandlerRead udpHandlerRead)
                :localIpaddress_e(QString::fromStdString(localIpaddress)),
                 localportNum_e(portNum),
                 udpHandlerRead_e(udpHandlerRead)

{
    // create socket
    udpSocket_e = std::make_unique<QUdpSocket>();
    // check the port type
    switch (portType)
    {
        case PortType::kUdp_Broadcast:
        {
            // bind the multicast socket with broadcast addrees to receive Vehicle Announcement
            if(!(udpSocket_e->bind(QHostAddress::Broadcast, localportNum_e, QAbstractSocket::ShareAddress)))
            {// error
                qDebug() << " [Function :" << Q_FUNC_INFO << "]:error happennded with binding broadcast address";
            }
        }
        break;
        case PortType::kUdp_Unicast:
        {
            // bind the unicast socket to localIpaddress to receive Vehicle Identification response
            if(!(udpSocket_e->bind(localIpaddress_e, localportNum_e, QAbstractSocket::ShareAddress)))
            {// error
                qDebug() << " [Function :" << Q_FUNC_INFO << "]:error happennded with binding unicast address";
            }
        }
        break;
        default:
            // do nothing
        break;
    }
    // connect the sockets with read function whenever datagram are reveiced
    connect(udpSocket_e.get(), &QUdpSocket::readyRead, this, &createUdpSocket::HandleMessage);
    qDebug() << " [Function :" << Q_FUNC_INFO << "]: Udp Socket started successfully ";
}

// dtor
createUdpSocket::~createUdpSocket()
{

}

// function to transmit udp messages
bool createUdpSocket::Transmit(const UdpMessage udpMessage)
{
    QNetworkDatagram datagram_e;
    QByteArray data;
    bool retVal = false;
    qint64 num_byte_written = 0;
    // convert to bytearray from vector
    copyVectortoByteArray(data, udpMessage.txBuffer);
    //fill sender ip and port / local machine
    datagram_e.setSender(localIpaddress_e,0);
    // set receiver ip address / host machine
    hostIpaddress_e.setAddress(QString::fromStdString(udpMessage.hostIpAddress));
    //fill destination ip and port /remote machine
    datagram_e.setDestination(hostIpaddress_e, localportNum_e);
    //fill data
    datagram_e.setData(data);
    // trigger send
    num_byte_written = udpSocket_e->writeDatagram(datagram_e);
    if(num_byte_written == -1)
    {
        //error in write
         qDebug() << " [Function :" << Q_FUNC_INFO << "]: Udp message sending failed ";
    }
    else
    {
        if(datagram_e.data().size() == num_byte_written)
        {// successful
            retVal = true;
        }
    }
    return retVal;
}

// Function to destroy the socket
bool createUdpSocket::Destroy()
{
    // destroy the socket
    udpSocket_e.get()->disconnect();
    udpSocket_e.reset(nullptr);
    return true;
}

// function invoked when datagram is received
void createUdpSocket::HandleMessage()
{
    while(udpSocket_e->hasPendingDatagrams())
    {
        QNetworkDatagram datagram;
        QByteArray rxdata;
        QHostAddress hostAddress;
        UdpMessage udpRxMessage;

        datagram = udpSocket_e->receiveDatagram();
        rxdata = datagram.data();
        // convert to vector from bytearray
        copyByteArraytoVector(udpRxMessage.rxBuffer, rxdata);
        // get host ip address
        hostAddress = datagram.senderAddress();
        // send the datagram to rx handler for further processing
        udpHandlerRead_e(udpRxMessage);
    }
}

} // udp
} // libSocket
} // libQt
} // libOsAbstraction
