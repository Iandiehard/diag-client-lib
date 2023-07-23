/* Diagnostic Client library
 * Copyright (C) 2023  Avijit Dey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_SPAN_H_
#define DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_SPAN_H_

#include <array>
#include <limits>
#include <type_traits>
#include <utility>

namespace core_type {

/**
 * @brief       A constant for creating Spans with dynamic sizes
 * @details     The constant is always set to std::numeric_limits<std::size_t>::max()
 */
constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

/**
 * @brief       Forward declaration of Span
 */
template<typename T, std::size_t Extent = dynamic_extent>
class Span;

namespace details {

/**
 * @brief       Helper template alias to remove reference and cv and return the actual type
 * @tparam      T
 *              The Element type
 */
template<typename T>
using remove_cv_ref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

/**
 * @brief       Primary template handles Span types that have no nested ::type member
 */
template<typename>
struct is_span : std::false_type {};

/**
 * @brief       Type trait to check if passed element is Span or not
 * @tparam      T
 *              The element type in Span
 * @tparam      Extent
 *              The size of Span
 */
template<typename T, std::size_t Extent>
struct is_span<Span<T, Extent>> : std::true_type {};

/**
 * @brief       Primary template handles std::array types that have no nested ::type member
 */
template<typename>
struct is_std_array : std::false_type {};

/**
 * @brief       Type trait to check if passed element is std::array or not
 * @tparam      T
 *              The element type in std::array
 * @tparam      N
 *              The size of std::array
 */
template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

/**
 * @brief       Primary template handles std::size, std::data that have no nested ::type member
 */
template<typename T, typename = void>
struct is_data_size_valid : std::false_type {};

/**
 * @brief       Type trait to check if std::data(cont) and std::size(cont) are both well-formed.
 * @tparam      T
 *              The element type
 */
template<typename T>
struct is_data_size_valid<T,
                          std::void_t<decltype(std::data(std::declval<T>())), decltype(std::size(std::declval<T>()))>>
    : std::true_type {};

/**
 * @brief       Check if the element type is not a specialization of Span, Container is not a specialization of Array, Container is not a
*               specialization of std::array, std::is_array<Container>::value is false, std::data(cont) and std::size(cont) are
*               both well-formed.
 * @tparam      T
 *              The Element type
 */
template<typename T, typename ElementType = remove_cv_ref_t<T>>
struct is_container_type {
  static constexpr bool value = !is_span<ElementType>::value && !is_std_array<ElementType>::value &&
                                !std::is_array<ElementType>::value && is_data_size_valid<T>::value;
};

/**
 * @brief       Checks if std::remove_pointer_t<decltype(std::data(arr))>(*)[] is convertible to T(*)[]
 * @tparam      F
 *              The type to convert From
 * @tparam      T
 *              The type to convert To
 */
template<typename, typename, typename = void>
struct is_container_element_type_convertible : std::false_type {};

/**
 * @brief       Checks if std::remove_pointer_t<decltype(std::data(arr))>(*)[] is convertible to T(*)[]
 * @tparam      F
 *              The type to convert From
 * @tparam      T
 *              The type to convert To
 */
template<typename F, typename T>
struct is_container_element_type_convertible<
    F, T,
    typename std::enable_if<
        std::is_convertible<std::remove_pointer_t<decltype(std::data(std::declval<F>()))> (*)[], T (*)[]>::value>::type>
    : std::true_type {};
}  // namespace details

/**
 * @brief       A view over a contiguous sequence of objects
 * @details     The type T is required to be a complete object type that is not an abstract class type. This class is
 *              implemented based on API specification of ara::core::Span from Adaptive Platform Core AUTOSAR AP R21-11
 * @tparam      T
 *              The type of elements in the Span
 * @tparam      Extent
 *              The extent to use for this Span
 */
template<typename T, std::size_t Extent>
class Span final {
public:
  /**
   * @brief  Type alias for the type of elements in this Span
   */
  using element_type = T;

  /**
   * @brief  Type alias for the type of values in this Span
   */
  using value_type = typename std::remove_cv<element_type>::type;

  /**
   * @brief  Type alias for the type of parameters that indicate a size or a number of values
   */
  using size_type = std::size_t;

  /**
   * @brief  Type alias for the type of parameters that indicate a difference of indexes into the Span
   */
  using difference_type = std::ptrdiff_t;

  /**
   * @brief  Type alias type for a pointer to an element
   */
  using pointer = element_type *;

  /**
   * @brief  Type alias type for a pointer to an const element
   */
  using const_pointer = const element_type *;

