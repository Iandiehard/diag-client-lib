/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// includes
#include "jsonParser.h"

namespace libOsAbstraction {
namespace libBoost {
namespace jsonparser {
// ctor
createJsonParser::createJsonParser() {}

// dtor
createJsonParser::~createJsonParser() {}

// function to get the property tree
void createJsonParser::getJsonPtree(std::string &jsonPath, boostTree &jsontree) {
  boost::property_tree::read_json(jsonPath, jsontree);
}
}  // namespace jsonparser
}  // namespace libBoost
}  // namespace libOsAbstraction
