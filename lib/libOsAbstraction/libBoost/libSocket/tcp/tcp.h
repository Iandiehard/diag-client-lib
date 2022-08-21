#ifndef TCP_H
#define TCP_H

// includes
#include "tcp_Types.h"


namespace libOsAbstraction {
namespace libBoost {
namespace libSocket {
namespace tcp {

using TcpSocket         = boost::asio::ip::tcp;
using TcpIpAddress      = boost::asio::ip::address;
using TcpErrorCodeType  = boost::system::error_code;

/*
 @ Class Name        : Create Tcp Socket
 @ Class Description : Class used to create a tcp socket for handling transmission
                       and reception of tcp message from driver
*/
class createTcpSocket
{
  public:
    //ctor
    createTcpSocket(Boost_String& localIpaddress, uint16_t localportNum, TcpHandlerRead tcpHandlerRead);
    //dtor
    virtual ~createTcpSocket();
    // Function to Open the socket
    bool Open();
    // Function to Connect to host
    bool ConnectToHost(Boost_String hostIpaddress, uint16_t hostportNum);
    // Function to Disconnect from host
    bool DisconnectFromHost();
    // Function to trigger transmission
    bool Transmit(TcpMessageConstPtr tcpMessage);
    // Function to destroy the socket
    bool Destroy();
  private:
    // local Ip address
    Boost_String localIpaddress_e;
    // local port number
    uint16_t localportNum_e;
    // tcp socket
    std::unique_ptr<TcpSocket::socket> tcpSocket_e;
    // boost io context 
    boost::asio::io_context io_context;
    // flag to terminate the thread
    std::atomic_bool exit_request_e;
    // flag th start the thread
    std::atomic_bool running_e;
    // conditional variable to block the thread
    std::condition_variable cond_var_e;
    // threading var
    std::thread thread_e;
    // locking critical section   
    mutable std::mutex mutex_e;
    // Handler invoked during read operation
    TcpHandlerRead tcpHandlerRead_e;
    // Rxbuffer
    uint8_t rxbuffer_e[8];

  private:
    // function to handle read
    void HandleMessage();
    // function to handle run
    void Run();
    // function to manage DoIP header
    uint32_t GetNextBytesToReadFrmDoIPHeader(std::vector<uint8_t> doipHeader);
    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(tcp_socket_ctx);
};
} // tcp
} // libSocket
} // libBoost
} // libOsAbstraction



#endif // TCP_H