  /**
   * @brief  Type alias type for a reference to an element
   */
  using reference = element_type &;

  /**
   * @brief  Type alias type for a reference to an const element
   */
  using const_reference = const element_type &;

  /**
   * @brief     The type of an iterator to elements
   * @details   This iterator shall implement the concepts RandomAccessIterator, ContiguousIterator, and ConstexprIterator
   */
  using iterator = pointer;

  /**
   * @brief     The type of a const_iterator to elements
   * @details   This iterator shall implement the concepts RandomAccessIterator, ContiguousIterator, and ConstexprIterator
   */
  using const_iterator = const_pointer;

  /**
   * @brief     The type of a reverse_iterator to elements
   */
  using reverse_iterator = std::reverse_iterator<iterator>;

  /**
   * @brief     The type of a const_reverse_iterator to elements
   */
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /**
   * @brief     A constant reflecting the configured Extent of this Span
   */
  static constexpr size_type extent = Extent;

  /**
   * @brief       Default constructor
   * @details     This constructor shall not participate in overload resolution unless (Extent == dynamic_extent || Extent == 0) is true
   */
  template<std::size_t E = Extent, typename std::enable_if<(E == dynamic_extent || E == 0), bool>::type = true>
  constexpr Span() noexcept {}

  /**
   * @brief       Construct a new Span from the given pointer and size
   * @details     [ptr, ptr + count) shall be a valid range. If extent is not equal to dynamic_extent, then count shall
   *              be equal to Extent
   * @param[in]   ptr
   *              The pointer
   * @param[in]   count
   *              The number of elements to take from ptr
   */
  constexpr Span(pointer ptr, size_type count) {
    // abort on invalid range
  }

  /**
   * @brief       Construct a new Span from the open range between [first_elem, last_elem)
   * @details     [first_elem, last_elem) shall be a valid range. If extent is not equal to dynamic_extent, then (last_elem - first_elem)
   *              shall be equal to extent
   * @param[in]   first_elem
   *              The pointer to the first element
   * @param[in]   last_elem
   *              The pointer to past the last element
   */
  constexpr Span(pointer first_elem, pointer last_elem) {
    // abort on invalid range
  }

  /**
   * @brief       Construct a new Span from the given raw array
   * @details     This constructor shall not participate in overload resolution unless: extent == dynamic_extent || N == extent is true,
   *              and std::remove_pointer_t<decltype(std::data(arr))>(*)[] is convertible to T(*)[]
   * @tparam      N
   *              The size of the raw array
   * @param[in]   arr
   *              The raw array
   */
  template<std::size_t N, std::size_t E = Extent,
           typename std::enable_if<(E == dynamic_extent || N == extent) &&
                                       details::is_container_element_type_convertible<element_type (&)[N], T>::value,
                                   bool>::type = true>
  constexpr explicit Span(element_type (&arr)[N]) noexcept {}

  /**
   * @brief       Construct a new Span from the given std::array
   * @details     This constructor shall not participate in overload resolution unless: extent == dynamic_extent || N == extent is true,
   *              and std::remove_pointer_t<decltype(std::data(arr))>(*)[] is convertible to T(*)[]
   * @tparam      U
   *              The type of elements within the std::array
   * @tparam      N
   *              The size of the raw array
   * @param[in]   arr
   *              The std::array
   */
  template<typename U, std::size_t N, std::size_t E = Extent,
           typename std::enable_if<(E == dynamic_extent || N == extent) &&
                                       details::is_container_element_type_convertible<std::array<U, N> &, T>::value,
                                   bool>::type = true>
  constexpr explicit Span(std::array<U, N> &arr) noexcept {}

  /**
   * @brief       Construct a new Span from the given const std::array
   * @details     This constructor shall not participate in overload resolution unless: extent == dynamic_extent || N == extent is true,
   *              and std::remove_pointer_t<decltype(std::data(arr))>(*)[] is convertible to T(*)[]
   * @tparam      U
   *              The type of elements within the std::array
   * @tparam      N
   *              The size of the raw array
   * @param[in]   arr
   *              The std::array
   */
  template<
      typename U, std::size_t N, std::size_t E = Extent,
      typename std::enable_if<(E == dynamic_extent || N == extent) &&
                                  details::is_container_element_type_convertible<const std::array<U, N> &, T>::value,
                              bool>::type = true>
  constexpr explicit Span(const std::array<U, N> &arr) noexcept {}

