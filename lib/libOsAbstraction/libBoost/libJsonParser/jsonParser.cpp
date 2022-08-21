// includes
#include "jsonParser.h"

namespace libOsAbstraction {
namespace libBoost {
namespace jsonparser {

// ctor
createJsonParser::createJsonParser() {
}

// dtor
createJsonParser::~createJsonParser() {
}

// function to get the property tree
void createJsonParser::getJsonPtree(std::string jsonPath, boostTree &jsontree) {
    boost::property_tree::read_json(jsonPath, jsontree);
}

} // jsonparser
} // libBoost
} // libOsAbstraction