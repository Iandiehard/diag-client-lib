#ifndef _UDP_CHANNEL_H_
#define _UDP_CHANNEL_H_

//includes
#include "common/common_doIP_Types.h"
#include "sockets/udp_SocketHandler.h"
#include "libTimer/oneShotAsync/oneShotAsyncTimer.h"

namespace ara{
namespace diag{
namespace doip{

// Forward declaration
namespace udpTransport {
class udp_TransportHandler;
}

namespace udpChannel{

//typedefs
using UdpMessage    = ara::diag::doip::udpSocket::UdpMessage;
using UdpMessagePtr = ara::diag::doip::udpSocket::UdpMessagePtr;
using udpChanlTimer = libOsAbstraction::libBoost::libTimer::oneShot::oneShotAsyncTimer;

/*
 @ Class Name        : udpChannel
 @ Class Description : Class used to handle Doip Udp Channel
 */
class udpChannel
{
    public:
        enum class udpChannelState : std::uint8_t
        {
            kIdle = 0,
            kWaitforVehicleAnnouncement,
            kSendVehicleIdentificationReq,
            kWaitforVehicleIdentificationRes,
            kA_DoIP_Ctrl_Timeout
        };
        //ctor
        udpChannel(kDoip_String& localIpaddress, uint16_t portNum, ara::diag::doip::udpTransport::udp_TransportHandler& udpTransport_Handler);
        //dtor
        ~udpChannel();
        // Initialize
        ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize ();
        //Start
        void Start();
        // Stop
        void Stop();
        // function to handle read broadcast
        void HandleMessageBroadcast(UdpMessagePtr udpRxMessage);
        // function to handle read unicast
        void HandleMessageUnicast(UdpMessagePtr udpRxMessage);
        // Function to indicate a message start
        void IndicateMessage(UdpMessage& udpRxMessage);
        // Function to notify failure
        void NotifyMessageFailure();
        // Function to trigger transmission
        bool Transmit(ara::diag::doip::VehicleInfo &vehicleInfo_Ref);
        // Function to confirm transmission
        void TransmitConfirmation(bool result);
    private:
        // Function to trigger Vehicle Identification req
        Std_ReturnType SendVehicleIdentificationRequest(ara::diag::doip::VehicleInfo &vehicleInfo_Ref);
        // Function to trigger Vehicle Identification req with EID
        Std_ReturnType SendVehicleIdentificationRequest_EID(ara::diag::doip::VehicleInfo &vehicleInfo_Ref);
        // Function to trigger Vehicle Identification req with VIN
        Std_ReturnType SendVehicleIdentificationRequest_VIN(ara::diag::doip::VehicleInfo &vehicleInfo_Ref);
        //
        void ProcessVehicleIdentificationResponse(UdpMessagePtr udpRxMessage);
        // Function Called during DoIP_Ctrl timeout
        void DoIP_CtrlTimeout(void);
        // Function to create Generic Header
        void CreateDoIPGenericHeader(std::vector<uint8_t> &doipHeader, uint16_t payloadType, uint32_t payloadLen);
        // Function to send Doip Generic NACK message
        bool SendDoIPNACKMessage(uint8_t nackType);
        // Function to process DoIP Header
        bool ProcessDoIPHeader(std::vector<uint8_t> &doipHeader, uint8_t &nackCode);
        // Function to check for duplicate Vehicle info received
        bool CheckForDuplicateVehicleInfo(ara::diag::doip::VehicleInfo &vehicleInfo_Ref);
    private:
        // udp transport handler ref
        ara::diag::doip::udpTransport::udp_TransportHandler& udpTransport_Handler_e;
        // //udp socket handler broadcast
        std::unique_ptr<ara::diag::doip::udpSocket::udp_SocketHandler> udpSocket_Handler_bcast;
        //udp socket handler unicast
        std::unique_ptr<ara::diag::doip::udpSocket::udp_SocketHandler> udpSocket_Handler_ucast;
        //udp channel state for broadcast
        udpChannelState udpChannleState_bcast{udpChannelState::kIdle};
        //udp channel state for broadcast
        udpChannelState udpChannleState_ucast{udpChannelState::kIdle};
        //vector to store all vehicle info
        std::vector<ara::diag::doip::VehicleInfo> vehicle_info;
        // timer broadcast
        std::unique_ptr<udpChanlTimer> timer_bcast;
        // timer unicast
        std::unique_ptr<udpChanlTimer> timer_ucast;
};


} // tcpChannel
} // doip
} // diag
} // ara

#endif // _UDP_CHANNEL_H_
