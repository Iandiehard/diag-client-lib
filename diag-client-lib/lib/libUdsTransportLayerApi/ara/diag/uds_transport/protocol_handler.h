/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _PROTOCOL_HANDLER_H_
#define _PROTOCOL_HANDLER_H_

#include "protocol_types.h"
#include "protocol_mgr.h"

namespace ara{
namespace diag{

namespace connection{
    class Connection;
}
namespace conversion{
    class ConversionHandler;
}

namespace uds_transport{

using kDoip_String = std::string;

class UdsTransportProtocolHandler
{    
    public:
        enum class InitializationResult : std::uint8_t {
            kInitializeOk = 0,
            kInitializeFailed = 1
        };
        
        //ctor
        inline UdsTransportProtocolHandler (const UdsTransportProtocolHandlerID handler_id, 
                                                  UdsTransportProtocolMgr &transport_protocol_mgr) : handler_id_e(handler_id),
                                                                                                     transport_protocol_mgr_e(transport_protocol_mgr)
        {}
        
        //dtor
        inline virtual ~UdsTransportProtocolHandler() = default;
        
        // Return the UdsTransportProtocolHandlerID
        inline virtual UdsTransportProtocolHandlerID GetHandlerID () const { return handler_id_e;};
        
        // Initialize
        virtual InitializationResult Initialize () = 0;
        
        // Start processing the implemented Uds Transport Protocol
        virtual void Start () = 0;
        
        // Method to indicate that this UdsTransportProtocolHandler should terminate
        virtual void Stop () = 0;
        
        // Get or Create connection
        virtual std::shared_ptr<ara::diag::connection::Connection> FindorCreateConnection(
                                                                                        const std::shared_ptr<ara::diag::conversion::ConversionHandler> &conversion, 
                                                                                        kDoip_String& tcpIpaddress, 
                                                                                        kDoip_String& udpIpaddress, 
                                                                                        uint16_t portNum) = 0;

    protected:
        UdsTransportProtocolHandlerID handler_id_e;
    private:
        UdsTransportProtocolMgr& transport_protocol_mgr_e;
};
            


} // uds_transport
} // diag
} // ara





#endif // _PROTOCOL_HANDLER_H_
