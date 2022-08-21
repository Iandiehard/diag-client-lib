
#include "diagnostic_client.h"
#include "diagnostic_client_impl.h"

namespace diag {
namespace client {

std::unique_ptr<diag::client::DiagClient> CreateDiagnosticClient(
                                                            std::string dm_client_config) {
    return (std::make_unique<diag::client::DiagClientImpl>(dm_client_config));
}

} // client
} // diag