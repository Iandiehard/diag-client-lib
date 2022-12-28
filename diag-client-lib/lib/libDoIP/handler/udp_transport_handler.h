/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _UDP_TRANSPORT_HANDLER_H_
#define _UDP_TRANSPORT_HANDLER_H_

//includes
#include "common/common_doip_header.h"
#include "channel/udp_channel.h"

namespace ara{
namespace diag{
namespace doip{

//forward declaration
namespace  connection{
class DoipUdpConnection;
}

namespace udpTransport{

/*
 @ Class Name        : UdpTransportHandler
 @ Class Description : Class used to create a udp transport handler to initiate transmission
                       and reception of udp message from/to user                            
*/
class UdpTransportHandler {
public:
  // ctor
  UdpTransportHandler(kDoip_String& localIpaddress,
                        uint16_t portNum,
                        connection::DoipUdpConnection& doipConnection);

  // dtor
  ~UdpTransportHandler();

  // Initialize
  ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize ();

  // Start
  void Start();

  // Stop
  void Stop();

  // Transmit
  ara::diag::uds_transport::UdsTransportProtocolMgr::TransmissionResult
    Transmit (
      ara::diag::uds_transport::UdsMessageConstPtr message,
      ara::diag::uds_transport::ChannelID channel_id);

  // Indicate Message
  ara::diag::uds_transport::UdsTransportProtocolMgr::IndicationResult 
    Indicate(std::vector<ara::diag::doip::VehicleInfo> &vehicleInfo_Ref);

private:
    // reference to doip Connection 
    connection::DoipUdpConnection& doip_connection_;

    // Udp channel responsible for transmitting and reception of UDP messages
    std::unique_ptr<ara::diag::doip::udpChannel::udpChannel> udp_channel;
};

} // udpTransport
} // doip
} // diag
} // ara  

#endif // _UDP_TRANSPORT_HANDLER_H_
