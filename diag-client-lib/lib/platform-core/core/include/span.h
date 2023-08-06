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
#include <iostream>
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
 * @brief       A view over a contiguous sequence of objects
 * @details     The type T is required to be a complete object type that is not an abstract class type. This class is
 *              implemented based on API specification of ara::core::Span from Adaptive Platform Core AUTOSAR AP R21-11
 * @tparam      T
 *              The type of elements in the Span
 * @tparam      Extent
 *              The extent to use for this Span
 */
template<typename T, std::size_t Extent = dynamic_extent>
class Span;

namespace details {

/**
 * @brief       Check if the condition is as expected otherwise abort with provided message
 * @param[in]   cond
 *              The condition to check
 * @param[in]   message
 *              The message to output when violated
 */
inline void CheckIfExpectedOrAbort(bool cond, const char *message) {
  if (!cond) {
    std::cerr << message << std::endl;
    std::abort();
  }
}

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

/**
 * @brief       A view over a contiguous sequence of objects
 * @tparam      T
 *              The type of elements in the Span storage
 * @tparam      Extent
 *              The extent to use for this Span storage
 */
template<typename T, std::size_t Extent>
struct span_storage {
  constexpr span_storage() noexcept = default;

  constexpr span_storage(T *ptr, std::size_t) noexcept : ptr_{ptr} {}

