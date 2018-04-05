//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/common/type_utils.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_COMMON_TYPE_UTILS_H_
# define _PAL_COMMON_TYPE_UTILS_H_ 1

#include <type_traits>

////////////////////////////////////////

namespace PAL_NAMESPACE
{

template <typename V>
struct is_float_vec : std::integral_constant<bool,std::is_floating_point<typename V::value_type>::value && sizeof(typename V::value_type) == 4 && std::is_class<typename V::mask_type>::value>
{};

template <typename V>
struct is_double_vec : std::integral_constant<bool,std::is_floating_point<typename V::value_type>::value && sizeof(typename V::value_type) == 8 && std::is_class<typename V::mask_type>::value>
{};

template <typename V>
struct is_floating_point_vec : std::integral_constant<bool,std::is_floating_point<typename V::value_type>::value && std::is_class<typename V::mask_type>::value>
{};

#if __cplusplus >= 201402L
template <typename T> using float_vec_t = typename std::enable_if_t<is_float_vec<T>::value, T>;
template <typename T> using double_vec_t = typename std::enable_if_t<is_double_vec<T>::value, T>;
template <typename T> using floating_point_vec_t = typename std::enable_if_t<is_floating_point_vec<T>::value, T>;
# define PAL_ENABLE_FLOAT(T) float_vec_t<T>
# define PAL_ENABLE_DOUBLE(T) double_vec_t<T>
# define PAL_ENABLE_ANY_FLOAT(T) floating_point_vec_t<T>
#else
# define PAL_ENABLE_FLOAT(T) typename std::enable_if<is_float_vec<T>::value,T>::type
# define PAL_ENABLE_DOUBLE(T) typename std::enable_if<is_double_vec<T>::value,T>::type
# define PAL_ENABLE_ANY_FLOAT(T) typename std::enable_if<is_floating_point_vec<T>::value,T>::type
#endif

#if __cplusplus >= 201703L
template <typename V> inline constexpr bool is_float_vec_v = is_float_vec<V>::value;
template <typename V> inline constexpr bool is_double_vec_v = is_double_vec<V>::value;
template <typename V> inline constexpr bool is_floating_point_vec_v = is_floating_point_vec<V>::value;
#endif

template <typename V>
struct is_int_vec
{
};

} // namespace pal

#endif // _PAL_COMMON_TYPE_UTILS_H_


