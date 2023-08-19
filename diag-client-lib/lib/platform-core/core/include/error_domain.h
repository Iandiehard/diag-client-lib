/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_ERROR_DOMAIN_H_
#define DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_ERROR_DOMAIN_H_

#include <cstdint>

namespace core_type {

/**
 * @brief       Encapsulation of an error domain. An error domain is the controlling entity for ErrorCode’s error code values,
 *              and defines the mapping of such error code values to textual representations.
 * @details     This class is a literal type, and subclasses are strongly advised to be literal types as well.
 */
class ErrorDomain {
 public:
  /**
   * @brief  Type alias for a unique ErrorDomain identifier type
   */
  using IdType = std::uint64_t;

  /**
   * @brief  Type alias for a domain-specific error code value
   */
  using CodeType = std::int32_t;

  /**
   * @brief  Type alias type for vendor-specific supplementary data
   */
  using SupportDataType = std::uint8_t;

  /**
   * @brief       Copy construction shall be disabled
   */
  ErrorDomain(const ErrorDomain &) = delete;

  /**
   * @brief       Move construction shall be disabled
   */
  ErrorDomain(ErrorDomain &&) = delete;

  /**
   * @brief       Copy assignment shall be disabled
   */
  ErrorDomain &operator=(const ErrorDomain &) = delete;

  /**
   * @brief       Move assignment shall be disabled
   */
  ErrorDomain &operator=(ErrorDomain &&) = delete;

  /**
   * @brief       Return the unique domain identifier.
   * @return      The unique identifier
   */
  constexpr IdType Id() const noexcept;

  /**
   * @brief       Return the name of this error domain.
   * @return      The returned pointer remains owned by class ErrorDomain and shall not be freed by clients.
   */
  virtual const char *Name() const noexcept = 0;

  /**
   * @brief       Return a textual representation of the given error code.
   * @param[in]   error_code
   *              The error code value
   * @return      The returned pointer remains owned by the ErrorDomain subclass and shall not be freed by clients.
   */
  virtual const char *Message(CodeType error_code) noexcept = 0;

 protected:
  /**
   * @brief       Construct a new instance with the given identifier. Identifiers are expected to be system-wide unique.
   * @param[in]   id
   *              The unique identifier
   */
  explicit ErrorDomain(IdType id) noexcept;

  /**
   * @brief       Destructor.
   * @details     This dtor is non-virtual (and trivial) so that this class can be a literal type. While this class has
   *              virtual functions, no polymorphic destruction is needed
   */
  virtual ~ErrorDomain() noexcept = default;

 private:
  /**
   * @brief       Store the unique identifier
   */
  IdType id_;
};

}  // namespace core_type

#endif  // DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_ERROR_DOMAIN_H_