  T *ptr_ = nullptr;
  std::size_t size_{Extent};
};

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
  static_assert(!std::is_abstract<T>::value, "A span's element type cannot be an abstract class type");

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
  constexpr Span(pointer ptr, size_type count) : storage_{ptr, count} {
    details::CheckIfExpectedOrAbort(Extent != dynamic_extent && count == Extent, "Invalid range");
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
  constexpr Span(pointer first_elem, pointer last_elem) : storage_{first_elem, last_elem - first_elem} {
    details::CheckIfExpectedOrAbort(Extent != dynamic_extent && (last_elem - first_elem) == Extent, "Invalid range");
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
  constexpr explicit Span(element_type (&arr)[N]) noexcept : storage_{arr, N} {}

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
  constexpr explicit Span(std::array<U, N> &arr) noexcept : storage_{arr.data(), N} {}

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
  constexpr explicit Span(const std::array<U, N> &arr) noexcept : storage_{arr.data(), N} {}

  /**
   * @brief       Construct a new Span from the given container
   * @details     [std::data(cont), std::data(cont) + std::size(cont)) shall be a valid range.
   *              This constructor shall not participate in overload resolution unless: extent == dynamic_extent is true,
   *              Container is not a specialization of Span, Container is not a specialization of Array, Container is not a
   *              specialization of std::array, std::is_array<Container>::value is false, std::data(cont) and std::size(cont) are
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
  constexpr explicit Span(Container &cont) noexcept : storage_{std::data(cont), std::size(cont)} {}

  /**
   * @brief       Construct a new Span from the given const container
   * @details     [std::data(cont), std::data(cont) + std::size(cont)) shall be a valid range.
   *              This constructor shall not participate in overload resolution unless: extent == dynamic_extent is true,
   *              Container is not a specialization of Span, Container is not a specialization of Array, Container is not a
   *              specialization of std::array, std::is_array<Container>::value is false, std::data(cont) and std::size(cont) are
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
  constexpr explicit Span(const Container &cont) noexcept : storage_{std::data(cont), std::size(cont)} {}

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
  constexpr explicit Span(const Span<U, N> &other_span) noexcept : storage_{other_span.data(), other_span.size()} {}

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
  constexpr Span<element_type, Count> first() const {
    details::CheckIfExpectedOrAbort(Count <= size(), "Count > size()");
    return Span{data(), Count};
  }

  /**
   * @brief       Return a subspan containing only the first elements of this Span
   * @details     The behavior of this function is undefined if (count > size())
   * @param[in]   count
   *              The number of elements to take over
   * @return      The subspan
   */
  constexpr Span<element_type, dynamic_extent> first(size_type count) const {
    details::CheckIfExpectedOrAbort(count <= size(), "Count > size()");
    return Span{data(), count};
  }

  /**
   * @brief       Return a subspan containing only the last elements of this Span
   * @details     The implementation shall ensure that (Count <= Extent) is true. The behavior of this
   *              function is undefined if (Count > size()).
   * @tparam      Count
   *              The number of elements to take over
   * @return      The subspan
   */
  template<std::size_t Count>
  constexpr Span<element_type, Count> last() const {
    details::CheckIfExpectedOrAbort(Count <= size(), "Count > size()");
    return Span{data() + (size() - Count), Count};
  }

  /**
   * @brief       Return a subspan containing only the last elements of this Span
   * @details     The behavior of this function is undefined if (count > size())
   * @param[in]   count
   *              The number of elements to take over
   * @return      The subspan
   */
  constexpr Span<element_type, dynamic_extent> last(size_type count) const {
    details::CheckIfExpectedOrAbort(count <= size(), "Count > size()");
    return Span{data() + (size() - count), count};
  }

  /**
   * @brief       Return a subspan of this Span
   * @details     The second template argument of the returned Span type is:
   *              Count != dynamic_extent ? Count : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent)
   *              The implementation shall ensure that (Offset <= Extent && (Count == dynamic_extent || Count <= Extent - Offset)) is true.
   *              The behavior of this function is undefined unless (Offset <= size() && (Count == dynamic_extent || Count <= size() - Offset)) is true.
   * @tparam      Offset
   *              The offset into this Span from which to start
   * @tparam      Count
   *              The number of elements to take over
   * @return      The subspan
   */
  template<std::size_t Offset, std::size_t Count = dynamic_extent>
  constexpr auto subspan() const noexcept
      -> Span<element_type,
              Count != dynamic_extent ? Count : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent)> {
    details::CheckIfExpectedOrAbort((Offset <= size() && (Count == dynamic_extent || Count <= size() - Offset)),
                                    "(Offset <= size() && (Count == dynamic_extent || Count <= size() - Offset))");
    return Span{data() + Offset, Count != dynamic_extent ? Count : size() - Offset};
  }

  /**
   * @brief       Return a subspan of this Span
   * @details     The behavior of this function is undefined unless (offset <= size() && (count == dynamic_extent || count <= size() - offset)) is true
   * @param[in]   offset
   *              The offset into this Span from which to start
   * @param[in]   count
   *              The number of elements to take over
   * @return      The subspan
   */
  constexpr Span<element_type, dynamic_extent> subspan(size_type offset, size_type count = dynamic_extent) const {
    details::CheckIfExpectedOrAbort((offset <= size() && (count == dynamic_extent || count <= size() - offset)),
                                    "(offset <= size() && (count == dynamic_extent || count <= size() - offset))");
    return Span{data() + offset, count != dynamic_extent ? count : size() - offset};
  }

  /**
   * @brief       Return the size of this Span
   * @return      The number of elements contained in this Span
   */
  constexpr size_type size() const noexcept { return storage_.size_; }

  /**
   * @brief       Return the size of this Span in bytes
   * @return      The number of bytes covered by this Span
   */
  constexpr size_type size_bytes() const noexcept { return size() * sizeof(element_type); }

  /**
   * @brief       Return whether this Span is empty
   * @return      True if this Span contains 0 elements, False otherwise
   */
  constexpr bool empty() const noexcept { return size() == 0u; }

  /**
   * @brief       Return a reference to the n-th element of this Span
   * @param[in]   idx
   *              The index into this Span
   * @return      The reference
   */
  constexpr reference operator[](size_type idx) const {
    details::CheckIfExpectedOrAbort(idx < size(), "idx > size()");
    return *(data() + idx);
  }

  /**
   * @brief       Return a reference to the first element of this Span
   * @details     The behavior of this function is undefined if empty() is true
   * @return      The reference
   */
  constexpr reference front() const {
    details::CheckIfExpectedOrAbort(!empty(), "Span is empty");
    return *(data());
  }

  /**
   * @brief       Return a reference to the last element of this Span
   * @details     The behavior of this function is undefined if empty() is true
   * @return      The reference
   */
  constexpr reference back() const {
    details::CheckIfExpectedOrAbort(!empty(), "Span is empty");
    return *(data() + (size() - 1));
  }

  /**
   * @brief       Return a pointer to the start of the memory block covered by this Span
   * @return      The pointer
   */
  constexpr pointer data() const noexcept { return storage_.ptr_; }

  /**
   * @brief       Return an iterator pointing to the first element of this Span
   * @return      The iterator
   */
  constexpr iterator begin() const noexcept { return data(); }

  /**
   * @brief       Return an iterator pointing past the last element of this Span
   * @return      The iterator
   */
  constexpr iterator end() const noexcept { return data() + size(); }

  /**
   * @brief       Return a const_iterator pointing to the first element of this Span
   * @return      The const_iterator
   */
  constexpr const_iterator cbegin() const noexcept { return const_iterator(begin()); }

  /**
   * @brief       Return a const_iterator pointing past the last element of this Span
   * @return      The const_iterator
   */
  constexpr const_iterator cend() const noexcept { return const_iterator(end()); }

  /**
   * @brief       Return a reverse_iterator pointing to the last element of this Span
   * @return      The reverse_iterator
   */
  constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

  /**
   * @brief       Return a reverse_iterator pointing past the first element of this Span
   * @return      The reverse_iterator
   */
  constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

  /**
   * @brief       Return a const_reverse_iterator pointing to the last element of this Span
   * @return      The const_reverse_iterator
   */
  constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

  /**
   * @brief       Return a const_reverse_iterator pointing past the first element of this Span
   * @return      The const_reverse_iterator
   */
  constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

private:
  /**
   * @brief       The storage of span related data
   */
  details::span_storage<T, Extent> storage_;
};

}  // namespace core_type

#endif  // DIAG_CLIENT_LIB_LIB_PLATFORM_CORE_SPAN_H_
