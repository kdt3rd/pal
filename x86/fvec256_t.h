//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/fvec256_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_FVEC256_T_H_
# define _PAL_X86_FVEC256_T_H_ 1

namespace PAL_NAMESPACE
{

#ifdef PAL_ENABLE_AVX
# define PAL_HAS_FVEC8 1

class fvec8
{
public:
	typedef float value_type;
	typedef uint32_t bitmask_type;
	typedef ivec256<int> int_vec_type;
	typedef mask256<float> mask_type;

	static const int value_count = 8;

	/// @defgroup declare default construction / copy semantics
	/// @{
	fvec8( void ) = default;
	~fvec8( void ) = default;
	fvec8( const fvec8 & ) = default;
	fvec8( fvec8 && ) = default;
	fvec8 &operator=( const fvec8 & ) = default;
	fvec8 &operator=( fvec8 && ) = default;
	/// @}

	explicit PAL_INLINE fvec8( float v ) : _vec( _mm256_set1_ps( v ) ) {}
	PAL_INLINE fvec8( float v0, float v1, float v2, float v3,
					  float v4, float v5, float v6, float v7 )
		: _vec( _mm256_set_ps( v7, v6, v5, v4, v3, v2, v1, v0 ) )
	{}
	template <size_t N>
	explicit PAL_INLINE fvec8( const float(&a)[N] )
		: _vec( _mm256_set_ps( a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0] ) )
	{
		// TODO: should we handle different ones and zero fill? meh.
		static_assert( N == 8, "fvec8 needs 8 floats in initializer" );
	}

	template <typename U, size_t N>
	explicit PAL_INLINE fvec8( const U(&a)[N] )
	: _vec( _mm256_set_ps( static_cast<float>( a[7] ),
						   static_cast<float>( a[6] ),
						   static_cast<float>( a[5] ),
						   static_cast<float>( a[4] ),
						   static_cast<float>( a[3] ),
						   static_cast<float>( a[2] ),
						   static_cast<float>( a[1] ),
						   static_cast<float>( a[0] )) )
	{
		static_assert( N == 8, "fvec8 needs 8 floats in initializer" );
	}

	template <size_t N>
	explicit PAL_INLINE fvec8( const std::array<float, N> &a )
		: _vec( _mm256_set_ps( a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0] ) )
	{
		// TODO: should we handle different ones and zero fill? meh.
		static_assert( N == 8, "fvec8 needs 8 floats in initializer" );
	}
	explicit PAL_INLINE fvec8( bitmask_type v )
		: _vec( _mm256_castsi256_ps( _mm256_set1_epi32( static_cast<int>(v) ) ) )
	{}

	PAL_INLINE fvec8( __m128 a, __m128 b ) : _vec( _mm256_insertf128_ps( _mm256_castps128_ps256( a ), b, 1 ) ) {}
	PAL_INLINE fvec8( __m256 v ) : _vec( v ) {}
	PAL_INLINE fvec8( mask_type v ) : _vec( v ) {}

	/// @brief enable transparent calls to intrinsic functions
	PAL_INLINE operator __m256( void ) const { return _vec; }
	PAL_INLINE int_vec_type as_int( void ) const { return int_vec_type( _mm256_castps_si256( _vec ) ); }

	PAL_INLINE fvec8 &operator=( float v ) { _vec = _mm256_set1_ps( v ); }

	PAL_INLINE float operator[]( int i ) const
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v8sf)_vec)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256 vec;
			float vals[8];
		};
		vec = _vec;
		return vals[i];
#else
		PAL_ALIGN_256 float vals[8];
		_mm256_store_ps( vals, _vec );
		return vals[i];
#endif
	}

	PAL_INLINE fvec8 &operator+=( fvec8 a )
	{
		_vec = _mm256_add_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec8 &operator+=( float v )
	{
		_vec = _mm256_add_ps( _vec, _mm256_set1_ps( v ) );
		return *this;
	}
	PAL_INLINE fvec8 &operator-=( fvec8 a )
	{
		_vec = _mm256_sub_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec8 &operator-=( float v )
	{
		_vec = _mm256_sub_ps( _vec, _mm256_set1_ps( v ) );
		return *this;
	}
	PAL_INLINE fvec8 &operator*=( fvec8 a )
	{
		_vec = _mm256_mul_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec8 &operator*=( float v )
	{
		_vec = _mm256_mul_ps( _vec, _mm256_set1_ps( v ) );
		return *this;
	}

	PAL_INLINE fvec8 &operator/=( fvec8 a )
	{
		_vec = _mm256_div_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec8 &operator/=( float v )
	{
		_vec = _mm256_div_ps( _vec, _mm256_set1_ps( v ) );
		return *this;
	}

	static PAL_INLINE fvec8 zero( void ) { return fvec8( _mm256_setzero_ps() ); }
	static PAL_INLINE fvec8 splat( float v ) { return fvec8( _mm256_set1_ps( v ) ); }

	static PAL_INLINE fvec8 convert_int( int_vec_type v ) { return fvec8( _mm256_cvtepi32_ps( v ) ); }
private:
	__m256 _vec;
};

// see all the operators defined in sse_fvec_operators.h

/// @brief declare a specialization of vector_limits for fvec8
template <> struct vector_limits<fvec8> : public std::numeric_limits<float>
{
	static_assert( radix == 2, "expect a power 2 radix" );
	static_assert( sizeof(float) == 4, "expect a 32-bit float" );

	static const int bits = 256;
	static const int bytes = 32;
	static const int value_count = 8;

	static const int value_bits = 32;
	static const int mantissa_bits = digits - 1;
	static const int sign_bits = 1;
	static const int exponent_bits = value_bits - mantissa_bits - sign_bits;
	static const int exponent_bias = (1 << (exponent_bits-1)) - 1;
	static const uint32_t mantissa_mask = (uint32_t(1) << mantissa_bits) - 1;
	static const uint32_t exponent_mask = ((uint32_t(1) << exponent_bits) - 1) << mantissa_bits;
	static const uint32_t sign_mask = uint32_t(1) << (value_bits - 1);
};

#endif

} // namespace pal

#endif // _PAL_X86_FVEC256_T_H_
