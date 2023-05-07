/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef JSON_PARSER_H
#define JSON_PARSER_H
// includes
#include <string_view>

#include "libCommon/Boost_Header.h"

namespace libOsAbstraction {
namespace libBoost {
namespace jsonparser {
using boostTree = boost::property_tree::ptree;

/*
 @ Class Name        : Create Udp Socket
 @ Class Description : Class used to create an udp socket for handling transmission
                       and reception of udp message from driver
*/
class createJsonParser {
public:
  // ctor
  createJsonParser() = default;

  // dtor
  ~createJsonParser() = default;

  // function to get the property tree
  void GetJsonPtree(std::string_view jsonPath, boostTree &jsontree);
};
}  // namespace jsonparser
}  // namespace libBoost
}  // namespace libOsAbstraction
#endif  // JSON_PARSER_H
