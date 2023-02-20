/* Diagnostic Client library
 * Copyright (C) 2022  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "utility/logger.h"

namespace libUtility {
namespace logger {
Logger::Logger(const std::string &context_id) {
#ifdef USE_DLT
  DLT_REGISTER_CONTEXT(contxt_, context_id.c_str(), "Application Context");
#endif
}

Logger::Logger(const std::string &app_id, const std::string &context_id) {
#ifdef USE_DLT
  DLT_REGISTER_APP(app_id.c_str(), "Application Id");
  DLT_REGISTER_CONTEXT(contxt_, context_id.c_str(), "Application Context");
#endif
}

Logger::~Logger() {
#ifdef USE_DLT
  DLT_UNREGISTER_CONTEXT(contxt_);
  DLT_UNREGISTER_APP();
#endif
}

}  // namespace logger
}  // namespace libUtility
