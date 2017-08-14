//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/simd_constants.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SIMD_CONSTANTS_H_
# define _PAL_X86_SIMD_CONSTANTS_H_ 1

namespace PAL_NAMESPACE
{

template <typename VT>
struct float_constants
{
	typedef VT vec_type;
	typedef vector_limits<vec_type> limits;
	typedef typename VT::value_type value_type;

	static PAL_INLINE const vec_type zero( void ) { return vec_type::zero(); }

	static PAL_INLINE const vec_type epsilon( void ) { return vec_type( limits::epsilon() ); }
	static PAL_INLINE const vec_type infinity( void ) { return vec_type( limits::infinity() ); }
	static PAL_INLINE const vec_type nan( void ) { return vec_type( limits::quiet_NaN() ); }
	/// usually - max (new in c++11)
	static PAL_INLINE const vec_type lowest( void ) { return vec_type( limits::lowest() ); }
	/// smallest non sub-normal
	static PAL_INLINE const vec_type min( void ) { return vec_type( limits::min() ); }
	static PAL_INLINE const vec_type max( void ) { return vec_type( limits::max() ); }

	static PAL_INLINE const vec_type one_half( void ) { return vec_type( value_type(0.5) ); }
	static PAL_INLINE const vec_type one( void ) { return vec_type( value_type(1.) ); }
	static PAL_INLINE const vec_type two( void ) { return vec_type( value_type(2.) ); }
	static PAL_INLINE const vec_type three( void ) { return vec_type( value_type(3.) ); }
	static PAL_INLINE const vec_type e( void ) { return vec_type( value_type(M_E) ); }
	static PAL_INLINE const vec_type log2_e( void ) { return vec_type( value_type(M_LOG2E) ); }
	static PAL_INLINE const vec_type log10_e( void ) { return vec_type( value_type(M_LOG10E) ); }
	static PAL_INLINE const vec_type log_2( void ) { return vec_type( value_type(M_LN2) ); }
	static PAL_INLINE const vec_type log_10( void ) { return vec_type( value_type(M_LN10) ); }
	static PAL_INLINE const vec_type log_2_10( void ) { return vec_type( value_type(M_LN2/M_LN10) ); }
	static PAL_INLINE const vec_type pi( void ) { return vec_type( value_type(M_PI) ); }
	static PAL_INLINE const vec_type pi_half( void ) { return vec_type( value_type(M_PI_2) ); }
	static PAL_INLINE const vec_type pi_quarter( void ) { return vec_type( value_type(M_PI_4) ); }
	static PAL_INLINE const vec_type two_pi( void ) { return vec_type( value_type(M_PI * 2.0) ); }
	static PAL_INLINE const vec_type one_pi( void ) { return vec_type( value_type(M_1_PI) ); }

	static PAL_INLINE const vec_type sqrt2( void ) { return vec_type( value_type(M_SQRT2) ); }
	static PAL_INLINE const vec_type sqrt1_2( void ) { return vec_type( value_type(M_SQRT1_2) ); }
};

////////////////////////////////////////

template <typename VT>
struct float_extract_constants
{
	static_assert( std::is_floating_point<typename VT::value_type>::value, "floating point extraction constants only valid for floating point vectors" );
	typedef VT vec_type;
	typedef typename VT::int_vec_type ivec_type;
	typedef typename VT::value_type value_type;
	typedef vector_limits<vec_type> limits;

	static PAL_INLINE const ivec_type exponent_mask( void ) { return ivec_type( int32_t(limits::exponent_mask) ); }
	static PAL_INLINE const ivec_type mantissa_mask( void ) { return ivec_type( int32_t(limits::mantissa_mask) ); }
	static PAL_INLINE const ivec_type sign_mask( void ) { return ivec_type( int32_t(limits::sign_mask) ); }
	static PAL_INLINE const ivec_type bias( void ) { return ivec_type( int32_t(limits::exponent_bias) ); }
	// useful for normalizing floats in frexp
	static PAL_INLINE const ivec_type bias_m1( void ) { return ivec_type( int32_t(limits::exponent_bias - 1) ); }
	static PAL_INLINE const ivec_type min_exponent( void ) { return ivec_type( int32_t(limits::min_exponent) ); }
	static PAL_INLINE const ivec_type max_exponent( void ) { return ivec_type( int32_t(limits::max_exponent) ); }
	static PAL_INLINE const ivec_type biased_exponent_range( void ) { return ivec_type( int32_t(limits::max_exponent - limits::min_exponent) ); }
};

////////////////////////////////////////

template <typename VT>
struct int_constants
{
	typedef VT vec_type;
	typedef typename vec_type::bitmask_type bitmask_type;
	typedef typename vec_type::value_type value_type;
	typedef vector_limits<vec_type> limits;

	static PAL_INLINE const vec_type nonsign_bitmask( void ) { return vec_type( bitmask_type(~limits::sign_mask) ); }
	static PAL_INLINE const vec_type sign_bitmask( void ) { return vec_type( bitmask_type(limits::sign_mask) ); }
	static PAL_INLINE const vec_type one( void ) { return vec_type( bitmask_type(1) ); }
	static PAL_INLINE const vec_type neg_one( void ) { return vec_type( bitmask_type(-1) ); }
};

} // namespace pal

#endif // _PAL_X86_SIMD_CONSTANTS_H_
