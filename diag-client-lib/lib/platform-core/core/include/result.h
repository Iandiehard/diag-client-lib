/* Diagnostic Client library
 * Copyright (C) 2024  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_RESULT_H_
#define DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_RESULT_H_

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "core/include/error_code.h"

namespace core_type {

/**
 * @brief       Class type to contains a value (of type ValueType), or an error (of type ErrorType)
 * @details     This class is implemented based on API specification of ara::core::Result from Adaptive Platform Core AUTOSAR AP R21-11
 * @tparam      T
 *              The type of value
 * @tparam      E
 *              The type of error
 */
template<typename T, typename E = ErrorCode>
class Result final {
 public:
  /**
   * @brief  Type alias for the type T of values
   */
  using value_type = T;

  /**
   * @brief  Type alias for the type E of errors
   */
  using error_type = E;

  /**
   * @brief       Build a new Result from the specified value (given as lvalue)
   * @param[in]   t
   *              The value to put into the Result
   * @return      Result
   *              A Result that contains the value t
   */
  static Result FromValue(const T &t) noexcept { return Result{t}; }

  /**
   * @brief       Build a new Result from the specified value (given as rvalue)
   * @param[in]   t
   *              The value to put into the Result
   * @return      Result
   *              A Result that contains the value t
   */
  static Result FromValue(T &&t) noexcept { return Result{std::move(t)}; }

  /**
   * @brief       Build a new Result from a value that is constructed in-place from the given arguments
   * @details     This function shall not participate in overload resolution unless: std::is_constructible<T,
   *              Args&&...>::value is true, and the first type of the expanded parameter pack is not T, and the
   *              first type of the expanded parameter pack is not a specialization of Result
   * @tparam      Args
   *              The types of arguments given to this function
   * @param[in]   args
   *              The arguments used for constructing the value
   * @return      Result
   *              A Result that contains a value
   */
  template<typename... Args>
  static Result FromValue(Args &&...args) noexcept {
    return Result{T{std::forward<Args>(args)...}};
  }

  /**
   * @brief       Build a new Result from the specified error (given as lvalue)
   * @param[in]   e
   *              The error to put into the Result
   * @return      Result
   *              A Result that contains the error e
   */
  static Result FromError(const E &e) noexcept { return Result{e}; }

  /**
   * @brief       Build a new Result from the specified error (given as rvalue)
   * @param[in]   e
   *              The error to put into the Result
   * @return      Result
   *              A Result that contains the error e
   */
  static Result FromError(E &&e) noexcept { return Result{std::move(e)}; }

  /**
   * @brief       Build a new Result from an error that is constructed in-place from the given arguments
   * @details     This function shall not participate in overload resolution unless: std::is_const
   *              Args&&...>::value is true, and the first type of the expanded parameter pack
   *              first type of the expanded parameter pack is not a specialization of Result
   * @tparam      Args
   *              The types of arguments given to this function
   * @param[in]   args
   *              The arguments used for constructing the error
   * @return      Result
   *              A Result that contains an error
   */
  template<typename... Args>
  static Result FromError(Args &&...args) noexcept {
    return Result{E{std::forward<Args>(args)...}};
  }

  /**
   * @brief       Construct a new Result from the specified value (given as lvalue)
   * @param[in]   t
   *              The value to put into the Result
   */
  constexpr explicit Result(const T &t) noexcept : storage_{t} {}

  /**
   * @brief       Construct a new Result from the specified value (given as rvalue)
   * @param[in]   t
   *              The value to put into the Result
   */
  constexpr explicit Result(T &&t) noexcept : storage_{std::move(t)} {}

  /**
   * @brief       Construct a new Result from the specified error (given as lvalue)
   * @param[in]   e
   *              The error to put into the Result
   */
  constexpr explicit Result(const E &e) noexcept : storage_{e} {}

