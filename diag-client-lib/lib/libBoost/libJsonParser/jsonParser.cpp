/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// includes
#include "jsonParser.h"

#include <string>

namespace libOsAbstraction {
namespace libBoost {
namespace jsonparser {

// function to get the property tree
void createJsonParser::GetJsonPtree(std::string_view jsonPath, boostTree &jsontree) {
  boost::property_tree::read_json(std::string{jsonPath}, jsontree);
}

}  // namespace jsonparser
}  // namespace libBoost
}  // namespace libOsAbstraction