  /**
   * @brief       Construct a new Span from the given container
   * @details     [ara::core::data(cont), ara::core::data(cont) + ara::core::size(cont)) shall be a valid range.
   *              This constructor shall not participate in overload resolution unless: extent == dynamic_extent is true,
   *              Container is not a specialization of Span, Container is not a specialization of Array, Container is not a
   *              specialization of std::array, std::is_array<Container>::value is false, ara::core::data(cont) and ara::core::size(cont) are
   *              both well-formed, and std::remove_pointer_t<decltype(ara::core::data(cont))>(*)[] is convertible to T(*)[].
   * @tparam      Container
   *              The type of container
   * @param[in]   cont
   *              The container
   */
  template<typename Container, std::size_t E = Extent,
           typename std::enable_if<(E == dynamic_extent) && details::is_container_type<Container>::value &&
                                       details::is_container_element_type_convertible<Container &, T>::value,
                                   bool>::type = true>
  constexpr explicit Span(Container &cont) noexcept {}

  /**
   * @brief       Construct a new Span from the given const container
   * @details     [ara::core::data(cont), ara::core::data(cont) + ara::core::size(cont)) shall be a valid range.
   *              This constructor shall not participate in overload resolution unless: extent == dynamic_extent is true,
   *              Container is not a specialization of Span, Container is not a specialization of Array, Container is not a
   *              specialization of std::array, std::is_array<Container>::value is false, ara::core::data(cont) and ara::core::size(cont) are
   *              both well-formed, and std::remove_pointer_t<decltype(ara::core::data(cont))>(*)[] is convertible to T(*)[].
   * @tparam      Container
   *              The type of container
   * @param[in]   cont
   *              The container
   */
  template<typename Container, std::size_t E = Extent,
           typename std::enable_if<(E == dynamic_extent) && details::is_container_type<Container>::value &&
                                       details::is_container_element_type_convertible<Container &, T>::value,
                                   bool>::type = true>
  constexpr explicit Span(const Container &cont) noexcept {}

  /**
   * @brief       Copy construct a new Span from another instance
   */
  constexpr Span(const Span &) noexcept = default;

  /**
   * @brief       Converting constructor
   * @details     This ctor allows construction of a cv-qualified Span from a normal Span, and also of a
   *              dynamic_extent-Span<> from a static extent-one. This constructor shall not participate in overload resolution
   *              unless: Extent == dynamic_extent || Extent == N is true, U(*)[] is convertible to T(*)[].
   * @tparam      U
   *              The type of elements within the other Span
   * @tparam      N
   *              The Extent of the other Span
   * @param[in]   other_span
   *              The other Span instance
   */
  template<
      typename U, std::size_t N,
      typename std::enable_if<(Extent == dynamic_extent || N == dynamic_extent || Extent == N) &&
                              details::is_container_element_type_convertible<U (*)[], T (*)[]>::value>::type = true>
  constexpr explicit Span(const Span<U, N> &other_span) noexcept {}

  /**
   * @brief       Destructor
   */
  ~Span() noexcept = default;

  /**
   * @brief       Copy assignment operator
   */
  constexpr Span &operator=(const Span &other) noexcept = default;

  /**
   * @brief       Return a subspan containing only the first elements of this Span
   * @details     The implementation shall ensure that (Count <= Extent) is true. The behavior of this
   *              function is undefined if (Count > size()).
   * @tparam      Count
   *              The number of elements to take over
   * @return      The subspan
   */
  template<std::size_t Count>
  constexpr Span<element_type, Count> first() const {}

  /**
   * @brief       Return a subspan containing only the first elements of this Span
   * @details     The behavior of this function is undefined if (count > size())
   * @param[in]   count
   *              The number of elements to take over
   * @return      The subspan
   */
  constexpr Span<element_type, dynamic_extent> first(size_type count) const {}

  /**
   * @brief       Return a subspan containing only the last elements of this Span
   * @details     The implementation shall ensure that (Count <= Extent) is true. The behavior of this
   *              function is undefined if (Count > size()).
   * @tparam      Count
   *              The number of elements to take over
   * @return      The subspan
   */
  template<std::size_t Count>
  constexpr Span<element_type, Count> last() const {}

  /**
   * @brief       Return a subspan containing only the last elements of this Span
   * @details     The behavior of this function is undefined if (count > size())
   * @param[in]   count
   *              The number of elements to take over
   * @return      The subspan
   */
  constexpr Span<element_type, dynamic_extent> last(size_type count) const {}
};

}  // namespace core_type

#endif  // DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_SPAN_H_