  /**
   * @brief       Construct a new Result from the specified error (given as rvalue)
   * @param[in]   e
   *              The error to put into the Result
   */
  constexpr explicit Result(E &&e) noexcept : storage_{std::move(e)} {}

  /**
   * @brief       Copy-construct a new Result from another instance
   * @param[in]   other
   *              The other instance
   */
  Result(const Result &other) = default;

  /**
   * @brief       Copy-assign another Result to this instance
   * @param[in]   other
   *              The other instance
   * @return      Result &
   *              *this, containing the contents of other
   */
  Result &operator=(const Result &other) = default;

  /**
   * @brief       Move-construct a new Result from another instance
   * @param[in]   other
   *              The other instance
   */
  Result(Result &&other) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                  std::is_nothrow_move_constructible<E>::value) = default;

  /**
   * @brief       Move-assign another Result to this instance
   * @param[in]   other
   *              The other instance
   * @return      Result &
   *              *this, containing the contents of other
   */
  Result &operator=(Result &&other) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                             std::is_nothrow_move_assignable<T>::value &&
                                             std::is_nothrow_move_constructible<E>::value &&
                                             std::is_nothrow_move_assignable<E>::value) = default;

  /**
   * @brief      Destruct an instance of Result
   */
  ~Result() noexcept = default;

  /**
   * @brief       Put a new value into this instance, constructed in-place from the given arguments
   * @tparam      Args
   *              The types of arguments given to this function
   * @param[in]   args
   *              The arguments used for constructing the value
   */
  template<typename... Args>
  void EmplaceValue(Args &&...args) noexcept {
    storage_.template emplace<value_type>(std::forward<Args>(args)...);
  }

  /**
   * @brief       Put a new error into this instance, constructed in-place from the given arguments
   * @tparam      Args
   *              The types of arguments given to this function
   * @param[in]   args
   *              The arguments used for constructing the error
   */
  template<typename... Args>
  void EmplaceError(Args &&...args) noexcept {
    storage_.template emplace<error_type>(std::forward<Args>(args)...);
  }

  /**
   * @brief       Check whether *this contains a value
   * @return      bool
   *              True if *this contains a value, false otherwise
   */
  bool HasValue() const noexcept { return std::holds_alternative<value_type>(storage_); }

  /**
   * @brief       Check whether *this contains a value
   * @return      bool
   *              True if *this contains a value, false otherwise
   */
  explicit operator bool() const noexcept { return HasValue(); }

  /**
   * @brief       Access the contained value
   * @details     This function’s behavior is undefined if *this does not contain a value
   * @return      const T &
   *              A const_reference to the contained value
   */
  const T &operator*() const & noexcept { return std::get<value_type>(storage_); }

  /**
   * @brief       Access the contained value
   * @details     This function’s behavior is undefined if *this does not contain a value
   * @return      T &&
   *              An rvalue reference to the contained value
   */
  T &&operator*() && noexcept { return std::move(std::get<value_type>(storage_)); }

  /**
   * @brief       Access the contained value
   * @details     This function’s behavior is undefined if *this does not contain a value
   * @return      const T *
   *              A pointer to the contained value
   */
  const T *operator->() const noexcept { return std::get_if<value_type>(storage_); }

  /**
   * @brief       Access the contained value
   * @details     This function’s behavior is undefined if *this does not contain a value
   * @return      const T &
   *              A const reference to the contained value
   */
  const T &Value() const & noexcept { return std::get<value_type>(storage_); }

  /**
   * @brief       Access the contained value
   * @details     This function’s behavior is undefined if *this does not contain a value
   * @return      T &&
   *              An rvalue reference to the contained value
   */
  T &&Value() && noexcept { return std::move(std::get<value_type>(storage_)); }

  /**
   * @brief       Access the contained error
   * @details     This function’s behavior is undefined if *this does not contain an error
   * @return      const E &
   *              A const reference to the contained error
   */
  const E &Error() const & noexcept { return std::get<error_type>(storage_); }

  /**
   * @brief       Access the contained error
   * @details     This function’s behavior is undefined if *this does not contain an error
   * @return      E &&
   *              An rvalue reference to the contained error
   */
  E &&Error() && noexcept { return std::move(std::get<error_type>(storage_)); }

  /**
   * @brief       Return the contained value as an Optional
   * @return      std::optional<T>
   *              An Optional with the value, if present
   */
  std::optional<T> Ok() const & noexcept {
    std::optional<T> opt_val{};
    if (HasValue()) { opt_val.emplace(Value()); }
    return opt_val;
  }

  /**
   * @brief       Return the contained value as an Optional
   * @return      std::optional<T>
   *              An Optional with the value, if present
   */
  std::optional<T> Ok() && noexcept {
    std::optional<T> opt_val{};
    if (HasValue()) { opt_val.emplace(std::move(Value())); }
    return opt_val;
  }

  /**
   * @brief       Return the contained error as an Optional
   * @return      std::optional<E>
   *              An Optional with the error, if present
   */
  std::optional<E> Err() const & noexcept {
    std::optional<E> opt_err{};
    if (!HasValue()) { opt_err.emplace(Error()); }
    return opt_err;
  }

  /**
   * @brief       Return the contained error as an Optional
   * @return      std::optional<E>
   *              An Optional with the error, if present
   */
  std::optional<E> Err() && noexcept {
    std::optional<E> opt_err{};
    if (!HasValue()) { opt_err.emplace(std::move(Error())); }
    return opt_err;
  }

  /**
   * @brief       Returns the result of the given function on the contained value if it exists;
   *              otherwise, returns the result itself
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F, typename R2 = typename std::invoke_result_t<F, value_type &&>>
  auto AndThen(F &&fn) && noexcept -> R2 {
    return HasValue() ? std::forward<F>(fn)(std::move(*this).Value())
                      : R2{std::move(*this).Error()};
  }

  /**
   * @brief       Returns the result of the given function on the contained value if it exists;
   *              otherwise, returns the result itself
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F, typename E2 = std::invoke_result_t<F, E>>
  Result<T, E2> MapError(F &&fn) {
    return HasValue() ? Result<T, E2>{std::move(*this).Value()}
                      : Result<T, E2>{fn(std::move(*this).Error())};
  }

  /**
   * @brief       Returns the result itself if it contains a value;
   *              otherwise, returns the result of the given function on the error value
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F>
  Result OrElse(F &&fn) && noexcept {
    return HasValue() ? Result{std::move(*this)} : Result{fn(std::move(*this).Error())};
  }

  /**
   * @brief       Return the contained value or the given default value
   * @details     If *this contains a value, it is returned. Otherwise, the specified default value is returned, static_cast’d to T
   * @tparam      U
   *              The type of defaultValue
   * @param[in]   defaultValue
   *              The value to use if *this does not contain a value
   * @return      T
   *              The value
   */
  template<typename U>
  T ValueOr(U &&defaultValue) const & noexcept {
    return HasValue() ? Value() : static_cast<T>(std::forward<U>(defaultValue));
  }

  /**
   * @brief       Return the contained value or the given default value
   * @details     If *this contains a value, it is returned. Otherwise, the specified default value is returned, static_cast’d to T
   * @tparam      U
   *              The type of defaultValue
   * @param[in]   defaultValue
   *              The value to use if *this does not contain a value
   * @return      T
   *              The value
   */
  template<typename U>
  T ValueOr(U &&defaultValue) && noexcept {
    return HasValue() ? std::move(Value()) : static_cast<T>(std::forward<U>(defaultValue));
  }

  /**
   * @brief       Return the contained error or the given default error
   * @details     If *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to E
   * @tparam      G
   *              The type of defaultError
   * @param[in]   defaultError
   *              The error to use if *this does not contain an error
   * @return      E
   *              The error
   */
  template<typename G>
  E ErrorOr(G &&defaultError) const & noexcept {
    return !HasValue() ? Error() : static_cast<E>(std::forward<G>(defaultError));
  }

  /**
   * @brief       Return the contained error or the given default error
   * @details     If *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to E
   * @tparam      G
   *              The type of defaultError
   * @param[in]   defaultError
   *              The error to use if *this does not contain an error
   * @return      E
   *              The error
   */
  template<typename G>
  E ErrorOr(G &&defaultError) && noexcept {
    return !HasValue() ? std::move(Error()) : static_cast<E>(std::forward<G>(defaultError));
  }

  /**
   * @brief       Return the contained value or return the result of a function call
   * @details     If *this contains a value, it is returned. Otherwise, the specified callable is invoked and its return value
   *              which is to be compatible to type T is returned from this function.
   *              The Callable is expected to be compatible to this interface: T f(const E&)
   * @tparam      F
   *              The type of the Callable f
   * @param[in]   f
   *              The Callable
   * @return      T
   *              The value
   */
  template<typename F>
  T Resolve(F &&f) const {
    return HasValue() ? Value() : std::forward<F>(f)(Error());
  }

  /**
   * @brief Exchange the contents of this instance with those of other
   * @param[inout] other
   *               The other instance
   */
  void Swap(Result &other) noexcept(std::is_nothrow_move_constructible<T>::value &&
                                    std::is_nothrow_move_assignable<T>::value &&
                                    std::is_nothrow_move_constructible<E>::value &&
                                    std::is_nothrow_move_assignable<E>::value) {
    storage_.swap(other.storage_);
  }

 private:
  /**
   * @brief      Storage to contain value of type T or error of type E
   */
  std::variant<T, E> storage_;
};

