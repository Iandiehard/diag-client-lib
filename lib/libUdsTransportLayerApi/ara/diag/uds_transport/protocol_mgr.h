#ifndef _PROTOCOL_MANAGER_H_
#define _PROTOCOL_MANAGER_H_


#include "uds_message.h"

namespace ara{
namespace diag{
namespace uds_transport{

class UdsTransportProtocolMgr
{
    public:
        // Type of tuple to pack UdsTransportProtocolHandlerID and ChannelID together, to form a global
        // unique (among all used UdsTransportProtocolHandlers within DM) identifier of a UdsTransport
        // Protocol channel.
        using GlobalChannelIdentifier = std::tuple<UdsTransportProtocolHandlerID, ChannelID>;
        
        // Result for Indication of message received
        enum class IndicationResult : std::uint8_t
        {
            kIndicationOk = 0,
            kIndicationOccupied,
            kIndicationOverflow,
            kIndicationUnknownTargetAddress,
            kIndicationPending,
            kIndicationNOk
        };
        
        // Result for transmission of message sent
        enum class TransmissionResult : std::uint8_t
        {
            kTransmitOk = 0,
            kTransmitFailed,
            kNoTransmitAckRevd      
        };

        // Result for connection to remote endpoint
        enum class ConnectionResult : std::uint8_t
        {
            kConnectionOk = 0,
            kConnectionFailed,
            kConnectionTimeout
        };

        // Result for disconnection to remote endpoint
        enum class DisconnectionResult : std::uint8_t
        {
            kDisconnectionOk = 0,
            kDisconnectionFailed
        };

        //ctor
        inline UdsTransportProtocolMgr(){};
        
        //dtor
        virtual ~UdsTransportProtocolMgr() = default;
        
        // initialize all the transport protocol handler
        virtual void Startup() = 0;
        
        // start all the transport protocol handler
        virtual void Run() = 0;
        
        // terminate all the transport protocol handler
        virtual void Shutdown() = 0;
};






} // uds_transport
} // diag
} // ara

#endif // _PROTOCOL_MANAGER_H_
