/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_UDP_UDP_SERVER_H_
#define DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_UDP_UDP_SERVER_H_

#include "boost-support/client/udp/udp_client.h"

namespace boost_support {
namespace server {
namespace udp {

/**
* @brief    Server that manages udp connection
* @details  This is type alias of udp client since both provides the same functionality till now.
*/
using UdpServer = boost_support::client::udp::UdpClient;

}  // namespace udp
}  // namespace server
}  // namespace boost_support
#endif  // DIAG_CLIENT_LIB_LIB_BOOST_SUPPORT_INCLUDE_BOOST_SUPPORT_SERVER_UDP_UDP_SERVER_H_
