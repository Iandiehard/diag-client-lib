/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_ERROR_DOMAIN_DOIP_ERROR_DOMAIN_H_
#define DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_ERROR_DOMAIN_DOIP_ERROR_DOMAIN_H_

#include <string>

#include "core/include/error_code.h"
#include "core/include/error_domain.h"

namespace doip_client {
namespace error_domain {

/**
 * @brief  Definition of error code in Doip Client
 */
enum class DoipErrorErrc : core_type::ErrorDomain::CodeType {
  kInitializationFailed = 0U,   /**< Failure on Initialization */
  kDeInitializationFailed = 1U, /**< Failure on De-Initialization */
  kSocketError = 2U,            /**< Failure on Socket Open/Destroy */
  kGenericError = 3U            /**< Generic Error */
};

/**
 * @brief  Definition of Doip Client Error domain
 */
class DoipErrorDomain final : public core_type::ErrorDomain {
 public:
  /**
   * @brief  Type alias for the error code value enumeration
   */
  using Errc = DoipErrorErrc;

  /**
   * @brief       Default constructor.
   */
  explicit DoipErrorDomain() noexcept;

  /**
   * @brief       Default destructor.
   */
  ~DoipErrorDomain() noexcept override = default;

  /**
   * @brief       Return the name of this error domain.
   * @return      The returned pointer remains owned by class ErrorDomain and shall not be freed by clients.
   */
  const char *Name() const noexcept override;

  /**
   * @brief       Return a textual representation of the given error code.
   * @param[in]   error_code
   *              The error code value
   * @return      The returned pointer remains owned by the ErrorDomain subclass and shall not be freed by clients.
   */
  const char *Message(CodeType error_code) noexcept override;

 private:
  /**
   * @brief  Store the error domain name
   */
  const std::string domain_name_{"DoipClientErrorDomain"};

  /**
   * @brief  Store the error message
   */
  std::string message_;
};

/**
 * @brief       Create a new ErrorCode within DoipErrorDomain.
 * @details     This function is used internally by constructors of ErrorCode. It is usually not used directly by
 *              users
 * @param[in]   code
 *              The DoipErrorDomain-specific error code value
 * @param[in]   data
 *              The optional vendor-specific error data
 * @return      ErrorCode
 *              A new ErrorCode instance
 */
core_type::ErrorCode MakeErrorCode(DoipErrorErrc code, core_type::ErrorDomain::SupportDataType data =
                                                           core_type::ErrorDomain::SupportDataType{}) noexcept;

}  // namespace error_domain
}  // namespace doip_client
#endif  // DIAG_CLIENT_LIB_LIB_DOIP_CLIENT_ERROR_DOMAIN_DOIP_ERROR_DOMAIN_H_
