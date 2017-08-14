//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/ivec256_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_IVEC256_T_H_
# define _PAL_X86_IVEC256_T_H_ 1

#ifdef PAL_ENABLE_AVX

// defined some type-traits like entities that allow ivec to
// share code by using a template system to allow different
// integer types
#include "detail/ivec256.h"

namespace PAL_NAMESPACE
{

/// @brief base integer templated class
///
/// This is the basic implementation of all the integer vector
/// types. It is only defined in SSE2 and above.  Following this
/// definition, the various integer types are enumerated as a set of
/// typedefs for ease in programming.
template <typename inttype>
class ivec256
{
	typedef detail::ivec256_traits<sizeof(inttype)> manip_traits;
	typedef typename manip_traits::itype intrin_type;
public:
	static_assert( std::is_integral<inttype>::value, "Expecting integer type" );
	static_assert( sizeof(inttype) <= 8, "Value must be 64-bits or smaller for ivec256" );
	
	typedef inttype value_type;
	typedef inttype bitmask_type;
	typedef mask256<inttype> mask_type;

	static const int value_count = manip_traits::value_count;

	/// @defgroup declare default construction / copy semantics
	/// @{
	ivec256( void ) = default;
	~ivec256( void ) = default;
	ivec256( const ivec256 & ) = default;
	ivec256( ivec256 && ) = default;
	ivec256 &operator=( const ivec256 & ) = default;
	ivec256 &operator=( ivec256 && ) = default;
	/// @}

	explicit PAL_INLINE ivec256( value_type v ) : _vec( manip_traits::splat( static_cast<intrin_type>( v ) ) ) {}

	template <typename... Args>
	PAL_INLINE ivec256( value_type a0, value_type a1, Args &&... vals )
		: _vec( manip_traits::init( static_cast<intrin_type>( a0 ),
									static_cast<intrin_type>( a1 ),
									static_cast<intrin_type>( vals )... ) )
	{
		static_assert( (sizeof...(Args) + 2) == static_cast<size_t>(value_count), "unexpected argument count to constructor of ivec256" );
	}

	PAL_INLINE ivec256( __m256i x ) : _vec( x ) {}
	template <typename I>
	PAL_INLINE ivec256 &operator=( I x )
	{
		static_assert( std::is_integral<I>::value, "Expecting integer type in assignment" );
		_vec = manip_traits::splat( static_cast<intrin_type>( x ) );
		return *this;
	}
	PAL_INLINE ivec256 &operator=( __m256i x ) { _vec = x; return *this; }

	/// @brief provide transparent access in intrinsic calls
	PAL_INLINE operator __m256i( void ) const { return _vec; }

	/// @brief ability to reinterpret this as a float 256 vec
	PAL_INLINE __m256i as_int( void ) const { return _vec; }
	PAL_INLINE __m256 as_float( void ) const { return _mm256_castsi256_ps( _vec ); }
	PAL_INLINE __m256d as_double( void ) const { return _mm256_castsi256_pd( _vec ); }

	PAL_INLINE float operator[]( int i ) const
	{
		return manip_traits::access( _vec, i );
	}

	PAL_INLINE ivec256 &operator+=( ivec256 a )
	{
		if ( std::is_signed<value_type>::value )
			_vec = manip_traits::adds( _vec, a );
		else
			_vec = manip_traits::addu( _vec, a );
		return *this;
	}
	PAL_INLINE ivec256 &operator-=( ivec256 a )
	{
		if ( std::is_signed<value_type>::value )
			_vec = manip_traits::subs( _vec, a );
		else
			_vec = manip_traits::subu( _vec, a );
		return *this;
	}

	/// @brief factory to create a zero-initialized value
	static PAL_INLINE ivec256 zero( void ) { return ivec256( _mm256_setzero_si256() ); }
	/// @brief factory to splat an integer
	///
	/// Provided as a convenience, same as constructing with a single
	/// value.
	static PAL_INLINE ivec256 splat( value_type a ) { return ivec256( manip_traits::splat( static_cast<intrin_type>( a ) ) ); }

private:
	__m256i _vec;
};

typedef ivec256<int8_t> cvec32;
typedef ivec256<uint8_t> ucvec32;
typedef ivec256<int16_t> svec16;
typedef ivec256<uint16_t> usvec16;
typedef ivec256<int32_t> lvec8;
typedef ivec256<uint32_t> ulvec8;
typedef ivec256<int64_t> llvec4;
typedef ivec256<uint64_t> ullvec4;

/// @brief declare a specialization of vector_limits for ivec256
template <typename itype> struct vector_limits< ivec256<itype> > : public std::numeric_limits<itype>
{
	typedef itype value_type;

	static const int bits = 256;
	static const int bytes = 32;
	static const int value_count = detail::ivec256_traits<sizeof(itype)>::value_count;
	static const int value_bits = 8*sizeof(value_type);

	static const value_type sign_mask = value_type( uint64_t(1) << (value_bits - 1) );
};

}

# endif // PAL_ENABLE_AVX

#endif // _PAL_X86_IVEC256_T_H_
