/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "utility/logger.h"

namespace utility {
namespace logger {

Logger::Logger(std::string_view context_id) : contxt_{}, app_id_{}, context_id_{context_id}, registration_with_app_id_{false} {
#ifdef ENABLE_DLT_LOGGER
  DLT_REGISTER_CONTEXT(contxt_, context_id_.c_str(), "Application Context");
#else
  UNUSED_PARAM(registration_with_app_id_);
#endif
}

Logger::Logger(std::string_view app_id, std::string_view context_id)
    : contxt_{},
      app_id_{app_id},
      context_id_{context_id},
      registration_with_app_id_{true} {
#ifdef ENABLE_DLT_LOGGER
  DLT_REGISTER_APP(app_id_.c_str(), "Application Id");
  DLT_REGISTER_CONTEXT(contxt_, context_id_.c_str(), "Application Context");
#else
  UNUSED_PARAM(registration_with_app_id_);
#endif
}

Logger::~Logger() {
#ifdef ENABLE_DLT_LOGGER
  DLT_UNREGISTER_CONTEXT(contxt_);
  if (registration_with_app_id_) { DLT_UNREGISTER_APP(); }
#endif
}

}  // namespace logger
}  // namespace utility
