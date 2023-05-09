/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_PARSER_JSON_PARSER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_PARSER_JSON_PARSER_H
// includes
#include <string_view>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace boost_support {
namespace parser {
using boostTree = boost::property_tree::ptree;

/*
 @ Class Name        : Parser to get the configuration from json file.
*/
class JsonParser {
public:
  JsonParser() = default;
  
  boostTree operator() (std::string_view json_path) {
    boostTree json_tree{};
    boost::property_tree::read_json(std::string{json_path}, json_tree);
    return json_tree;
  }
};
}  // namespace parser
}  // namespace boost_support

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_PARSER_JSON_PARSER_H
