/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "core/include/error_domain.h"

namespace core_type {

ErrorDomain::ErrorDomain(ErrorDomain::IdType id) noexcept : id_{id} {}

constexpr ErrorDomain::IdType ErrorDomain::Id() const noexcept { return id_; }

}  // namespace core_type