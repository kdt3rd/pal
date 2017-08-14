//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/fvec128_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_FVEC128_T_H_
# define _PAL_X86_FVEC128_T_H_ 1

namespace PAL_NAMESPACE
{

# define PAL_HAS_FVEC4 1

class fvec4
{
public:
	typedef float value_type;
	typedef uint32_t bitmask_type;
	typedef ivec128<int> int_vec_type;
	typedef mask128<float> mask_type;

	static const int value_count = 4;

	/// @defgroup declare default construction / copy semantics
	/// @{
	fvec4( void ) = default;
	~fvec4( void ) = default;
	fvec4( const fvec4 & ) = default;
	fvec4( fvec4 && ) = default;
	fvec4 &operator=( const fvec4 & ) = default;
	fvec4 &operator=( fvec4 && ) = default;
	/// @}

	explicit PAL_INLINE fvec4( float v ) : _vec( _mm_set1_ps( v ) ) {}
	PAL_INLINE fvec4( float v0, float v1, float v2, float v3 ) : _vec( _mm_set_ps( v3, v2, v1, v0 ) ) {}
	template <size_t N>
	explicit PAL_INLINE fvec4( const float(&a)[N] )
		: _vec( _mm_set_ps( a[3], a[2], a[1], a[0] ) )
	{
		// TODO: should we handle different ones and zero fill? meh.
		static_assert( N == 4, "fvec4 needs 4 floats in initializer" );
	}

	explicit PAL_INLINE fvec4( bitmask_type v )
		: _vec( _mm_castsi128_ps( _mm_set1_epi32( static_cast<int>(v) ) ) )
	{}

	template <typename U, size_t N>
	explicit PAL_INLINE fvec4( const U(&a)[N] )
	: _vec( _mm_set_ps( static_cast<float>( a[3] ),
						static_cast<float>( a[2] ),
						static_cast<float>( a[1] ),
						static_cast<float>( a[0] ) ) )
	{}

	PAL_INLINE fvec4( const std::array<float, 4> &a ) : _vec( _mm_set_ps( a[3], a[2], a[1], a[0] ) ) {}

	PAL_INLINE fvec4( __m128 v ) : _vec( v ) {}

	/// @brief enable transparent calls to intrinsic functions
	PAL_INLINE operator __m128( void ) const { return _vec; }
			  
	PAL_INLINE __m128i as_int( void ) const { return _mm_castps_si128( _vec ); }
	PAL_INLINE __m128 as_float( void ) const { return _vec; }
	PAL_INLINE __m128d as_double( void ) const { return _mm_castps_pd( _vec ); }

	PAL_INLINE fvec4 &operator=( float v ) { _vec = _mm_set1_ps( v ); return *this; }
	PAL_INLINE fvec4 &operator=( __m128 x ) { _vec = x; return *this; }

	PAL_INLINE float operator[]( int i ) const
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v4sf)_vec)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128 vec;
			float vals[4];
		};
		vec = _vec;
		return vals[i];
#else
		PAL_ALIGN_128 float vals[4];
		_mm_store_ps( vals, _vec );
		return vals[i];
#endif
	}

	PAL_INLINE fvec4 &operator+=( fvec4 a )
	{
		_vec = _mm_add_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec4 &operator+=( float v )
	{
		_vec = _mm_add_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}
	PAL_INLINE fvec4 &operator-=( fvec4 a )
	{
		_vec = _mm_sub_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec4 &operator-=( float v )
	{
		_vec = _mm_sub_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}
	PAL_INLINE fvec4 &operator*=( fvec4 a )
	{
		_vec = _mm_mul_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec4 &operator*=( float v )
	{
		_vec = _mm_mul_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}

	PAL_INLINE fvec4 &operator/=( fvec4 a )
	{
		_vec = _mm_div_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec4 &operator/=( float v )
	{
		_vec = _mm_div_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}

	PAL_INLINE int_vec_type convert_to_int( void ) const { return int_vec_type( _mm_cvtps_epi32( _vec ) ); }
	PAL_INLINE int_vec_type convert_to_int_trunc( void ) const { return int_vec_type( _mm_cvttps_epi32( _vec ) ); }

	static PAL_INLINE fvec4 zero( void ) { return fvec4( _mm_setzero_ps() ); }
	static PAL_INLINE fvec4 splat( float v ) { return fvec4( _mm_set1_ps( v ) ); }

	static PAL_INLINE fvec4 convert_int( int_vec_type v ) { return fvec4( _mm_cvtepi32_ps( v ) ); }
private:
	__m128 _vec;
};

std::ostream &operator<<( std::ostream &os, fvec4 v )
{
	os << "{ " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << " }";
	return os;
}

// see all the operators defined in sse_fvec_operators.h

/// @brief declare a specialization of vector_limits for fvec4
template <> struct vector_limits<fvec4> : public std::numeric_limits<float>
{
	static_assert( radix == 2, "expect a power 2 radix" );
	static_assert( sizeof(float) == 4, "expect a 32-bit float" );
	typedef float value_type;

	static const int bits = 128;
	static const int bytes = 16;
	static const int value_count = 4;

	static const int value_bits = 32;
	static const int mantissa_bits = digits - 1;
	static const int sign_bits = 1;
	static const int exponent_bits = value_bits - mantissa_bits - sign_bits;
	static const int exponent_bias = (1 << (exponent_bits-1)) - 1;
	static const uint32_t mantissa_mask = (uint32_t(1) << mantissa_bits) - 1;
	static const uint32_t exponent_mask = ((uint32_t(1) << exponent_bits) - 1) << mantissa_bits;
	static const uint32_t sign_mask = uint32_t(1) << (value_bits - 1);
};

} // namespace pal

#endif // _PAL_X86_FVEC128_T_H_
