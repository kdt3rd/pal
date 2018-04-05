//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/simd_math.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SIMD_MATH_H_
# define _PAL_X86_SIMD_MATH_H_ 1

#include "fvec4_operators.h"
#include "ivec4_operators.h"
#include "dvec2_operators.h"

#include "fvec8_operators.h"
#include "ivec8_operators.h"
#include "dvec4_operators.h"

#include "fvec4_math.h"
#include "ivec4_math.h"
#include "fvec8_math.h"

namespace PAL_NAMESPACE
{

/// @brief clamp values between two values
///
/// NB: if a value coming in is +/- NaN, it will end up as the "high"
/// value instead of preserving NaN, which is usually desired
template <typename vec>
PAL_INLINE vec clamp( vec a, vec low, vec high ) 
{
	return max( min( a, high ), low );
}

/// @brief alternate overload for clamp with low and high being scalars
///
/// @sa clamp
template <typename vec>
PAL_INLINE vec clamp( vec a, typename vec::value_type low, typename vec::value_type high ) 
{
	return max( min( a, vec::splat( high ) ), vec::splat( low ) );
}

/// @brief if mask type is '1', returns t, else returns f
///
/// NB: This may have the same semantics as SSE where it only checks
/// the first bit of any block of the vector, although a good
/// implementation will have all bits set one way or another
template <typename mask, typename vec>
PAL_INLINE vec ifthen( mask m, vec t, vec f )
{
	typedef typename vec::mask_type vmask;
	return vec( vmask(m).blend( f, t ) );
}

////////////////////////////////////////

/// @brief should be the same as fabs from C library
///
/// NaN behavior? NaNs have a sign, but the man page says
/// nothing other than nan is returned...
template <typename vec>
PAL_INLINE PAL_ENABLE_ANY_FLOAT(vec)
fabs( vec v )
{
	typedef typename vec::int_vec_type ivec;
	typedef int_constants<ivec> constants;

	return v & constants::nonsign_bitmask().as_float();
}

template <typename vec>
PAL_INLINE PAL_ENABLE_FLOAT(vec)
fabsf( vec v )
{
	return fabs( v );
}

/// @brief also provide abs in case that is what the user looks for or they call it with an int?
template <typename vec>
PAL_INLINE vec abs( vec v )
{
	typedef vector_limits<vec> limits;
	typedef typename vec::mask_type mvec;

	return v & mvec::splat_mask( ~limits::sign_mask );
}

/// @brief implements standard c copysign function
template <typename vec>
PAL_INLINE PAL_ENABLE_ANY_FLOAT(vec)
copysign( vec a, vec b )
{
	typedef int_constants<vec> constants;
	typedef typename vec::mask_type mvec;

	mvec m( constants::sign_bitmask() );
	return m.bit_mix( a, b );
}

/// @brief implements standard c copysign function
template <typename vec>
PAL_INLINE PAL_ENABLE_FLOAT(vec)
copysignf( vec a, vec b )
{
	return copysign( a, b );
}

// standard vectorized roundf
// 
// NB: this is different than rounding provided by default in SSE
// anyway in that, like the C library, does NOT do "banker's rounding"
// where it alternately rounds odds / evens up/down.
template <typename vec>
PAL_INLINE PAL_ENABLE_FLOAT(vec)
roundf( vec a )
{
	// this takes 2 constants to compute, is there a better way?
	vec rf = float_constants<vec>::one_half();
	rf = ( a < vec::zero() ).blend( a + rf, a - rf );
	return truncf( rf );
}

/// @brief compute the square of a number (a * a)
template <typename vec>
PAL_INLINE vec square( vec a )
{
	return a * a;
}

/// @brief fmod
template <typename vec>
PAL_INLINE PAL_ENABLE_ANY_FLOAT(vec)
fmod( vec n, vec d )
{
	vec c = n / d;
	vec t = vec::convert_int( c.convert_to_int_trunc() );
	return nmadd( t, d, n );
}

/// @brief fmod
template <typename vec>
PAL_INLINE PAL_ENABLE_FLOAT(vec)
fmodf( vec n, vec d )
{
	vec c = n / d;
	vec t = vec::convert_int( c.convert_to_int_trunc() );
	return nmadd( t, d, n );
}

} // namespace pal

#endif // _PAL_X86_SIMD_MATH_H_
