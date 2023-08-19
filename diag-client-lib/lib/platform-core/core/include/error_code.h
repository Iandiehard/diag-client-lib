/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_ERROR_CODE_H_
#define DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_ERROR_CODE_H_

#include <string_view>

#include "core/include/error_domain.h"

namespace core_type {

/**
 * @brief       Encapsulation of an error code. An ErrorCode contains a raw error code value and an error domain.
 *              The raw error code value is specific to this error domain.
 * @details     This class is implemented based on API specification of ara::core::ErrorCode from Adaptive Platform Core AUTOSAR AP R21-11
 */
class ErrorCode final {
 public:
  /**
   * @brief       Construct a new ErrorCode instance with parameters
   * @details     This constructor does not participate in overload resolution unless EnumT is an enum type
   * @tparam      EnumT
   *              An enum type that contains error code values
   * @param[in]   e
   *              A domain-specific error code value
   * @param[in]   data
   *              Optional vendor-specific supplementary error context data
   */
  template<typename EnumT>
  explicit ErrorCode(EnumT e, ErrorDomain::SupportDataType data = ErrorDomain::SupportDataType{}) noexcept;

  /**
   * @brief       Construct a new ErrorCode instance with parameters
   * @tparam      EnumT
   *              An enum type that contains error code values
   * @param[in]   value
   *              A domain-specific error code value
   * @param[in]   domain
   *              The ErrorDomain associated with value
   * @param[in]   data
   *              Optional vendor-specific supplementary error context data
   */
  ErrorCode(ErrorDomain::CodeType value, ErrorDomain &domain,
            ErrorDomain::SupportDataType data = ErrorDomain::SupportDataType{}) noexcept;

  /**
   * @brief       Return the raw error code value.
   * @return      The raw code value
   */
  constexpr ErrorDomain::CodeType Value() const noexcept;

  /**
   * @brief       Return the domain with which this ErrorCode is associated.
   * @return      The reference to error domain
   */
  constexpr const ErrorDomain &Domain() const noexcept;

  /**
   * @brief       Return the supplementary error context data.
   * @details     The underlying type and the meaning of the returned value are implementation-defined
   * @return      The support data type
   */
  constexpr ErrorDomain::SupportDataType SupportData() const noexcept;

  /**
   * @brief       Return a textual representation of this ErrorCode.
   * @return      The error message
   */
  std::string_view Message() noexcept;

 private:
  /**
   * @brief       Store the domain error code value
   */
  ErrorDomain::CodeType code_value_{};

  /**
   * @brief       Store the reference to error domain contained
   */
  ErrorDomain &domain_;

  /**
   * @brief       Store the supported data
   */
  ErrorDomain::SupportDataType support_data_{};
};

}  // namespace core_type

#endif  // DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_ERROR_CODE_H_