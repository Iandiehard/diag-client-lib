/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

/* includes */
#include "doip-client/error_domain/doip_client_error_domain.h"

#include "core/include/error_domain.h"

namespace doip_client {
namespace error_domain {
namespace {

/**
 * @brief  Store the unique identifier of this domain
 */
constexpr core_type::ErrorDomain::IdType unique_identifier{0x5000000000000002};

/**
 * @brief  Dm error domain
 */
DoipErrorDomain doip_error_domain{};

auto ConvertErrorCodeToString(core_type::ErrorDomain::CodeType error_code) noexcept -> std::string {
  std::string result{};
  DoipErrorErrc const doip_error_code{error_code};

  switch (doip_error_code) {
    case DoipErrorErrc::kInitializationFailed:
      result = "InitializationFailed";
      break;
    case DoipErrorErrc::kDeInitializationFailed:
      result = "DeInitializationFailed";
      break;
    case DoipErrorErrc::kSocketError:
      result = "SocketError";
      break;
    case DoipErrorErrc::kGenericError:
      result = "GenericError";
      break;
  }
  return result;
}

}  // namespace

DoipErrorDomain::DoipErrorDomain() noexcept : core_type::ErrorDomain(unique_identifier) {}

const char* DoipErrorDomain::Name() const noexcept { return domain_name_.c_str(); }

const char* DoipErrorDomain::Message(core_type::ErrorDomain::CodeType error_code) noexcept {
  message_.clear();
  message_ = ConvertErrorCodeToString(error_code);
  return message_.c_str();
}

core_type::ErrorCode MakeErrorCode(DoipErrorErrc,
                                   core_type::ErrorDomain::SupportDataType data) noexcept {
  return core_type::ErrorCode{data, doip_error_domain};
}

}  // namespace error_domain
}  // namespace doip_client
