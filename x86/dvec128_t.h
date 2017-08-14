//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/dvec128_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_DVEC128_T_H_
# define _PAL_X86_DVEC128_T_H_ 1

namespace PAL_NAMESPACE
{

#ifdef PAL_ENABLE_SSE
# define PAL_HAS_DVEC2 1

struct dvec2_constants;
class dvec2
{
public:
	typedef double value_type;
	typedef uint64_t bitmask_type;
	typedef ivec128<int64_t> int_vec_type;
	typedef mask128<value_type> mask_type;
	typedef dvec2_constants constant_holder;

	static const int value_count = 2;

	/// @defgroup declare default construction / copy semantics
	/// @{
	dvec2( void ) = default;
	~dvec2( void ) = default;
	dvec2( const dvec2 & ) = default;
	dvec2( dvec2 && ) = default;
	dvec2 &operator=( const dvec2 & ) = default;
	dvec2 &operator=( dvec2 && ) = default;
	/// @}

	explicit PAL_INLINE dvec2( value_type v ) : _vec( _mm_set1_pd( v ) ) {}
	PAL_INLINE dvec2( value_type v0, value_type v1 ) : _vec( _mm_set_pd( v1, v0 ) ) {}
	PAL_INLINE dvec2( __m128d v ) : _vec( v ) {}

	/// @brief enable transparent calls to intrinsic functions
	PAL_INLINE operator __m128d( void ) const { return _vec; }
	PAL_INLINE int_vec_type as_int( void ) const { return int_vec_type( _mm_castpd_si128( _vec ) ); }

	PAL_INLINE dvec2 &operator=( value_type v ) { _vec = _mm_set1_pd( v ); return *this; }
	PAL_INLINE dvec2 &operator=( __m128d v ) { _vec = v; return *this; }

	PAL_INLINE value_type operator[]( int i ) const
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v2df)_vec)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128d vec;
			value_type vals[2];
		};
		vec = _vec;
		return vals[i];
#else
		PAL_ALIGN_128 value_type vals[2];
		_mm_store_pd( vals, _vec );
		return vals[i];
#endif
	}

	PAL_INLINE dvec2 &operator+=( dvec2 a )
	{
		_vec = _mm_add_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec2 &operator+=( value_type v )
	{
		_vec = _mm_add_pd( _vec, _mm_set1_pd( v ) );
		return *this;
	}
	PAL_INLINE dvec2 &operator-=( dvec2 a )
	{
		_vec = _mm_sub_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec2 &operator-=( value_type v )
	{
		_vec = _mm_sub_pd( _vec, _mm_set1_pd( v ) );
		return *this;
	}
	PAL_INLINE dvec2 &operator*=( dvec2 a )
	{
		_vec = _mm_mul_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec2 &operator*=( value_type v )
	{
		_vec = _mm_mul_pd( _vec, _mm_set1_pd( v ) );
		return *this;
	}

	PAL_INLINE dvec2 &operator/=( dvec2 a )
	{
		_vec = _mm_div_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec2 &operator/=( value_type v )
	{
		_vec = _mm_div_pd( _vec, _mm_set1_pd( v ) );
		return *this;
	}

	static PAL_INLINE dvec2 zero( void ) { return dvec2( _mm_setzero_pd() ); }
	static PAL_INLINE dvec2 splat( value_type v ) { return dvec2( v ); }

	// converts the first 2 ints to doubles
	static PAL_INLINE dvec2 convert_int32( int_vec_type v ) { return dvec2( _mm_cvtepi32_pd( v ) ); }
	// converts the int64_t to double
	static PAL_INLINE dvec2 convert_int64( __m128i v ) { return dvec2( _mm_cvtepi64_pd( v ) ); }
private:
	__m128d _vec;
};

// see all the operators defined in sse_dvec_operators.h

/// @brief declare a specialization of vector_limits for dvec2
template <> struct vector_limits<dvec2> : public std::numeric_limits<dvec2::value_type>
{
	static_assert( radix == 2, "expect a power 2 radix" );
	static_assert( sizeof(double) == 8, "expect a 64-bit double" );
	typedef double value_type;

	static const int bits = 128;
	static const int bytes = 16;
	static const int value_count = 2;

	static const int value_bits = 64;
	static const int mantissa_bits = digits - 1;
	static const int sign_bits = 1;
	static const int exponent_bits = value_bits - mantissa_bits - sign_bits;
	static const int exponent_bias = (1 << (exponent_bits-1)) - 1;
	static const uint64_t mantissa_mask = (uint64_t(1) << mantissa_bits) - 1;
	static const uint64_t exponent_mask = ((uint64_t(1) << exponent_bits) - 1) << mantissa_bits;
	static const uint64_t sign_mask = uint64_t(1) << (value_bits - 1);
};

#endif // PAL_ENABLE_SSE

} // namespace pal

#endif // _PAL_X86_DVEC128_T_H_
