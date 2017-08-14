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
struct is_float_vec
{
	typedef typename V::value_type value_type;
	typedef typename std::enable_if<std::is_floating_point<value_type>::value && sizeof(value_type) == 4,V>::type type;
};

template <typename V>
struct is_double_vec
{
	typedef typename V::value_type value_type;
	typedef typename std::enable_if<std::is_floating_point<value_type>::value && sizeof(value_type) == 8,V>::type type;
};

template <typename V>
struct is_floating_point_vec
{
	typedef typename V::value_type value_type;
	typedef typename std::enable_if<std::is_floating_point<value_type>::value,V>::type type;
};

template <typename V>
struct is_int_vec
{
};

///
/// @brief Class type_utils provides...
///
class type_utils
{
public:
	type_utils( void );
	virtual ~type_utils( void );
private:

};

} // namespace pal

#endif // _PAL_COMMON_TYPE_UTILS_H_


