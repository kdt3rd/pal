//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#if !defined _PAL_H_
# error "Never use <pal/x86/fvec8_math.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_FVEC8_MATH_H_
# define _PAL_X86_SSE_FVEC8_MATH_H_ 1

# ifdef PAL_HAS_FVEC8

namespace PAL_NAMESPACE
{

////////////////////////////////////////

// contains the templated routines that work for fvec8 and fvec8

/// @brief return the max of the 2 numbers. if one is NaN, return the other?
PAL_INLINE fvec8 max( fvec8 a, fvec8 b ) { return fvec8( _mm256_max_ps( a, b ) ); }
/// @brief return the min of the 2 numbers. if one is NaN, return the other?
PAL_INLINE fvec8 min( fvec8 a, fvec8 b ) { return fvec8( _mm256_min_ps( a, b ) ); }

/// @brief apply a * b + c
PAL_INLINE fvec8 fma( fvec8 a, fvec8 b, fvec8 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec8( _mm256_fmadd_ps( a, b, c ) );
#else
	return fvec8( _mm256_add_ps( _mm256_mul_ps( a, b ), c ) );
#endif
}

/// @brief apply a * b - c
PAL_INLINE fvec8 fms( fvec8 a, fvec8 b, fvec8 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec8( _mm256_fmsub_ps( a, b, c ) );
#else
	return fvec8( _mm256_sub_ps( _mm256_mul_ps( a, b ), c ) );
#endif
}

/// @brief apply -( a * b ) + c
PAL_INLINE fvec8 nmadd( fvec8 a, fvec8 b, fvec8 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec8( _mm256_fnmadd_ps( a, b, c ) );
#else
	// hmmm, this is same as c - ( a * b )
	return fvec8( _mm256_sub_ps( c, _mm256_mul_ps( a, b ) ) );
#endif
}

/// @brief apply -(a * b) - c
PAL_INLINE fvec8 nmsub( fvec8 a, fvec8 b, fvec8 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec8( _mm256_fnmsub_ps( a, b, c ) );
#else
	return -( a * b ) - c;
#endif
}

/// @brief computes a horizontal sum of the 4 elements of the vector
PAL_INLINE float hsum( fvec8 v )
{
	__m256 a = _mm256_hadd_ps( v, v );
	a = _mm256_add_ps( a, _mm256_permute2f128_ps( a, a, 0x1 ) );
	return _mm_cvtss_f32( _mm_hadd_ps( _mm256_castps256_ps128( a ),
									   _mm256_castps256_ps128( a ) ) );
}

/// @brief return a fast (estimate) reciprocal (1 / v) of each value
PAL_INLINE fvec8 faster_recip( fvec8 v )
{
	return fvec8( _mm256_rcp_ps( v ) );
}

/// @brief return reciprocal (1 / v) of each value
///
/// This is a bit more accurate than the standard rcp_ps operation
/// as it does a round of newton-raphson refinement
PAL_INLINE fvec8 fast_recip( fvec8 v )
{
	fvec8 e = fvec8( _mm256_rcp_ps( v ) );
	// see the logic path in recip for fvec4 
	// one round of newton-raphson refinement
	return nmadd( v, e * e, e + e );
}

/// @brief return the reciprocal (1 / v) of each value
PAL_INLINE fvec8 recip( fvec8 v )
{
	return float_constants<fvec8>::one() / v;
}

/// @brief create a mask for any values that are NaN
PAL_INLINE fvec8::mask_type isnan( fvec8 v )
{
	return fvec8::mask_type( _mm256_cmp_ps( v, v, _CMP_UNORD_Q ) );
}

/// @brief create a mask for any values that are NOT NaN
PAL_INLINE fvec8::mask_type isfinite( fvec8 v )
{
	// ! NaN && ! inf
	return fvec8::mask_type( _mm256_cmp_ps( v, _mm256_mul_ps( _mm256_setzero_ps(), v ), _CMP_ORD_Q ) );
}

/// @brief create a mask for any values that are not NaN,
/// not inf, not zero
///
/// TODO: Does not test for subnormals
PAL_INLINE fvec8::mask_type isnormal( fvec8 v )
{
	// hmmm, subnormal returns true w/ cmpord_ps
	return fvec8::mask_type(
		_mm256_and_ps(
			_mm256_cmp_ps( v, _mm256_mul_ps( v, _mm256_setzero_ps() ), _CMP_ORD_Q ),
			_mm256_cmp_ps( v, _mm256_setzero_ps(), _CMP_NEQ_OQ ) )
				   );
}

/// @brief create a mask for any values that infinite
PAL_INLINE fvec8::mask_type isinf( fvec8 v )
{
	// setzero, mul, cmpord, cmpunord, and - 5 instructions, one 0
	// const... could be 4 instructions w/ 2 constants - one to make
	// it absolute, one to test if equal to 0x7F800000, but fewer
	// registers is probably better, right?
	return fvec8::mask_type( _mm256_and_ps(
						_mm256_cmp_ps( v, v, _CMP_ORD_Q ),
						_mm256_cmp_ps( v, _mm256_mul_ps( _mm256_setzero_ps(), v ), _CMP_UNORD_Q ) )
					);
}

/// @brief implement signbit for float values
///
/// This allows for differentiating -0.0 and 0.0
/// returns 1 if has a sign bit set
///
/// NB: only available on sse2 and higher since it returns an int to
/// match the C library
PAL_INLINE lvec8 signbit( fvec8 v )
{
#ifdef PAL_ENABLE_AVX2
	return lvec8( _mm256_srli_epi32( _mm256_castps_si256( v ), 31 ) );
#else
	return lvec8( _mm256_and_epi32(
					  _mm256_cmp_ps(
						  _mm256_and_ps(
							  v,
							  _mm256_set1_epi32( int(0x80000000) ) ),
						  _mm256_set1_epi32( int(0x80000000) ),
						  _CMP_EQ_OQ ),
					  _mm256_set1_epi32( 1 ) ) );
#endif
}

/// @brief should be the same as a single float fabsf
///
/// NaN behavior? NaNs have a sign, but...
PAL_INLINE fvec8 fabsf( fvec8 v )
{
#ifdef PAL_ENABLE_AVX2
	return fvec8( _mm256_castsi256_ps( _mm256_abs_epi32( v.as_int() ) ) );
#else
	return fvec8( _mm256_and_ps( v, lvec8_constants::nonsign_bitmask().as_float() ) );
#endif
}

PAL_INLINE fvec8 sqrtf( fvec8 a )
{
	return fvec8( _mm256_sqrt_ps( a ) );
}

PAL_INLINE fvec8 faster_rsqrtf( fvec8 a )
{
	return fvec8( _mm256_rsqrt_ps( a ) );
}

PAL_INLINE fvec8 fast_rsqrtf( fvec8 a )
{
	fvec8 e = _mm256_rsqrt_ps( a );
	// see the logic in fast_rsqrtf for fvec4
	return fvec8( fma( nmadd( a * e * e, e, e ),
					   float_constants<fvec8>::one_half(), e ) );
}

PAL_INLINE fvec8 rsqrtf( fvec8 a )
{
	return recip( sqrtf( a ) );
}

////////////////////////////////////////

/// @brief truncf per c library
PAL_INLINE fvec8 truncf( fvec8 a )
{
	return _mm256_round_ps( a, (_MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC) );
}

/// @brief floorf per c library
PAL_INLINE fvec8 floorf( fvec8 a )
{
	return _mm256_floor_ps( a );
}

/// @brief ceilf per c library
PAL_INLINE fvec8 ceilf( fvec8 a )
{
	return _mm256_ceil_ps( a );
}

// standard rintf
PAL_INLINE fvec8 rintf( fvec8 a )
{
	return _mm256_round_ps( a, _MM_FROUND_CUR_DIRECTION );
}

// standard nearbyintf
PAL_INLINE fvec8 nearbyintf( fvec8 a )
{
	return _mm256_round_ps( a, ( _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC ) );
}

} // namespace pal

# endif // PAL_HAS_FVEC8

#endif // _PAL_X86_FVEC8_MATH_H_