/**
 * @brief       Specialization of class Result for "void" values
 * @details     This class is implemented based on API specification of ara::core::Result from Adaptive Platform Core AUTOSAR AP R21-11
 * @tparam      E
 *              The type of error
 */
template<typename E>
class Result<void, E> final {
 public:
  /**
   * @brief  Type alias for the type T of values, always "void" for this specialization
   */
  using value_type = void;

  /**
   * @brief  Type alias for the type E of errors
   */
  using error_type = E;

  /**
   * @brief       Build a new Result with "void" as value
   * @param[in]   t
   *              The value to put into the Result
   */
  static Result FromValue() noexcept { return Result{}; }

  /**
   * @brief       Build a new Result from the specified error (given as lvalue)
   * @param[in]   e
   *              The error to put into the Result
   * @return      Result
   *              A Result that contains the error e
   */
  static Result FromError(const E &e) noexcept { return Result{e}; }

  /**
   * @brief       Build a new Result from the specified error (given as rvalue)
   * @param[in]   e
   *              The error to put into the Result
   * @return      Result
   *              A Result that contains the error e
   */
  static Result FromError(E &&e) noexcept { return Result{std::move(e)}; }

  /**
   * @brief       Build a new Result from an error that is constructed in-place from the given arguments
   * @details     This function shall not participate in overload resolution unless: std::is_const
   *              Args&&...>::value is true, and the first type of the expanded parameter pack
   *              first type of the expanded parameter pack is not a specialization of Result
   * @tparam      Args
   *              The types of arguments given to this function
   * @param[in]   args
   *              The arguments used for constructing the error
   * @return      Result
   *              A Result that contains an error
   */
  template<typename... Args>
  static Result FromError(Args &&...args) noexcept {
    return Result{E{std::forward<Args>(args)...}};
  }

