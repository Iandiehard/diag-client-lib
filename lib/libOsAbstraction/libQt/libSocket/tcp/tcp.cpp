
// includes
#include "tcp.h"
#include "Qt_Templates.h"

namespace libOsAbstraction {
namespace libQt {
namespace libSocket {
namespace tcp {

// ctor
createTcpSocket::createTcpSocket(Qt_String &localIpaddress, uint16_t localportNum, TcpHandlerRead tcpHandlerRead)
                :localIpaddress_e(QString::fromStdString(localIpaddress)) ,
                 localportNum_e(localportNum) ,
                 tcpHandlerRead_e(tcpHandlerRead)
{
    //create socket
    tcpSocket_e = std::make_unique<QTcpSocket>();
    //bind to local address and random port
    if(!(tcpSocket_e->bind(localIpaddress_e, 0, QAbstractSocket::ShareAddress)))
    {// error
        qDebug() << " [Function :" << Q_FUNC_INFO << "]:error happennded with binding tcp address";
        return;
    }
    // for performing asynchronous calls
    connect(tcpSocket_e.get(), &QAbstractSocket::connected, this, &createTcpSocket::HostConnected);
    connect(tcpSocket_e.get(), &QAbstractSocket::disconnected, this, &createTcpSocket::HostDisconnected);
    connect(tcpSocket_e.get(), &QAbstractSocket::readyRead, this, &createTcpSocket::HandleMessage);
    qDebug() << " [Function :" << Q_FUNC_INFO << "]:socket successfully created & binded ";
}

// dtor
createTcpSocket::~createTcpSocket()
{}

// connect to host
bool createTcpSocket::ConnectToHost(std::string hostIpaddress, uint16_t hostportNum)
{
    bool retVal = true;
    QHostAddress hostIpAddress_local;
    hostIpAddress_local.setAddress(QString::fromStdString(hostIpaddress));
    //connect to provided ipAddress
    tcpSocket_e->connectToHost(hostIpAddress_local, hostportNum);
    return retVal;
}

// Disconnect from Host
bool createTcpSocket::DisconnectFromHost()
{
    bool retVal = true;
    tcpSocket_e->disconnectFromHost();
    return retVal;
}

// Function to transmit tcp messages
bool createTcpSocket::Transmit(const TcpMessage tcpMessage)
{
    QByteArray Txdata;
    bool retVal = false;
    qint64 num_byte_written = 0;
    // convert to bytearray from vector
    copyVectortoByteArray(Txdata, tcpMessage.txBuffer);
    if(tcpSocket_e->state() == QAbstractSocket::ConnectedState)
    {
        num_byte_written = tcpSocket_e->write(Txdata);
        if(num_byte_written == Txdata.size())
        {//success
            retVal = true;
        }
        else if(num_byte_written == -1)
        {// failure
        }
        else
        {//failure
        }
    }
    else
    {

    }
    return retVal;

}

// Destroy the socket
bool createTcpSocket::Destroy()
{
    //destroy the socket
    tcpSocket_e.get()->disconnect();
    //close the socket
    tcpSocket_e.reset(nullptr);
    return true;
}

// Handle reading from socket
void createTcpSocket::HandleMessage()
{
    //Data received
    if(tcpSocket_e->bytesAvailable() >= (qint64)kDoipheadrSize)
    {//atleast 8 bytes available for reading
        uint32_t readNextBytes = 0;
        QByteArray doipHeader = tcpSocket_e->read((qint64)kDoipheadrSize);
        //find the next set of bytes to be read
        readNextBytes = GetNextBytesToReadFrmDoIPHeader(doipHeader);
        if(tcpSocket_e->bytesAvailable() >= readNextBytes)
        {
            TcpMessage tcpRxMessage;
            QByteArray doipDataStream = tcpSocket_e->read((qint64)readNextBytes);
            // add the header in front
            doipDataStream.prepend(doipHeader);
            // convert to vector from bytearray
            copyByteArraytoVector(tcpRxMessage.rxBuffer, doipDataStream);
            // change the state to message received
            tcpRxMessage.tcpSocketState_e = TcpMessage::tcpSocketState::kSocketRxMessageReceived;
            // send data to upper layer
            tcpHandlerRead_e(tcpRxMessage);
        }
        else
        {
            /* do nothing */
        }
    }
}

// invoked when connected to Host machine
void createTcpSocket::HostConnected()
{
    qDebug() << " [Function :" << Q_FUNC_INFO << "]:socket successfully connected to host ";
    TcpMessage tcpRxMessage;
    // change the state to socket online
    tcpRxMessage.tcpSocketState_e = TcpMessage::tcpSocketState::kSocketOnline;
    //indication to tcp channel that socket is online
    tcpHandlerRead_e(tcpRxMessage);
}

// invoked when disconnected from host machine
void createTcpSocket::HostDisconnected()
{
    qDebug() << " [Function :" << Q_FUNC_INFO << "]:socket successfully disconnected from host ";
    TcpMessage tcpRxMessage;
    // change the state to socket online
    tcpRxMessage.tcpSocketState_e = TcpMessage::tcpSocketState::kSocketOffline;
    //indication to tcp channel that socket is offline
    tcpHandlerRead_e(tcpRxMessage);
}

// function to read next bytes to be received
uint32_t createTcpSocket::GetNextBytesToReadFrmDoIPHeader(QByteArray doipHeader)
{
    return((uint32_t)((uint32_t)((doipHeader[4] << 24) & 0xFF000000) | \
                      (uint32_t)((doipHeader[5] << 16) & 0x00FF0000) | \
                      (uint32_t)((doipHeader[6] <<  8) & 0x0000FF00) | \
                      (uint32_t)((doipHeader[7] & 0x000000FF))));
}


} // tcp
} // libSocket
} // libQt
} // libOsAbstraction
