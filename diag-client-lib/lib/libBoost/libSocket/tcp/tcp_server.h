/* Diagnostic Client library
* Copyright (C) 2022  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBSOCKET_TCP_TCP_SERVER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBSOCKET_TCP_TCP_SERVER_H

// includes
#include "tcp_types.h"

namespace libBoost {
namespace libSocket {
namespace tcp {

class CreateTcpServerSocket {
public:
  class TcpServerConnection {

  };

public:
  // ctor
  CreateTcpServerSocket(Boost_String &local_ip_address, uint16_t local_port_num);
};

}  // namespace tcp
}  // namespace libSocket
}  // namespace libBoost

#endif // DIAGNOSTIC_CLIENT_LIB_LIB_LIBBOOST_LIBSOCKET_TCP_TCP_SERVER_H