  /**
   * @brief       Construct a new Result with a "void" value
   */
  constexpr explicit Result() noexcept : storage_{empty_value{}} {}

  /**
   * @brief       Construct a new Result from the specified error (given as lvalue)
   * @param[in]   e
   *              The error to put into the Result
   */
  constexpr explicit Result(const E &e) noexcept : storage_{e} {}

  /**
   * @brief       Construct a new Result from the specified error (given as rvalue)
   * @param[in]   e
   *              The error to put into the Result
   */
  constexpr explicit Result(E &&e) noexcept : storage_{std::move(e)} {}

  /**
   * @brief       Copy-construct a new Result from another instance
   * @param[in]   other
   *              The other instance
   */
  Result(const Result &other) = default;

  /**
   * @brief       Copy-assign another Result to this instance
   * @param[in]   other
   *              The other instance
   * @return      Result &
   *              *this, containing the contents of other
   */
  Result &operator=(const Result &other) = default;

  /**
   * @brief       Move-construct a new Result from another instance
   * @param[in]   other
   *              The other instance
   */
  Result(Result &&other) noexcept(std::is_nothrow_move_constructible<E>::value) = default;

  /**
   * @brief       Move-assign another Result to this instance
   * @param[in]   other
   *              The other instance
   * @return      Result &
   *              *this, containing the contents of other
   */
  Result &operator=(Result &&other) noexcept(std::is_nothrow_move_constructible<E>::value &&
                                             std::is_nothrow_move_assignable<E>::value) = default;

