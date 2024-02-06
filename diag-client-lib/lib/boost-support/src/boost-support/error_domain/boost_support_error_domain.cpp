/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

/* includes */
#include "boost-support/error_domain/boost_support_error_domain.h"

#include "core/include/error_domain.h"

namespace boost_support {
namespace error_domain {
namespace {

/**
 * @brief  Store the unique identifier of this domain
 */
constexpr core_type::ErrorDomain::IdType unique_identifier{0x5000000000000003};

/**
 * @brief  Dm error domain
 */
BoostSupportErrorDomain boost_support_error_domain{};

auto ConvertErrorCodeToString(core_type::ErrorDomain::CodeType error_code) noexcept -> std::string {
  std::string result{};
  BoostSupportErrorErrc const boost_support_error_code{error_code};

  switch (boost_support_error_code) {
    case BoostSupportErrorErrc::kInitializationFailed:
      result = "InitializationFailed";
      break;
    case BoostSupportErrorErrc::kDeInitializationFailed:
      result = "DeInitializationFailed";
      break;
    case BoostSupportErrorErrc::kSocketError:
      result = "SocketError";
      break;
    case BoostSupportErrorErrc::kGenericError:
      result = "GenericError";
      break;
  }
  return result;
}

}  // namespace

BoostSupportErrorDomain::BoostSupportErrorDomain() noexcept : core_type::ErrorDomain(unique_identifier) {}

const char* BoostSupportErrorDomain::Name() const noexcept { return domain_name_.c_str(); }

const char* BoostSupportErrorDomain::Message(core_type::ErrorDomain::CodeType error_code) noexcept {
  message_.clear();
  message_ = ConvertErrorCodeToString(error_code);
  return message_.c_str();
}

core_type::ErrorCode MakeErrorCode(BoostSupportErrorErrc, core_type::ErrorDomain::SupportDataType data) noexcept {
  return core_type::ErrorCode{data, boost_support_error_domain};
}

}  // namespace error_domain
}  // namespace boost_support
