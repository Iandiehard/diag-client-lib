/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_PARSER_JSON_PARSER_H
#define DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_PARSER_JSON_PARSER_H
// includes
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string_view>

#include "core/include/result.h"

namespace boost_support {
namespace parser {

/**
 * @brief  Type alias for boost property tree
 */
using boost_tree = boost::property_tree::ptree;

/**
 * @brief  Definitions of Parsing failure error codes
 */
enum class ParsingErrorCode : std::uint8_t { kError = 0U };

/**
 * @brief           Parser to get the configuration from json file
 * @param[in]       config_path
 *                  The path to config file present
 * @param[in,out]   json_tree
 *                  The config tree or success or empty on failure
 * @return          The result of type void on success or error code
 */
core_type::Result<void, ParsingErrorCode> Read(std::string_view config_path, boost_tree &json_tree);

}  // namespace parser
}  // namespace boost_support

#endif  // DIAGNOSTIC_CLIENT_LIB_LIB_BOOST_SUPPORT_PARSER_JSON_PARSER_H
