
#include "dcm/service/dm_uds_message.h"

namespace diag{
namespace client{
namespace uds_message{

DmUdsMessage::DmUdsMessage(Address sa
                        , Address ta
                        , IpAddress hostipaddress
                        , ara::diag::uds_transport::ByteVector& payload) 
                        : ara::diag::uds_transport::UdsMessage()
                        , source_address(sa)
                        , target_address(ta)
                        , host_ip_address(hostipaddress)
                        , uds_payload(payload) {
}

DmUdsMessage::~DmUdsMessage () {
}

DmUdsResponse::DmUdsResponse(ByteVector& payload)
                            : uds_payload(payload) {
}

DmUdsResponse::~DmUdsResponse () {    
}

} // uds_message
} // client
} // diag