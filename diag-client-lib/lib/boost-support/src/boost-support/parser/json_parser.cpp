/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "boost-support/parser/json_parser.h"

#include "boost-support/common/logger.h"

namespace boost_support {
namespace parser {

core_type::Result<void, ParsingErrorCode> Read(std::string_view config_path, boost_tree &json_tree) {
  core_type::Result<void, ParsingErrorCode> parse_result{};
  // Get the tree with configuration details
  try {
    boost::property_tree::read_json(std::string{config_path}, json_tree);
    parse_result.EmplaceValue();
  } catch (boost::property_tree::json_parser_error &error) {
    parse_result.EmplaceError(ParsingErrorCode::kError);
    common::logger::LibBoostLogger::GetLibBoostLogger().GetLogger().LogError(
        __FILE__, __LINE__, __func__,
        [&error](std::stringstream &msg) { msg << "Reading of config failed with error: " << error.message(); });
  }
  return parse_result;
}

}  // namespace parser
}  // namespace boost_support