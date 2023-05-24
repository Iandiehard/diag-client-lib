/* Diagnostic Client library
* Copyright (C) 2023  Avijit Dey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_RESULT_H
#define DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_RESULT_H

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "core/result.h"

namespace diag {
namespace client {

/**
 * @brief       Class type to contains a value (of type ValueType), or an error (of type ErrorType)
 * @details     This class is implemented based on API specification of ara::core::Result from Adaptive Platform Core AUTOSAR AP R21-11
 * @tparam      T
 *              The type of value
 * @tparam      E
 *              The type of error
 */
template<typename T, typename E>
using Result = core_type::Result<T, E>;

}  // namespace client
}  // namespace diag

#endif  //DIAGNOSTIC_CLIENT_LIB_APPL_INCLUDE_DIAGNOSTIC_CLIENT_RESULT_H
