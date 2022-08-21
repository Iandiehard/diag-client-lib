#ifndef TCP_H
#define TCP_H

// includes
#include "tcp_Types.h"


namespace libOsAbstraction {
namespace libQt {
namespace libSocket {
namespace tcp {


/*
 @ Class Name        : Create Tcp Socket
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver
*/
class createTcpSocket : public QObject
{
public:
  //ctor
  createTcpSocket(Qt_String& localIpaddress, uint16_t localportNum, TcpHandlerRead tcpHandlerRead);
  //dtor
  virtual ~createTcpSocket();
  // Function to Connect to host
  bool ConnectToHost(Qt_String hostIpaddress, uint16_t hostportNum);
  // Function to Disconnect from host
  bool DisconnectFromHost();
  // Function to trigger transmission
  bool Transmit(const TcpMessage tcpMessage);
  // Function to destroy the socket
  bool Destroy();
private:
  // local Ip address
  QHostAddress localIpaddress_e;
  // local port number
  uint16_t localportNum_e;
  // tcp socket
  std::unique_ptr<QTcpSocket> tcpSocket_e;
  // Handler invoked during read operation
  TcpHandlerRead tcpHandlerRead_e;
private:
  // function to handle read
  void HandleMessage();
  // function invoked when host is connected to client
  void HostConnected();
  // function invoked when host is disconnected from client
  void HostDisconnected();
private:
  // function to manage DoIP header
  inline uint32_t GetNextBytesToReadFrmDoIPHeader(QByteArray doipHeader);
};
} // tcp
} // libSocket
} // libQt
} // libOsAbstraction



#endif // TCP_H
