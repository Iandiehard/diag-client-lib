/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "boost-support/socket/io_context.h"

namespace boost_support {
namespace socket {

IoContext::IoContext() noexcept : io_context_{} {}

IoContext::~IoContext() noexcept = default;

IoContext::Context &IoContext::GetContext() noexcept { return io_context_; }

}  // namespace socket
}  // namespace boost_support
