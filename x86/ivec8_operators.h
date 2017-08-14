//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/sse_lvec8_operators.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_LVEC8_OPERATORS_H_
# define _PAL_X86_SSE_LVEC8_OPERATORS_H_ 1

#ifdef PAL_ENABLE_AVX

namespace PAL_NAMESPACE
{

////////////////////////////////////////
// Unary operators

PAL_INLINE lvec8 operator+( lvec8 a ) { return a; }
PAL_INLINE lvec8 operator-( lvec8 a )
{
#ifdef PAL_ENABLE_AVX2
	return lvec8( _mm256_xor_si256( a, int_constants<lvec8>::sign_bitmask() ) );
#else
	return lvec8( _mm256_castps_si256( _mm256_xor_ps( a.as_float(), int_constants<lvec8>::sign_bitmask().as_float() ) ) );
#endif
}
PAL_INLINE lvec8 operator~( lvec8 a )
{
#ifdef PAL_ENABLE_AVX2
	return lvec8( _mm256_xor_si256( a, int_constants<lvec8>::neg_one() ) );
#else
	return lvec8( _mm256_castps_si256( _mm256_xor_ps( a.as_float(), int_constants<lvec8>::neg_one().as_float() ) ) );
#endif
}
PAL_INLINE lvec8 operator!( lvec8 a )
{
	return ~a;
}

PAL_INLINE lvec8 operator++( lvec8 &a, int )
{
	lvec8 a0 = a;
	a += int_constants<lvec8>::one();
	return a0;
}

PAL_INLINE lvec8 &operator++( lvec8 &a )
{
	a += int_constants<lvec8>::one();
	return a;
}

PAL_INLINE lvec8 operator--( lvec8 &a, int )
{
	lvec8 a0 = a;
	a -= int_constants<lvec8>::one();
	return a0;
}

PAL_INLINE lvec8 &operator--( lvec8 &a )
{
	a -= int_constants<lvec8>::one();
	return a;
}

////////////////////////////////////////
// Binary operators

#ifdef PAL_ENABLE_AVX2
PAL_INLINE lvec8 operator<<( lvec8 a, int32_t amt )
{
	return lvec8( _mm256_sll_epi32( a, _mm_cvtsi32_si128( amt ) ) );
}
PAL_INLINE lvec8 &operator<<=( lvec8 &a, int32_t amt )
{
	a = a << amt;
	return a;
}

PAL_INLINE lvec8 operator>>( lvec8 a, int32_t amt )
{
	return lvec8( _mm256_sra_epi32( a, _mm_cvtsi32_si128( amt ) ) );
}
PAL_INLINE lvec8 &operator>>=( lvec8 &a, int32_t amt )
{
	a = a >> amt;
	return a;
}

PAL_INLINE lvec8 lsr( lvec8 a, int s )
{
	return _mm256_srli_epi32( a, s );
}

////////////////////////////////////////
// Comparison operators

PAL_INLINE lvec8::mask_type operator==( lvec8 a, lvec8 b )
{
	return lvec8::mask_type( _mm256_cmpeq_epi32( a, b ) );
}
PAL_INLINE lvec8::mask_type operator<( lvec8 a, lvec8 b )
{
	return lvec8::mask_type( _mm256_cmpgt_epi32( b, a ) );
}
PAL_INLINE lvec8::mask_type operator>( lvec8 a, lvec8 b )
{
	return lvec8::mask_type( _mm256_cmpgt_epi32( a, b ) );
}
PAL_INLINE lvec8::mask_type operator<=( lvec8 a, lvec8 b )
{
	return a < b || a == b;
}
PAL_INLINE lvec8::mask_type operator!=( lvec8 a, lvec8 b )
{
	return a < b || a > b;
}
PAL_INLINE lvec8::mask_type operator>=( lvec8 a, lvec8 b )
{
	return ( a > b || a == b );
}
#endif // PAL_ENABLE_AVX2

} // namespace pal

#endif // PAL_ENABLE_AVX

#endif // _PAL_X86_LVEC8_OPERATORS_H_
