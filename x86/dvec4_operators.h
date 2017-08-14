//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/sse_dvec4_operators.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_DVEC4_OPERATORS_H_
# define _PAL_X86_SSE_DVEC4_OPERATORS_H_ 1

#ifdef PAL_ENABLE_AVX

namespace PAL_NAMESPACE
{

////////////////////////////////////////
// Unary operators

PAL_INLINE dvec4 operator+( dvec4 a ) { return a; }
PAL_INLINE dvec4 operator-( dvec4 a )
{
	return dvec4( _mm256_xor_pd( a, int_constants<dvec4>::sign_bitmask() ) );
}

////////////////////////////////////////
// Binary operators

PAL_INLINE dvec4 operator+( dvec4 a, dvec4 b )
{
	a += b; return a;
}
PAL_INLINE dvec4 operator+( dvec4 a, double b )
{
	a += b; return a;
}
PAL_INLINE dvec4 operator+( double a, dvec4 b )
{
	b += a; return b;
}

PAL_INLINE dvec4 operator-( dvec4 a, dvec4 b )
{
	a -= b; return a;
}
PAL_INLINE dvec4 operator-( dvec4 a, double b )
{
	a -= b; return a;
}
PAL_INLINE dvec4 operator-( double a, dvec4 b )
{
	dvec4 r( a );
	r -= b; return r;
}

PAL_INLINE dvec4 operator*( dvec4 a, dvec4 b )
{
	a *= b; return a;
}
PAL_INLINE dvec4 operator*( dvec4 a, double b )
{
	a *= b; return a;
}
PAL_INLINE dvec4 operator*( double a, dvec4 b )
{
	b *= a; return b;
}

PAL_INLINE dvec4 operator/( dvec4 a, dvec4 b )
{
	a /= b; return a;
}
PAL_INLINE dvec4 operator/( dvec4 a, double b )
{
	a /= b; return a;
}
PAL_INLINE dvec4 operator/( double a, dvec4 b )
{
	dvec4 r( a );
	r /= b; return r;
}

////////////////////////////////////////
// bit-wise operators

PAL_INLINE dvec4 operator&( dvec4 a, dvec4 b )
{
	return dvec4( _mm256_and_pd( a, b ) );
}

PAL_INLINE dvec4 operator|( dvec4 a, dvec4 b )
{
	return dvec4( _mm256_or_pd( a, b ) );
}

PAL_INLINE dvec4 operator^( dvec4 a, dvec4 b )
{
	return dvec4( _mm256_xor_pd( a, b ) );
}

////////////////////////////////////////
// Comparison operators

PAL_INLINE dvec4::mask_type operator==( dvec4 a, dvec4 b )
{
	return dvec4::mask_type( _mm256_cmp_pd( a, b, _CMP_EQ_OQ ) );
}
PAL_INLINE dvec4::mask_type operator!=( dvec4 a, dvec4 b )
{
	return dvec4::mask_type( _mm256_cmp_pd( a, b, _CMP_NEQ_OQ ) );
}
PAL_INLINE dvec4::mask_type operator<( dvec4 a, dvec4 b )
{
	return dvec4::mask_type( _mm256_cmp_pd( a, b, _CMP_LT_OQ ) );
}
PAL_INLINE dvec4::mask_type operator<=( dvec4 a, dvec4 b )
{
	return dvec4::mask_type( _mm256_cmp_pd( a, b, _CMP_LE_OQ ) );
}
PAL_INLINE dvec4::mask_type operator>( dvec4 a, dvec4 b )
{
	return dvec4::mask_type( _mm256_cmp_pd( a, b, _CMP_GT_OQ ) );
}
PAL_INLINE dvec4::mask_type operator>=( dvec4 a, dvec4 b )
{
	return dvec4::mask_type( _mm256_cmp_pd( a, b, _CMP_GE_OQ ) );
}

} // namespace pal

#endif // AVX

#endif // _PAL_X86_DVEC4_OPERATORS_H_
