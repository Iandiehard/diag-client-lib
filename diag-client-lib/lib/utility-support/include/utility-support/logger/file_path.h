#include <string_view>

namespace utility_support {
namespace details {
// constexpr function to strip the file path
constexpr std::string_view StripFilePath(std::string_view path) {
  size_t pos{path.find_last_of("/\\")};
  return (pos == std::string_view::npos) ? path : path.substr(pos + 1);
}
}  // namespace details
}  // namespace utility_support

// Macro to get the file name during compile time
#define FILE_NAME utility_support::details::StripFilePath(__FILE__)
