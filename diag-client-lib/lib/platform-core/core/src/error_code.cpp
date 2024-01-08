/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "core/include/error_code.h"

namespace core_type {

template<typename EnumT>
ErrorCode::ErrorCode(EnumT e, ErrorDomain::SupportDataType data) noexcept : code_value_{e},
                                                                            support_data_{data} {}

ErrorCode::ErrorCode(ErrorDomain::CodeType value, ErrorDomain &domain, ErrorDomain::SupportDataType data) noexcept
    : code_value_{value},
      domain_{domain},
      support_data_{data} {}

constexpr ErrorDomain::CodeType ErrorCode::Value() const noexcept { return code_value_; }

constexpr const ErrorDomain &ErrorCode::Domain() const noexcept { return domain_; }

constexpr ErrorDomain::SupportDataType ErrorCode::SupportData() const noexcept { return support_data_; }

std::string_view ErrorCode::Message() noexcept { return std::string_view{domain_.Message(code_value_)}; }

}  // namespace core_type