  /**
   * @brief      Destruct an instance of Result
   */
  ~Result() noexcept { static_assert(std::is_trivially_destructible<E>::value); }

  /**
   * @brief       Put a new value into this instance, constructed in-place from the given arguments
   */
  void EmplaceValue() noexcept { storage_.EmplaceValue(empty_value{}); }

  /**
   * @brief       Put a new error into this instance, constructed in-place from the given arguments
   * @tparam      Args
   *              The types of arguments given to this function
   * @param[in]   args
   *              The arguments used for constructing the error
   */
  template<typename... Args>
  void EmplaceError(Args &&...args) noexcept {
    storage_.EmplaceError(std::forward<Args>(args)...);
  }

  /**
   * @brief       Check whether *this contains a value
   * @return      bool
   *              True if *this contains a value, false otherwise
   */
  bool HasValue() const noexcept { return storage_.HasValue(); }

  /**
   * @brief       Check whether *this contains a value
   * @return      bool
   *              True if *this contains a value, false otherwise
   */
  explicit operator bool() const noexcept { return HasValue(); }

  /**
   * @brief       Access the contained error
   * @details     This function’s behavior is undefined if *this does not contain an error
   * @return      const E &
   *              A const reference to the contained error
   */
  const E &Error() const & noexcept { return storage_.Error(); }

  /**
   * @brief       Access the contained error
   * @details     This function’s behavior is undefined if *this does not contain an error
   * @return      E &&
   *              An rvalue reference to the contained error
   */
  E &&Error() && noexcept { return std::move(storage_.Error()); }

  /**
   * @brief       Return the contained error as an Optional
   * @return      std::optional<E>
   *              An Optional with the error, if present
   */
  std::optional<E> Err() const & noexcept {
    std::optional<E> opt_err{};
    if (!HasValue()) { opt_err.emplace(Error()); }
    return opt_err;
  }

  /**
   * @brief       Return the contained error as an Optional
   * @return      std::optional<E>
   *              An Optional with the error, if present
   */
  std::optional<E> Err() && noexcept {
    std::optional<E> opt_err{};
    if (!HasValue()) { opt_err.emplace(std::move(Error())); }
    return opt_err;
  }

