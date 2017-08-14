//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/sse_ivec4_operators.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_IVEC4_OPERATORS_H_
# define _PAL_X86_SSE_IVEC4_OPERATORS_H_ 1

namespace PAL_NAMESPACE
{

////////////////////////////////////////
// Unary operators

PAL_INLINE lvec4 operator+( lvec4 a ) { return a; }
PAL_INLINE lvec4 operator-( lvec4 a )
{
	return lvec4( _mm_sub_epi32( lvec4::zero(), a ) );
}
PAL_INLINE lvec4 operator~( lvec4 a )
{
	return lvec4( _mm_xor_si128( a, int_constants<lvec4>::neg_one() ) );
}
PAL_INLINE lvec4 operator!( lvec4 a )
{
	return ~a;
}

PAL_INLINE lvec4 operator++( lvec4 &a, int )
{
	lvec4 a0 = a;
	a += int_constants<lvec4>::one();
	return a0;
}

PAL_INLINE lvec4 &operator++( lvec4 &a )
{
	a += int_constants<lvec4>::one();
	return a;
}

PAL_INLINE lvec4 operator--( lvec4 &a, int )
{
	lvec4 a0 = a;
	a -= int_constants<lvec4>::one();
	return a0;
}

PAL_INLINE lvec4 &operator--( lvec4 &a )
{
	a -= int_constants<lvec4>::one();
	return a;
}

PAL_INLINE lvec4 operator+( lvec4 a, lvec4 b )
{
	a += b; return a;
}
PAL_INLINE lvec4 operator+( lvec4 a, int32_t b )
{
	a += b; return a;
}
PAL_INLINE lvec4 operator+( int32_t a, lvec4 b )
{
	b += a; return b;
}

PAL_INLINE lvec4 operator-( lvec4 a, lvec4 b )
{
	a -= b; return a;
}
PAL_INLINE lvec4 operator-( lvec4 a, int32_t b )
{
	a -= b; return a;
}
PAL_INLINE lvec4 operator-( int32_t a, lvec4 b )
{
	lvec4 r( a );
	r -= b; return r;
}

PAL_INLINE lvec4 operator*( lvec4 a, lvec4 b )
{
	a *= b; return a;
}
PAL_INLINE lvec4 operator*( lvec4 a, int32_t b )
{
	a *= b; return a;
}
PAL_INLINE lvec4 operator*( int32_t a, lvec4 b )
{
	lvec4 r( a );
	r *= b; return r;
}

////////////////////////////////////////
// Binary operators

PAL_INLINE lvec4 operator&( lvec4 a, lvec4 b )
{
	return lvec4( _mm_and_si128( a, b ) );
}
PAL_INLINE lvec4 operator&( lvec4::mask_type a, lvec4 b )
{
	return lvec4( _mm_and_si128( a, b ) );
}
PAL_INLINE lvec4 operator&( lvec4 a, lvec4::mask_type b )
{
	return lvec4( _mm_and_si128( a, b ) );
}

PAL_INLINE lvec4 operator|( lvec4 a, lvec4 b )
{
	return lvec4( _mm_or_si128( a, b ) );
}
PAL_INLINE lvec4 operator|( lvec4::mask_type a, lvec4 b )
{
	return lvec4( _mm_or_si128( a, b ) );
}
PAL_INLINE lvec4 operator|( lvec4 a, lvec4::mask_type b )
{
	return lvec4( _mm_or_si128( a, b ) );
}

PAL_INLINE lvec4 operator^( lvec4 a, lvec4 b )
{
	return lvec4( _mm_xor_si128( a, b ) );
}
PAL_INLINE lvec4 operator^( lvec4::mask_type a, lvec4 b )
{
	return lvec4( _mm_xor_si128( a, b ) );
}
PAL_INLINE lvec4 operator^( lvec4 a, lvec4::mask_type b )
{
	return lvec4( _mm_xor_si128( a, b ) );
}

////////////////////////////////////////

PAL_INLINE lvec4 operator<<( lvec4 a, int32_t amt )
{
	return lvec4( _mm_sll_epi32( a, _mm_cvtsi32_si128( amt ) ) );
}
PAL_INLINE lvec4 &operator<<=( lvec4 &a, int32_t amt )
{
	a = a << amt;
	return a;
}

PAL_INLINE lvec4 operator>>( lvec4 a, int32_t amt )
{
	return lvec4( _mm_sra_epi32( a, _mm_cvtsi32_si128( amt ) ) );
}
PAL_INLINE lvec4 &operator>>=( lvec4 &a, int32_t amt )
{
	a = a >> amt;
	return a;
}

PAL_INLINE lvec4 lsr( lvec4 a, int s )
{
	return _mm_srli_epi32( a, s );
}


////////////////////////////////////////
// Comparison operators

PAL_INLINE lvec4::mask_type operator==( lvec4 a, lvec4 b )
{
	return lvec4::mask_type( _mm_cmpeq_epi32( a, b ) );
}
PAL_INLINE lvec4::mask_type operator!=( lvec4 a, lvec4 b )
{
	return ! lvec4::mask_type( _mm_cmpeq_epi32( a, b ) );
}
PAL_INLINE lvec4::mask_type operator<( lvec4 a, lvec4 b )
{
	return lvec4::mask_type( _mm_cmplt_epi32( a, b ) );
}
PAL_INLINE lvec4::mask_type operator<=( lvec4 a, lvec4 b )
{
	return lvec4::mask_type( _mm_cmpgt_epi32( b, a ) );
}
PAL_INLINE lvec4::mask_type operator>( lvec4 a, lvec4 b )
{
	return lvec4::mask_type( _mm_cmpgt_epi32( a, b ) );
}
PAL_INLINE lvec4::mask_type operator>=( lvec4 a, lvec4 b )
{
	return lvec4::mask_type( _mm_cmplt_epi32( b, a ) );
}

////////////////////////////////////////

// divide is ... annoying, we'll need the operators from above

namespace detail
{

// ceil(log2(d)) - 1 == (bitscan(d-1)+1)-1
constexpr inline int32_t bit_scan( const uint32_t d )
{
	return (d >= 16) ? 4 + bit_scan( d >> 4 ) :
		( ( d < 2 ) ? 0 : ( ( d < 4 ) ? 1 : ( d < 8 ) ? 2 : 3 ) );
}
constexpr inline int32_t compute_int_shift( const uint32_t d )
{
	return bit_scan( d - 1 );
}

PAL_INLINE constexpr int32_t compute_const_int_mul( int32_t sh, const uint32_t d )
{
	return int32_t(1 + (uint64_t(1) << (32+sh)) / uint32_t(d) - (int64_t(1) << 32));
}

PAL_INLINE constexpr int32_t compute_int_shift_abs( int32_t d )
{
	return ( ( d > 1 ) ? bit_scan( d - 1 ) : ( uint32_t(d) == 0x80000000 ) ? 30 : 0 );
}

PAL_INLINE constexpr int32_t compute_int_shift( int32_t d )
{
	return d < 0 ? compute_int_shift_abs( -d ) : compute_int_shift_abs( d );
}

PAL_INLINE constexpr int32_t compute_int_mul_abs( int32_t d )
{
	return ( ( d > 1 ) ? int32_t( ( ( int64_t(1) << ( 32 + compute_int_shift( d ) ) ) / d - ( ( int64_t(1) << 32 ) - 1 ) ) ) : int32_t( uint32_t(d) == 0x80000000 ? 0x80000001 : 1 ) );
}

PAL_INLINE constexpr int32_t compute_int_mul( int32_t d )
{
	return ( d < 0 ) ? compute_int_mul_abs( -d ) : compute_int_mul_abs( d );
}

} // detail

struct int_divisor
{
	PAL_INLINE int_divisor( int32_t d )
		: _mul( _mm_set1_epi32( detail::compute_int_mul( d ) ) ),
		  _shift( _mm_set_epi32( 0, 0, 0, detail::compute_int_shift( d ) ) ),
		  _sign( _mm_set1_epi32( d < 0 ? -1 : 0 ) )
	{
	}

