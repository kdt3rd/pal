//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/sse_dvec2_operators.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_DVEC2_OPERATORS_H_
# define _PAL_X86_SSE_DVEC2_OPERATORS_H_ 1

namespace PAL_NAMESPACE
{

////////////////////////////////////////
// Unary operators

PAL_INLINE dvec2 operator+( dvec2 a ) { return a; }
PAL_INLINE dvec2 operator-( dvec2 a )
{
	return dvec2( _mm_xor_pd( a, int_constants<dvec2>::sign_bitmask() ) );
}

////////////////////////////////////////
// Binary operators

PAL_INLINE dvec2 operator+( dvec2 a, dvec2 b )
{
	a += b; return a;
}
PAL_INLINE dvec2 operator+( dvec2 a, double b )
{
	a += b; return a;
}
PAL_INLINE dvec2 operator+( double a, dvec2 b )
{
	b += a; return b;
}

PAL_INLINE dvec2 operator-( dvec2 a, dvec2 b )
{
	a -= b; return a;
}
PAL_INLINE dvec2 operator-( dvec2 a, double b )
{
	a -= b; return a;
}
PAL_INLINE dvec2 operator-( double a, dvec2 b )
{
	dvec2 r( a );
	r -= b; return r;
}

PAL_INLINE dvec2 operator*( dvec2 a, dvec2 b )
{
	a *= b; return a;
}
PAL_INLINE dvec2 operator*( dvec2 a, double b )
{
	a *= b; return a;
}
PAL_INLINE dvec2 operator*( double a, dvec2 b )
{
	b *= a; return b;
}

PAL_INLINE dvec2 operator/( dvec2 a, dvec2 b )
{
	a /= b; return a;
}
PAL_INLINE dvec2 operator/( dvec2 a, double b )
{
	a /= b; return a;
}
PAL_INLINE dvec2 operator/( double a, dvec2 b )
{
	dvec2 r( a );
	r /= b; return r;
}

////////////////////////////////////////
// bit-wise operators

PAL_INLINE dvec2 operator&( dvec2 a, dvec2 b )
{
	return dvec2( _mm_and_pd( a, b ) );
}

PAL_INLINE dvec2 operator|( dvec2 a, dvec2 b )
{
	return dvec2( _mm_or_pd( a, b ) );
}

PAL_INLINE dvec2 operator^( dvec2 a, dvec2 b )
{
	return dvec2( _mm_xor_pd( a, b ) );
}

////////////////////////////////////////
// Comparison operators

PAL_INLINE dvec2::mask_type operator==( dvec2 a, dvec2 b )
{
	return dvec2::mask_type( _mm_cmpeq_pd( a, b ) );
}
PAL_INLINE dvec2::mask_type operator!=( dvec2 a, dvec2 b )
{
	return dvec2::mask_type( _mm_cmpneq_pd( a, b ) );
}
PAL_INLINE dvec2::mask_type operator<( dvec2 a, dvec2 b )
{
	return dvec2::mask_type( _mm_cmplt_pd( a, b ) );
}
PAL_INLINE dvec2::mask_type operator<=( dvec2 a, dvec2 b )
{
	return dvec2::mask_type( _mm_cmple_pd( a, b ) );
}
PAL_INLINE dvec2::mask_type operator>( dvec2 a, dvec2 b )
{
	return dvec2::mask_type( _mm_cmpgt_pd( a, b ) );
}
PAL_INLINE dvec2::mask_type operator>=( dvec2 a, dvec2 b )
{
	return dvec2::mask_type( _mm_cmpge_pd( a, b ) );
}

} // namespace pal

#endif // _PAL_X86_DVEC2_OPERATORS_H_
