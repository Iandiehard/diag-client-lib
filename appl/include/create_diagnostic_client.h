#ifndef _CREATE_DIAGNOSTIC_CLIENT_H_
#define _CREATE_DIAGNOSTIC_CLIENT_H_

#include "diagnostic_client.h"

namespace diag {
namespace client {

// 
std::unique_ptr<diag::client::DiagClient> CreateDiagnosticClient(
                                                            std::string dm_client_config);

} // client
} // diag

#endif // _CREATE_DIAGNOSTIC_CLIENT_H_