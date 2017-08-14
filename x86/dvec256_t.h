//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/dvec256_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_DVEC256_T_H_
# define _PAL_X86_DVEC256_T_H_ 1

namespace PAL_NAMESPACE
{

#ifdef PAL_ENABLE_AVX
# define PAL_HAS_DVEC4 1

struct dvec4_constants;
class dvec4
{
public:
	typedef double value_type;
	typedef uint64_t bitmask_type;
	typedef ivec256<int64_t> int_vec_type;
	typedef mask256<double> mask_type;
	typedef dvec4_constants constant_holder;

	static const int value_count = 4;

	/// @defgroup declare default construction / copy semantics
	/// @{
	dvec4( void ) = default;
	~dvec4( void ) = default;
	dvec4( const dvec4 & ) = default;
	dvec4( dvec4 && ) = default;
	dvec4 &operator=( const dvec4 & ) = default;
	dvec4 &operator=( dvec4 && ) = default;
	/// @}

	explicit PAL_INLINE dvec4( value_type v ) : _vec( _mm256_set1_pd( v ) ) {}
	PAL_INLINE dvec4( value_type v0, value_type v1, value_type v2, value_type v3 ) : _vec( _mm256_set_pd( v3, v2, v1, v0 ) ) {}
	explicit PAL_INLINE dvec4( __m256d v ) : _vec( v ) {}
	PAL_INLINE dvec4( mask_type v ) : _vec( v ) {}

	/// @brief enable transparent calls to intrinsic functions
	PAL_INLINE operator __m256d( void ) const { return _vec; }
	PAL_INLINE int_vec_type as_int( void ) const { return int_vec_type( _mm256_castpd_si256( _vec ) ); }

	PAL_INLINE dvec4 &operator=( value_type v ) { _vec = _mm256_set1_pd( v ); return *this; }
	PAL_INLINE dvec4 &operator=( __m256d v ) { _vec = v; return *this; }

	PAL_INLINE value_type operator[]( int i ) const
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v4df)_vec)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256d vec;
			value_type vals[4];
		};
		vec = _vec;
		return vals[i];
#else
		PAL_ALIGN_256 value_type vals[4];
		_mm256_store_pd( vals, _vec );
		return vals[i];
#endif
	}

	PAL_INLINE dvec4 &operator+=( dvec4 a )
	{
		_vec = _mm256_add_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec4 &operator+=( value_type v )
	{
		_vec = _mm256_add_pd( _vec, _mm256_set1_pd( v ) );
		return *this;
	}
	PAL_INLINE dvec4 &operator-=( dvec4 a )
	{
		_vec = _mm256_sub_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec4 &operator-=( value_type v )
	{
		_vec = _mm256_sub_pd( _vec, _mm256_set1_pd( v ) );
		return *this;
	}
	PAL_INLINE dvec4 &operator*=( dvec4 a )
	{
		_vec = _mm256_mul_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec4 &operator*=( value_type v )
	{
		_vec = _mm256_mul_pd( _vec, _mm256_set1_pd( v ) );
		return *this;
	}

	PAL_INLINE dvec4 &operator/=( dvec4 a )
	{
		_vec = _mm256_div_pd( _vec, a._vec );
		return *this;
	}
	PAL_INLINE dvec4 &operator/=( value_type v )
	{
		_vec = _mm256_div_pd( _vec, _mm256_set1_pd( v ) );
		return *this;
	}

	static PAL_INLINE dvec4 zero( void ) { return dvec4( _mm256_setzero_pd() ); }
	static PAL_INLINE dvec4 splat( value_type v ) { return dvec4( v ); }

	// converts the all 4 int32s to doubles
	static PAL_INLINE dvec4 convert_int32( __m128i v ) { return dvec4( _mm256_cvtepi32_pd( v ) ); }
	// TODO: add other integer conversions based on AVX512 features
private:
	__m256d _vec;
};

// see all the operators defined in sse_dvec_operators.h

/// @brief declare a specialization of vector_limits for dvec4
template <> struct vector_limits<dvec4> : public std::numeric_limits<dvec4::value_type>
{
	static_assert( radix == 2, "expect a power 2 radix" );
	static_assert( sizeof(double) == 8, "expect a 64-bit double" );
	typedef double value_type;

	static const int bits = 256;
	static const int bytes = 32;
	static const int value_count = 4;

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

#endif // _PAL_X86_DVEC256_T_H_
