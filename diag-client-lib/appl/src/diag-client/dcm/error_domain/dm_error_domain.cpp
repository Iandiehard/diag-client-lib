/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

/* includes */
#include "dm_error_domain.h"

#include "core/include/error_domain.h"

namespace diag {
namespace client {
namespace error_domain {
namespace {

/**
 * @brief  Store the unique identifier of this domain
 */
constexpr core_type::ErrorDomain::IdType unique_identifier{0x5000000000000001};

/**
 * @brief  Dm error domain
 */
DmErrorDomain dm_error_domain{};

auto ConvertErrorCodeToString(core_type::ErrorDomain::CodeType error_code) noexcept -> std::string {
  std::string result{};
  DmErrorErrc const dm_error_code{error_code};

  switch (dm_error_code) {
    case DmErrorErrc::kInitializationFailed:
      result = "InitializationFailed";
      break;
    case DmErrorErrc::kDeInitializationFailed:
      result = "DeInitializationFailed";
      break;
  }
  return result;
}

}  // namespace

DmErrorDomain::DmErrorDomain() noexcept : core_type::ErrorDomain(unique_identifier) {}

const char* DmErrorDomain::Name() const noexcept { return domain_name_.c_str(); }

const char* DmErrorDomain::Message(core_type::ErrorDomain::CodeType error_code) noexcept {
  message_.clear();
  message_ = ConvertErrorCodeToString(error_code);
  return message_.c_str();
}

core_type::ErrorCode MakeErrorCode(DmErrorErrc,
                                   core_type::ErrorDomain::SupportDataType data) noexcept {
  return core_type::ErrorCode{data, dm_error_domain};
}

}  // namespace error_domain
}  // namespace client
}  // namespace diag
