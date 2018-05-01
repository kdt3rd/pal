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

/// @brief 128-bit floating point vector
///
/// Represents 4 floating point values. 
///
/// Discussion: we could templatize this if need be, but there is really only
/// floats here if intel (or another architecture) starts offering half
/// float, it may make some sense to do at that point (and then add the
/// same detail implementation as we do for integers)
class fvec128
{
public:
	using ftype = float; // instead of a template argument

	static_assert( std::is_floating_point<ftype>::value && sizeof(ftype) == 4, "expecting float type for now" );

	using value_type = ftype;
	using bitmask_type = uint32_t;
	using int_vec_type = ivec128<int>;
	using mask_type = mask128<value_type>;

	static const int value_count = 4;

	/// @defgroup declare default construction / copy semantics
	/// @{
	fvec128( void ) = default;
	~fvec128( void ) = default;
	fvec128( const fvec128 & ) = default;
	fvec128( fvec128 && ) = default;
	fvec128 &operator=( const fvec128 & ) = default;
	fvec128 &operator=( fvec128 && ) = default;
	/// @}

	explicit PAL_INLINE fvec128( value_type v ) : _vec( _mm_set1_ps( v ) ) {}
	PAL_INLINE fvec128( value_type v0, value_type v1, value_type v2, value_type v3 ) : _vec( _mm_set_ps( v3, v2, v1, v0 ) ) {}
	template <size_t N>
	explicit PAL_INLINE fvec128( const value_type(&a)[N] )
		: _vec( _mm_set_ps( a[3], a[2], a[1], a[0] ) )
	{
		// TODO: should we handle different ones and zero fill? meh.
		static_assert( N == 4, "fvec128 needs 4 floats in initializer" );
	}

	explicit PAL_INLINE fvec128( bitmask_type v )
		: _vec( _mm_castsi128_ps( _mm_set1_epi32( static_cast<int>(v) ) ) )
	{}

	template <typename U, size_t N>
	explicit PAL_INLINE fvec128( const U(&a)[N] )
	: _vec( _mm_set_ps( static_cast<value_type>( a[3] ),
						static_cast<value_type>( a[2] ),
						static_cast<value_type>( a[1] ),
						static_cast<value_type>( a[0] ) ) )
	{}

	PAL_INLINE fvec128( const std::array<value_type, 4> &a ) : _vec( _mm_set_ps( a[3], a[2], a[1], a[0] ) ) {}

	PAL_INLINE fvec128( __m128 v ) : _vec( v ) {}

	/// @brief enable transparent calls to intrinsic functions
	PAL_INLINE operator __m128( void ) const { return _vec; }
			  
	PAL_INLINE __m128i as_int( void ) const { return _mm_castps_si128( _vec ); }
	PAL_INLINE __m128 as_float( void ) const { return _vec; }
	PAL_INLINE __m128d as_double( void ) const { return _mm_castps_pd( _vec ); }

	PAL_INLINE fvec128 &operator=( value_type v ) { _vec = _mm_set1_ps( v ); return *this; }
	PAL_INLINE fvec128 &operator=( __m128 x ) { _vec = x; return *this; }

	template <int i>
	void set( value_type v )
	{
#ifdef PAL_ENABLE_SSE4_1
		_vec = _mm_insert_ps( _vec, _mm_set1_ps(v), uint8_t(i<<4) );
#else
		_vec = _mm_set_ps( (i == 3) ? v : ((__v4sf)_vec)[3],
						   (i == 2) ? v : ((__v4sf)_vec)[2],
						   (i == 1) ? v : ((__v4sf)_vec)[1],
						   (i == 0) ? v : ((__v4sf)_vec)[0] );
#endif
	}

	template <int i>
	value_type get() const
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v4sf)_vec)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union
		{
			__m128 vec;
			value_type vals[4];
		};
		vec = _vec;
		return vals[i];
#else
		PAL_ALIGN_128 value_type vals[4];
		_mm_store_ps( vals, _vec );
		return vals[i];
#endif
	}

	PAL_INLINE value_type operator[]( int i ) const
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v4sf)_vec)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union
		{
			__m128 vec;
			value_type vals[4];
		};
		vec = _vec;
		return vals[i];
#else
		PAL_ALIGN_128 value_type vals[4];
		_mm_store_ps( vals, _vec );
		return vals[i];
#endif
	}

	PAL_INLINE fvec128 &operator+=( fvec128 a )
	{
		_vec = _mm_add_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec128 &operator+=( value_type v )
	{
		_vec = _mm_add_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}
	PAL_INLINE fvec128 &operator-=( fvec128 a )
	{
		_vec = _mm_sub_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec128 &operator-=( value_type v )
	{
		_vec = _mm_sub_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}
	PAL_INLINE fvec128 &operator*=( fvec128 a )
	{
		_vec = _mm_mul_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec128 &operator*=( value_type v )
	{
		_vec = _mm_mul_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}

	PAL_INLINE fvec128 &operator/=( fvec128 a )
	{
		_vec = _mm_div_ps( _vec, a._vec );
		return *this;
	}
	PAL_INLINE fvec128 &operator/=( value_type v )
	{
		_vec = _mm_div_ps( _vec, _mm_set1_ps( v ) );
		return *this;
	}

	PAL_INLINE int_vec_type convert_to_int( void ) const { return int_vec_type( _mm_cvtps_epi32( _vec ) ); }
	PAL_INLINE int_vec_type convert_to_int_trunc( void ) const { return int_vec_type( _mm_cvttps_epi32( _vec ) ); }

	static PAL_INLINE fvec128 zero( void ) { return fvec128( _mm_setzero_ps() ); }
	static PAL_INLINE fvec128 splat( value_type v ) { return fvec128( _mm_set1_ps( v ) ); }

	static PAL_INLINE fvec128 convert_int( int_vec_type v ) { return fvec128( _mm_cvtepi32_ps( v ) ); }
private:
	__m128 _vec;
};

inline std::ostream &operator<<( std::ostream &os, fvec128 v )
{
	os << "{ " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << " }";
	return os;
}

// see all the operators defined in sse_fvec_operators.h

/// @brief declare a specialization of vector_limits for fvec128
template <>
struct vector_limits< fvec128 > : public std::numeric_limits<float>
{
	using ftype = float;
	static_assert( radix == 2, "expect a power 2 radix" );
	static_assert( sizeof(ftype) == 4, "expect a 32-bit float" );
	static_assert( std::is_floating_point<ftype>::value, "expecting float type" );
	typedef ftype value_type;

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

using fvec4 = fvec128;

} // namespace pal

#endif // _PAL_X86_FVEC128_T_H_
