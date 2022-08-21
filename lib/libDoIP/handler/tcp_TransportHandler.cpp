
#include "handler/tcp_TransportHandler.h"
#include "connection/connection_Manager.h"

namespace ara{
namespace diag{
namespace doip{
namespace tcpTransport{


// ctor
tcp_TransportHandler::tcp_TransportHandler(kDoip_String &localIpaddress, uint16_t portNum, uint8_t total_tcpChannelReq,
                      connection::DoipConnection& doipConnection)
                    : doipConnection_e(doipConnection),
                      tcp_channel(std::make_unique<ara::diag::doip::tcpChannel::tcpChannel>(localIpaddress, *this)) {
}

// dtor
tcp_TransportHandler::~tcp_TransportHandler() {
}

// Initialize
ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult tcp_TransportHandler::Initialize() {
    return (tcp_channel->Initialize());
}

// start handler
void tcp_TransportHandler::Start() {
    tcp_channel->Start();
}

// stop handler
void tcp_TransportHandler::Stop() {
    tcp_channel->Stop();
}

// Connect to remote Host
ara::diag::uds_transport::UdsTransportProtocolMgr::ConnectionResult
        tcp_TransportHandler::ConnectToHost(ara::diag::uds_transport::UdsMessageConstPtr message) {
    return(tcp_channel->ConnectToHost(std::move(message)));
}

// Disconnect from remote host
ara::diag::uds_transport::UdsTransportProtocolMgr::DisconnectionResult
        tcp_TransportHandler::DisconnectFromHost() {
    return(tcp_channel->DisconnectFromHost());
}

// Transmit 
ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult 
        tcp_TransportHandler::Transmit(ara::diag::uds_transport::UdsMessageConstPtr message, 
                                            ara::diag::uds_transport::ChannelID channel_id) {
    // find the corresponding channel

    // Trigger transmit
    return(tcp_channel->Transmit(std::move(message)));
}

// Indicate message Diagnostic message reception over TCP to user
std::pair<ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult, ara::diag::uds_transport::UdsMessagePtr>
                  tcp_TransportHandler::IndicateMessage(ara::diag::uds_transport::UdsMessage::Address source_addr,
                                                        ara::diag::uds_transport::UdsMessage::Address target_addr,
                                                        ara::diag::uds_transport::UdsMessage::TargetAddressType type,
                                                        ara::diag::uds_transport::ChannelID channel_id,
                                                        std::size_t size, ara::diag::uds_transport::Priority priority,
                                                        ara::diag::uds_transport::ProtocolKind protocol_kind,
                                                        std::vector<uint8_t> payloadInfo) {
    return (doipConnection_e.IndicateMessage(source_addr, 
                                            target_addr, 
                                            type, 
                                            channel_id, 
                                            size,
                                            priority, 
                                            protocol_kind, 
                                            payloadInfo));
}

// Hands over a valid received Uds message (currently this is only a request type) from transport
// layer to session layer                
void tcp_TransportHandler::HandleMessage (ara::diag::uds_transport::UdsMessagePtr message) {
    doipConnection_e.HandleMessage(std::move(message));
}

} // tcpTransport
} // doip
} // diag
} // ara  
