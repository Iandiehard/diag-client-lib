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