  /**
   * @brief       Returns the result of the given function on the contained value if it exists;
   *              otherwise, returns the result itself
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F, typename E2 = std::invoke_result_t<F, E>>
  Result<void, E2> MapError(F &&fn) {
    return HasValue() ? Result<void, E2>{} : Result<void, E2>{fn(Error())};
  }

  /**
   * @brief       Returns the result of the given function on the contained value if it exists;
   *              otherwise, returns the result itself
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F, typename E2 = std::invoke_result_t<F, E>>
  Result<void, E2> CheckError(F &&fn) {
    return HasValue() ? Result<void, E2>{} : Result<void, E2>{fn(Error())};
  }

  /**
   * @brief       Returns the result of the given function on the contained value if it exists;
   *              otherwise, returns the result itself
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F>
  Result AndThen(F &&fn) && noexcept {
    if (HasValue()) { fn(); }
    return Result{std::move(*this)};
  }

  /**
   * @brief       Returns the result itself if it contains a value;
   *              otherwise, returns the result of the given function on the error value
   * @tparam      F
   *              Functor type
   * @param[in]   fn
   *              Callable function
   * @return      Result
   *              An Result
   */
  template<typename F>
  Result OrElse(F &&fn) && noexcept {
    return HasValue() ? Result{std::move(*this)} : Result{fn(Error())};
  }

  /**
   * @brief       Return the contained error or the given default error
   * @details     If *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to E
   * @tparam      G
   *              The type of defaultError
   * @param[in]   defaultError
   *              The error to use if *this does not contain an error
   * @return      E
   *              The error
   */
  template<typename G>
  E ErrorOr(G &&defaultError) const & noexcept {
    return !HasValue() ? Error() : static_cast<E>(std::forward<G>(defaultError));
  }

  /**
   * @brief       Return the contained error or the given default error
   * @details     If *this contains an error, it is returned. Otherwise, the specified default error is returned, static_cast’d to E
   * @tparam      G
   *              The type of defaultError
   * @param[in]   defaultError
   *              The error to use if *this does not contain an error
   * @return      E
   *              The error
   */
  template<typename G>
  E ErrorOr(G &&defaultError) && noexcept {
    return !HasValue() ? std::move(Error()) : static_cast<E>(std::forward<G>(defaultError));
  }

  /**
   * @brief       Do nothing or call a function
   * @details     If *this contains a value, this function does nothing. Otherwise, the specified callable is invoked.
   *              The Callable is expected to be compatible to this interface: void f(const E&);
   * @tparam      F
   *              The type of the Callable f
   * @param[in]   f
   *              The Callable
   * @return      T
   *              The value
   */
  template<typename F>
  void Resolve(F &&f) const {
    std::forward<F>(f)(Error());
  }

 private:
  /**
   * @brief      An empty value for handling void type in value type
   */
  struct empty_value {};

  /**
   * @brief      Storage to contain an empty value or error of type E
   */
  Result<empty_value, E> storage_;
};

// Compare Result with a T

template<typename T, typename E>
bool operator==(const Result<T, E> &lhs, const T &rhs) {
  return lhs ? *lhs == rhs : false;
}

template<typename T, typename E>
bool operator==(const T &lhs, const Result<T, E> &rhs) {
  return rhs ? lhs == *rhs : false;
}

template<typename T, typename E>
bool operator!=(const Result<T, E> &lhs, const T &rhs) {
  return lhs ? *lhs != rhs : true;
}

template<typename T, typename E>
bool operator!=(const T &lhs, const Result<T, E> &rhs) {
  return rhs ? lhs != *rhs : true;
}

// Compare Result with an E

template<typename T, typename E>
bool operator==(const Result<T, E> &lhs, const E &rhs) {
  return lhs ? false : lhs.Error() == rhs;
}

template<typename T, typename E>
bool operator==(const E &lhs, const Result<T, E> &rhs) {
  return rhs ? false : lhs == rhs.Error();
}

template<typename T, typename E>
bool operator!=(const Result<T, E> &lhs, const E &rhs) {
  return lhs ? true : lhs.Error() != rhs;
}

template<typename T, typename E>
bool operator!=(const E &lhs, const Result<T, E> &rhs) {
  return rhs ? true : lhs != rhs.Error();
}

}  // namespace core_type

#endif  // DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_RESULT_H_
