/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

// includes
#include "libCommon/Boost_Header.h"

namespace libOsAbstraction {
namespace libBoost {
namespace jsonparser {

using boostTree = boost::property_tree::ptree;

/*
 @ Class Name        : Create Udp Socket
 @ Class Description : Class used to create a udp socket for handling transmission
                       and reception of udp message from driver
*/

class createJsonParser {

public:
    // ctor
    createJsonParser();
    // dtor
    ~createJsonParser();
    // function to get the property tree
    void getJsonPtree(std::string jsonPath, boostTree &jsontree);

};

} // jsonparser
} // libBoost
} // libOsAbstraction

#endif // JSON_PARSER_H