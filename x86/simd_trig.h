//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/simd_trig.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SIMD_TRIG_H_
# define _PAL_X86_SIMD_TRIG_H_ 1

namespace PAL_NAMESPACE
{

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)::int_vec_type
reduce_pi_2( VT &x, VT &y, VT v )
{
}

namespace detail
{

constexpr inline int64_t factorial( const int x ) 
{
	return x == 1 ? x : x * factorial( x - 1 );
}

template <typename T>
constexpr inline T scale( int n, int sign )
{
	return ( sign > 0 ? T(1) : T(-1) ) / static_cast<T>(factorial(n));
}

}

/// TODO: this applies for double as well, but probably need more terms in the taylor series
/// to be accurate...
template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
sinf( VT v )
{
	using fvec = VT;
	using constants = float_constants<fvec>;
	using value_type = typename fvec::value_type;

	// error isn't great - would be better to range reduce more... should be able to
	// go to abs(v) < pi/2 trivially
	v = fmodf( v, constants::two_pi() );

	// for taylor series (well, Maclaurin series):
	// sin x is x - x^3/3! + x^5/5! - x^7/7! ... x^(2*n+1)*(-1)^n/(2n + 1)!
	//
	// most C libraries seem to go out to 7 terms (n = 0 1 2 3 4 5 6)
	//
	// TODO: we'll go beyond since we aren't doing enough range reduction?
	const fvec x2 = v * v;
	const fvec x3 = x2 * v;
	const fvec x5 = x2 * x3;
	const fvec x7 = x2 * x5;
	const fvec x9 = x2 * x7;
	const fvec x11 = x2 * x9;
	const fvec x13 = x2 * x11;
	const fvec x15 = x2 * x13;
	const fvec x17 = x2 * x15;
//	const fvec x19 = x2 * x17;
//	const fvec x21 = x2 * x19;
//	const fvec x23 = x2 * x21;

	fvec r = fma( x3, fvec::splat( detail::scale<value_type>( 3, -1 ) ), v );
	r = fma( x5, fvec::splat( detail::scale<value_type>( 5, 1 ) ), r );
	r = fma( x7, fvec::splat( detail::scale<value_type>( 7, -1 ) ), r );
	r = fma( x9, fvec::splat( detail::scale<value_type>( 9, 1 ) ), r );
	r = fma( x11, fvec::splat( detail::scale<value_type>( 11, -1 ) ), r );
	r = fma( x13, fvec::splat( detail::scale<value_type>( 13, 1 ) ), r );
	r = fma( x15, fvec::splat( detail::scale<value_type>( 15, -1 ) ), r );
	r = fma( x17, fvec::splat( detail::scale<value_type>( 17, 1 ) ), r );
//	r = fma( x19, fvec::splat( detail::scale<value_type>( 19, -1 ) ), r );
//	r = fma( x21, fvec::splat( detail::scale<value_type>( 21, 1 ) ), r );
//	r = fma( x23, fvec::splat( detail::scale<value_type>( 23, -1 ) ), r );

	return r;
}

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
cosf( VT v )
{
	using fvec = VT;
	using constants = float_constants<fvec>;
	using value_type = typename fvec::value_type;

	v = fmodf( v, constants::two_pi() );

	// for taylor series (well, Maclaurin series):
	// cos x is 1 - x^2/2! + x^4/4! + x^(2*n)*(-1)^n/(2n)!
	//
	// most C libraries seem to go out to 7 terms (n = 0 1 2 3 4 5 6)
	//
	// TODO: we'll go beyond since we aren't doing any range reduction?
	const fvec x2 = v * v;
	const fvec x4 = x2 * x2;
	const fvec x6 = x2 * x4;
	const fvec x8 = x2 * x6;
	const fvec x10 = x2 * x8;
	const fvec x12 = x2 * x10;
	const fvec x14 = x2 * x12;
	const fvec x16 = x2 * x14;
	const fvec x18 = x2 * x16;

	fvec r = fma( x2, fvec::splat( detail::scale<value_type>( 2, -1 ) ), constants::one() );
	r = fma( x4, fvec::splat( detail::scale<value_type>( 4, 1 ) ), r );
	r = fma( x6, fvec::splat( detail::scale<value_type>( 6, -1 ) ), r );
	r = fma( x8, fvec::splat( detail::scale<value_type>( 8, 1 ) ), r );
	r = fma( x10, fvec::splat( detail::scale<value_type>( 10, -1 ) ), r );
	r = fma( x12, fvec::splat( detail::scale<value_type>( 12, 1 ) ), r );
	r = fma( x14, fvec::splat( detail::scale<value_type>( 14, -1 ) ), r );
	r = fma( x16, fvec::splat( detail::scale<value_type>( 16, 1 ) ), r );
	r = fma( x18, fvec::splat( detail::scale<value_type>( 18, -1 ) ), r );

	return r;
}

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
sincosf( VT v, VT *s, VT *c )
{
	*s = sinf( v );
	*c = cosf( v );
}

} // namespace pal


#endif // _PAL_X86_SIMD_TRIG_H_