	PAL_INLINE int_divisor( int32_t s, int32_t m, int32_t sign )
		: _mul( _mm_set1_epi32( m ) ),
		  _shift( _mm_cvtsi32_si128( s ) ),
		  _sign( _mm_set1_epi32( sign ) )
	{}

	PAL_INLINE __m128i operator()( __m128i a ) const
	{
#ifdef PAL_ENABLE_SSE4_1
		// split mul in 2 32*32->64
		__m128i a02 = _mm_srli_epi64( _mm_mul_epi32( a, _mul ), 32 );
		__m128i a13 = _mm_mul_epi32( _mm_srli_epi64( a, 32 ), _mul );
		__m128i mula = _mm_blendv_epi8( a02, a13, _mm_set_epi32( -1, 0, -1, 0 ) );
#else
		// TODO: can optimize for AMD XOP if we ever care
		// use unsigned mul
		// hacker's delight, unsigned to signed mul (page 132)
		__m128i himul = _mm_or_si128(
			_mm_srli_epi64( _mm_mul_epu32( a, _mul ), 32 ),
			_mm_and_si128( _mm_mul_epu32(
							   _mm_srli_epi64( a, 32 ), _mul ),
						   _mm_set_epi32( -1, 0, -1, 0 ) ) );

		__m128i sma = _mm_and_si128( _mul, _mm_srai_epi32( a, 31 ) );
		__m128i sam = _mm_and_si128( a, _mm_srai_epi32( _mul, 31 ) );
		__m128i lows = _mm_add_epi32( sma, sam );
		__m128i mula = _mm_sub_epi32( himul, lows );
#endif
		__m128i sum = _mm_add_epi32( mula, a );
		__m128i r = _mm_sra_epi32( sum, _shift );
		__m128i rsign = _mm_sub_epi32( _mm_srai_epi32( a, 31 ), _sign );
		return _mm_xor_si128( _mm_sub_epi32( r, rsign ), _sign );
	}
private:
	__m128i _mul;
	__m128i _shift;
	__m128i _sign;
};

template <int32_t b>
static PAL_INLINE lvec4 divide_by_const( lvec4 a )
{
	// who would do this?
	if ( b == 1 )
		return a;
	if ( b == -1 )
		return -a;
	// overflow...
	if ( uint32_t(b) == 0x80000000 )
		return lvec4( a == lvec4( b ) ) & lvec4( 1 );

	// abs
	constexpr uint32_t ub = b > 0 ? uint32_t(b) : uint32_t(-b);
	if ( ( ub & ( ub - 1 ) ) == 0 )
	{
		// b is a power of 2
		constexpr int p2 = detail::compute_int_shift( ub + 1 );
		// need the sign
		lvec4 s = ( p2 > 1 ) ? lvec4( _mm_srai_epi32( a, p2 - 1 ) ) : a;

		lvec4 ret = _mm_srai_epi32( _mm_add_epi32( a, _mm_srli_epi32( s, 32 - p2 ) ),
									p2 );
		if ( b > 0 )
			return ret;
		return lvec4::zero() - ret;
	}
	constexpr int32_t shift = detail::compute_int_shift( ub );
	constexpr int32_t mult = detail::compute_const_int_mul( shift, ub );
	const int_divisor d( shift, mult, b < 0 ? -1 : 0 );
	return d( a );
}

PAL_INLINE lvec4 operator/( lvec4 a, const int_divisor &b )
{
	return b( a );
}

} // namespace pal

#endif // _PAL_X86_IVEC4_OPERATORS_H_
