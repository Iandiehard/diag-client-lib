/* MANDAREIN Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _CONFIG_PARSER_TYPE_H_
#define _CONFIG_PARSER_TYPE_H_

/* includes */
#include "common_Header.h"

namespace diag {
namespace client {
namespace config_parser{

class DoipNetworkType{

public:
    std::string tcpIpAddress;
    std::string udpIpAddress;
    std::string udpBroadcastAddress;
    uint16_t portNum;
};

class conversionType {

public:
    uint16_t    p2ClientMax;
    uint16_t    p2StarClientMax;
    uint16_t    txBufferSize;
    uint16_t    rxBufferSize;
    uint16_t    sourceAddress;
    uint16_t    targetAddress;
    std::string conversionName;
    DoipNetworkType network;
};

class ConversionConfig{

public:
    // number of conversion
    uint8_t num_of_conversion;
    // store all conversions
    std::vector<conversionType> conversions;
};

} // config_parser
} // client
} // diag

#endif //  _CONFIG_PARSER_TYPE_H_