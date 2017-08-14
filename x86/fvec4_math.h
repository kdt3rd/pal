//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/fvec4_math.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_FVEC4_MATH_H_
# define _PAL_X86_SSE_FVEC4_MATH_H_ 1

namespace PAL_NAMESPACE
{


// contains the templated routines that work for fvec4 and fvec8

/// @brief return the max of the 2 numbers. if one is NaN, return the other?
PAL_INLINE fvec4 max( fvec4 a, fvec4 b ) { return fvec4( _mm_max_ps( a, b ) ); }
/// @brief return the min of the 2 numbers. if one is NaN, return the other?
PAL_INLINE fvec4 min( fvec4 a, fvec4 b ) { return fvec4( _mm_min_ps( a, b ) ); }

/// @brief apply a * b + c
PAL_INLINE fvec4 fma( fvec4 a, fvec4 b, fvec4 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec4( _mm_fmadd_ps( a, b, c ) );
#else
	return fvec4( _mm_add_ps( _mm_mul_ps( a, b ), c ) );
#endif
}

/// @brief apply a * b - c
PAL_INLINE fvec4 fms( fvec4 a, fvec4 b, fvec4 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec4( _mm_fmsub_ps( a, b, c ) );
#else
	return fvec4( _mm_sub_ps( _mm_mul_ps( a, b ), c ) );
#endif
}

/// @brief apply -( a * b ) + c
PAL_INLINE fvec4 nmadd( fvec4 a, fvec4 b, fvec4 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec4( _mm_fnmadd_ps( a, b, c ) );
#else
	// hmmm, this is same as c - ( a * b )
	return fvec4( _mm_sub_ps( c, _mm_mul_ps( a, b ) ) );
#endif
}

/// @brief apply -(a * b) - c
PAL_INLINE fvec4 nmsub( fvec4 a, fvec4 b, fvec4 c )
{
#ifdef PAL_ENABLE_FMA_EXT
	return fvec4( _mm_fnmsub_ps( a, b, c ) );
#else
	return -( a * b ) - c;
#endif
}

/// @brief computes a horizontal sum of the 4 elements of the vector
PAL_INLINE float hsum( fvec4 v )
{
#ifdef PAL_ENABLE_SSE3
	__m128 a = _mm_hadd_ps( v, v );
	a = _mm_hadd_ps( a, a );
	return _mm_cvtss_f32( a );
	// this may be faster if uops are an issue
	// and the latency might be lower?
//	__m128 swap1 = _mm_movehdup_ps( v );
//	__m128 sum = _mm_add_ps( v, swap1 );
//	swap1 = _mm_movehl_ps( swap1, sum );
//	return _mm_cvtss_f32( _mm_add_ss( sum, swap1 ) );
#else
	__m128 tmp = _mm_add_ps( v, _mm_movehl_ps( v, v ) );
	return _mm_cvtss_f32( _mm_add_ss( tmp, _mm_shuffle_ps( tmp, tmp, 1 ) ) );
#endif
}

/// @brief return a fast (estimate) reciprocal (1 / v) of each value
///
/// has error less than 1.5*2^-12. so up to 4096 ULP
PAL_INLINE fvec4 faster_recip( fvec4 v )
{
	return fvec4( _mm_rcp_ps( v ) );
}

/// @brief return reciprocal (1 / v) of each value
///
/// This is a bit more accurate than the standard rcp_ps operation
/// as it does a round of newton-raphson refinement
/// should be accurate for float to ~1 ULP
PAL_INLINE fvec4 fast_recip( fvec4 v )
{
	fvec4 e = fvec4( _mm_rcp_ps( v ) );
	// one round of newton-raphson refinement
	// e_n = 2 * e - v * e^2
	//         1   1   1  1
	//       2 * e == (e + e)
	//   so
	//     = (e+e) - v * e * e
	// or 2-3 instructions and a constant
	//     e * ( 2 - v * e )
	//       1     1   1
	// let's assume that the version with no constant
	// is actually best since it saves a register?
	// and on machines with FMA, it goes down to 3 instr
	return nmadd( v, e * e, e + e );
//	static const fvec4 two( 2.F );
//	return nmadd( e, v, two ) * e;
}

/// @brief computes reciprocal 1/v for each value
PAL_INLINE fvec4 recip( fvec4 v )
{
	return float_constants<fvec4>::one() / v;
}

/// @brief create a mask for any values that are NaN
PAL_INLINE fvec4::mask_type isnan( fvec4 v )
{
	// AVX has _mm_cmp_ps, but that currently has the same
	// latency and throughput as this, so don't bother switching
	// the implementation
	return fvec4::mask_type( _mm_cmpunord_ps( v, v ) );
}

/// @brief create a mask for any values that are NOT NaN
PAL_INLINE fvec4::mask_type isfinite( fvec4 v )
{
	// ! NaN && ! inf
	return fvec4::mask_type( _mm_cmpord_ps( v, _mm_mul_ps( _mm_setzero_ps(), v ) ) );
}

/// @brief create a mask for any values that are not NaN,
/// not inf, not zero
///
/// TODO: Does not test for subnormals
PAL_INLINE fvec4::mask_type isnormal( fvec4 v )
{
	// hmmm, subnormal returns true w/ cmpord_ps?
	return fvec4::mask_type(
		_mm_and_ps(
			_mm_cmpord_ps( v, _mm_mul_ps( v, _mm_setzero_ps() ) ),
			_mm_cmpneq_ps( v, _mm_setzero_ps() ) )
				   );
}

/// @brief create a mask for any values that infinite
PAL_INLINE fvec4::mask_type isinf( fvec4 v )
{
	// setzero, mul, cmpord, cmpunord, and - 5 instructions, one 0
	// const... could be 4 instructions w/ 2 constants - one to make
	// it absolute, one to test if equal to 0x7F800000, but fewer
	// registers is probably better, right?
	return fvec4::mask_type( _mm_and_ps(
						_mm_cmpord_ps( v, v ),
						_mm_cmpunord_ps( v, _mm_mul_ps( _mm_setzero_ps(), v ) ) )
					);
}

/// @brief assigns each integer vector according to the results of fpclassify
///
/// FP_SUBNORMAL, FP_ZERO, FP_NAN, FP_INFINITE
PAL_INLINE lvec4 fpclassify( fvec4 v )
{
	static const lvec4 mask = lvec4( 0xFF, 0xFF, 0xFF, 0xFF );
	lvec4 e = _mm_and_si128( _mm_srli_epi32( v.as_int(), 23 ), mask );
	// if e == 0 but v << 1 > 0 -> FP_SUBNORMAL
	// else if e == 0 -> FP_ZERO
	// else if e == 0xFF and (v << 9) != 0 ? FP_NAN
	// else if e == 0xFF -> FP_INFINITE
	// else -> FP_NORMAL
	lvec4 ezero = _mm_cmpeq_epi32( e, _mm_setzero_si128() );
	lvec4 e255 = _mm_cmpeq_epi32( e, mask );
	lvec4 vsl1gt0 = _mm_cmpgt_epi32( _mm_sll_epi32( _mm_castps_si128( v ), _mm_cvtsi32_si128( 1 ) ), _mm_setzero_si128() );
	lvec4 esl9eqzero = _mm_cmpeq_epi32( _mm_sll_epi32( _mm_castps_si128( v ), _mm_cvtsi32_si128( 9 ) ), _mm_setzero_si128() );

	lvec4 issub = ezero & vsl1gt0;
	lvec4 iszero = _mm_andnot_si128( vsl1gt0, ezero );
	lvec4 isinf = e255 & esl9eqzero;
	lvec4 isnan = _mm_andnot_si128( esl9eqzero, e255 );
	lvec4 isnorm = _mm_andnot_si128( issub | iszero | isinf | isnan, _mm_set1_epi32( FP_NORMAL ) );

	return ( ( issub & lvec4::splat( FP_SUBNORMAL ) ) |
			 ( iszero & lvec4::splat( FP_ZERO ) ) |
			 ( isinf & lvec4::splat( FP_INFINITE ) ) |
			 ( isnan & lvec4::splat( FP_NAN ) ) |
			 isnorm );
}

/// @brief implement signbit for float values
///
/// This allows for differentiating -0.0 and 0.0
/// returns 1 if has a sign bit set
///
/// NB: only available on sse2 and higher since it returns an int to
/// match the C library
PAL_INLINE lvec4 signbit( fvec4 v )
{
	return lvec4( _mm_srli_epi32( v.as_int(), 31 ) );
}

/// @brief computes the sqrt of all values
PAL_INLINE fvec4 sqrtf( fvec4 a )
{
	return fvec4( _mm_sqrt_ps( a ) );
}

/// @brief computes a fast reciprocal sqrt 1/sqrt(v) for each value
PAL_INLINE fvec4 faster_rsqrtf( fvec4 a )
{
	return fvec4( _mm_rsqrt_ps( a ) );
}

/// @brief computes reciprocal sqrt 1/sqrt(v) for each value
///
/// uses a faster method and adds a round of NR refinement to
/// improve results
///
/// NB: does NOT handle infinity correctly as input, will produce NaN
/// on output
PAL_INLINE fvec4 fast_rsqrtf( fvec4 a )
{
	__m128 e = _mm_rsqrt_ps( a );
	// one round of newton-raphson
	// e_n = e * ( 3 - x * e^2 ) / 2
	//         1     1   1  1    1
	//     = 3e/2 - x*e^3/2
	// gives alternate
	// e_n = e + (e - x * e^3) / 2
	//         1    1   1  2   1
	// which only has 1 constant instead of 2
	// so 1 more instruction instead of a load
	// but on FMA hardware, you can do 2 ternery ops
	// bringing it back to same instr count but
	// one fewer constant
	return fma( nmadd( a * e * e, e, e ),
				float_constants<fvec4>::one_half(), e );
	// could add this to handle inf, but are the extra ops worth it?
//	fvec4::mask_type i = isinf( a );
//	return i.blend( r, fvec4::zero() );
}

/// @brief computes reciprocal sqrt 1/sqrt(v) for each value
PAL_INLINE fvec4 rsqrtf( fvec4 a )
{
	return recip( sqrtf( a ) );
}

////////////////////////////////////////

PAL_INLINE fvec4 truncf( fvec4 a )
{
#ifdef __SSE4_1__
	return _mm_round_ps( a, (_MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC ) );
#else
	return fvec4::convert_int( a.convert_to_int_trunc() );
#endif
}

// standard vectorized floor
PAL_INLINE fvec4 floorf( fvec4 a )
{
#ifdef __SSE4_1__
	return _mm_floor_ps( a );
#else
	typedef fvec4::int_vec_type ivec;
	ivec b = _mm_slli_epi32( _mm_srli_epi32( ivec::mask_type::yes(), 25 ), 23 );
	fvec4 j = b.as_float();
	fvec4 k = fvec4::convert_int( a.convert_to_int_trunc() );
	return k - (j & (k > a));
#endif
}

// standard vectorized ceilf
PAL_INLINE fvec4 ceilf( fvec4 a )
{
#ifdef __SSE4_1__
	return _mm_ceil_ps( a );
#else
	typedef fvec4::int_vec_type ivec;
	ivec b = _mm_slli_epi32( _mm_srli_epi32( ivec::mask_type::yes(), 25 ), 23 );
	fvec4 j = b.as_float();
	fvec4 k = fvec4::convert_int( a.convert_to_int_trunc() );
	return k + (j & (k < a));
#endif
}

// standard rintf
// TODO: deal w/ rounding mode for < sse 4.1
// NB: reminder that this is "banker's rounding" per IEEE
// so 3.5 == 4 but 4.5 == 4
PAL_INLINE fvec4 rintf( fvec4 a )
{
#ifdef __SSE4_1__
	return _mm_round_ps( a, _MM_FROUND_CUR_DIRECTION );
#else
	typedef fvec4::int_vec_type ivec;
	typedef vector_limits<fvec4> limits;
	typedef float_extract_constants<fvec4> feconst;
	typedef int_constants<ivec> iconst;

	fvec4 remfrac = fvec4( 1.F / std::numeric_limits<float>::epsilon() );
	ivec e = lsr( a.as_int(), limits::mantissa_bits ) & ivec( 0xFF );

	fvec4 yneg = a - remfrac + remfrac;
	fvec4 ypos = a + remfrac - remfrac;
	fvec4::mask_type isneg = a < fvec4::zero();
	fvec4 ret = isneg.blend( ypos, yneg );
	fvec4::mask_type isbig = fvec4::mask_type( e >= ivec( 0x7F + 23 ) );
	return isbig.blend( ret, a );
#endif
}

// standard nearbyintf
// NB: reminder that this is "banker's rounding" per IEEE
// so 3.5 == 4 but 4.5 == 4
PAL_INLINE fvec4 nearbyintf( fvec4 a )
{
#ifdef __SSE4_1__
	return _mm_round_ps( a, ( _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC ) );
#else
	return rintf( a );
#endif
}

} // namespace pal

#endif // _PAL_X86_FVEC4_MATH_H_
