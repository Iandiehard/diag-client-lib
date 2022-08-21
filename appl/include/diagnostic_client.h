#ifndef _DIAGNOSTIC_CLIENT_H_
#define _DIAGNOSTIC_CLIENT_H_

#include <memory>
#include <vector>
#include "diagnostic_client_conversion.h"

namespace diag {
namespace client {

class DiagClient {

public:
    // ctor
    inline DiagClient() {}

    // dtor
    inline virtual ~DiagClient() = default;

    // Initialize
    virtual void Initialize() = 0;

    // De-Initialize
    virtual void DeInitialize() = 0;

    // Get Required Conversion based on Conversion Name
    virtual diag::client::conversion::DiagClientConversion& 
                        GetDiagnosticClientConversion(std::string conversion_name) = 0;
};

} // client
} // diag

#endif // _DIAGNOSTIC_CLIENT_H_