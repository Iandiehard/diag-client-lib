/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _DOIP_TRANSPORTPROTOCOL_HANDLER_H_
#define _DOIP_TRANSPORTPROTOCOL_HANDLER_H_

// Includes 
#include "common/common_doIP_Header.h"


namespace ara{
namespace diag{
namespace doip{

//forward declaration
namespace connection{
  class DoipConnectionManager;
}

namespace transportProtocolHandler{

/*
 @ Class Name        : doipTransportProtocolHandler
 @ Class Description : This class must be instantiated by user for using the DoIP functionalities.  
                       This will inherit uds transport protocol handler                              
 */

class DoipTransportProtocolHandler : public ara::diag::uds_transport::UdsTransportProtocolHandler
{
  public:
    //ctor
    DoipTransportProtocolHandler(const ara::diag::uds_transport::UdsTransportProtocolHandlerID handler_id,
                                 ara::diag::uds_transport::UdsTransportProtocolMgr &transport_protocol_mgr);
    
    //dtor
    virtual ~DoipTransportProtocolHandler();
    
    // Return the UdsTransportProtocolHandlerID, which was given to the implementation during construction (ctor call).
    ara::diag::uds_transport::UdsTransportProtocolHandlerID GetHandlerID () const override;
    
    // Initializes handler
    ara::diag::uds_transport::UdsTransportProtocolHandler::InitializationResult Initialize () override;
    
    // Start processing the implemented Uds Transport Protocol
    void Start() override;
    
    // Method to indicate that this UdsTransportProtocolHandler should terminate
    void Stop() override;

    // Get or Create connection
    std::shared_ptr<ara::diag::connection::Connection> 
                        FindorCreateConnection(const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion, 
                                                                              kDoip_String& tcpIpaddress, 
                                                                              kDoip_String& udpIpaddress, 
                                                                              uint16_t portNum) override;

  private:    
    // store handle id
    ara::diag::uds_transport::UdsTransportProtocolHandlerID handle_id_e;
    
    // store the transport protocol manager
    ara::diag::uds_transport::UdsTransportProtocolMgr& transport_protocol_mgr_e;
  
    // Create Doip Connection Manager
    std::unique_ptr<ara::diag::doip::connection::DoipConnectionManager> doip_connection_mgr_ptr;

    // Declare dlt logging context
    DLT_DECLARE_CONTEXT(doipclient_main);
};

} // transportProtocolHandler
} // doip
} // diag
} // ara


#endif // _DOIP_TRANSPORTPROTOCOL_HANDLER_H_
