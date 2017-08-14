//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/ivec128_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_IVEC128_T_H_
# define _PAL_X86_IVEC128_T_H_ 1

// defined some type-traits like entities that allow ivec to
// share code by using a template system to allow different
// integer types
#include "detail/ivec128.h"

namespace PAL_NAMESPACE
{

/// @brief base integer templated class
///
/// This is the basic implementation of all the integer vector
/// types. It is only defined in SSE2 and above.  Following this
/// definition, the various integer types are enumerated as a set of
/// typedefs for ease in programming.
template <typename inttype>
class ivec128
{
	typedef detail::ivec128_traits<sizeof(inttype)> manip_traits;
	typedef typename manip_traits::itype intrin_type;
public:
	static_assert( std::is_integral<inttype>::value, "Expecting integer type" );
	static_assert( sizeof(inttype) <= 8, "Value must be 64-bits or smaller for ivec128" );
	
	typedef inttype value_type;
	typedef inttype bitmask_type;
	typedef mask128<value_type> mask_type;

	static const int value_count = manip_traits::value_count;

	/// @defgroup declare default construction / copy semantics
	/// @{
	ivec128( void ) = default;
	~ivec128( void ) = default;
	ivec128( const ivec128 & ) = default;
	ivec128( ivec128 && ) = default;
	ivec128 &operator=( const ivec128 & ) = default;
	ivec128 &operator=( ivec128 && ) = default;
	/// @}

	explicit PAL_INLINE ivec128( value_type v ) : _vec( manip_traits::splat( static_cast<intrin_type>( v ) ) ) {}

	template <typename... Args>
	PAL_INLINE ivec128( value_type a0, value_type a1, Args &&... vals )
		: _vec( manip_traits::init( static_cast<intrin_type>( a0 ),
									static_cast<intrin_type>( a1 ),
									static_cast<intrin_type>( vals )... ) )
	{
		static_assert( (sizeof...(Args) + 2) == static_cast<size_t>(value_count), "unexpected argument count to constructor of ivec128" );
	}

	PAL_INLINE ivec128( __m128i x ) : _vec( x ) {}

	template <typename U, size_t N>
	explicit PAL_INLINE ivec128( const U(&a)[N] ) : _vec( manip_traits::init( a ) )
	{
		static_assert(N == static_cast<size_t>( value_count ), "expect array of matching size to initialize ivec128" );
	}

	template <size_t N>
	explicit PAL_INLINE ivec128( const std::array<value_type, N> &a ) : _vec( manip_traits::init( a ) ) {}
														  
	template <typename I>
	PAL_INLINE ivec128 &operator=( I x )
	{
		static_assert( std::is_integral<I>::value, "Expecting integer type in assignment" );
		_vec = manip_traits::splat( static_cast<intrin_type>( x ) );
		return *this;
	}
	PAL_INLINE ivec128 &operator=( __m128i x ) { _vec = x; return *this; }

	/// @brief provide transparent access in intrinsic calls
	PAL_INLINE operator __m128i( void ) const { return _vec; }

	/// @brief ability to reinterpret this as a float 128 vec
	PAL_INLINE __m128 as_float( void ) const { return _mm_castsi128_ps( _vec ); }
	PAL_INLINE __m128d as_double( void ) const { return _mm_castsi128_pd( _vec ); }

	PAL_INLINE value_type operator[]( int i ) const
	{
		return manip_traits::access( _vec, i );
	}

	PAL_INLINE ivec128 &operator+=( ivec128 a )
	{
		if ( std::is_signed<value_type>::value )
			_vec = manip_traits::adds( _vec, a );
		else
			_vec = manip_traits::addu( _vec, a );
		return *this;
	}
	PAL_INLINE ivec128 &operator+=( value_type a )
	{
		if ( std::is_signed<value_type>::value )
			_vec = manip_traits::adds( _vec, ivec128( a ) );
		else
			_vec = manip_traits::addu( _vec, ivec128( a ) );
		return *this;
	}
	PAL_INLINE ivec128 &operator-=( ivec128 a )
	{
		if ( std::is_signed<value_type>::value )
			_vec = manip_traits::subs( _vec, a );
		else
			_vec = manip_traits::subu( _vec, a );
		return *this;
	}
	PAL_INLINE ivec128 &operator-=( value_type a )
	{
		if ( std::is_signed<value_type>::value )
			_vec = manip_traits::subs( _vec, ivec128( a ) );
		else
			_vec = manip_traits::subu( _vec, ivec128( a ) );
		return *this;
	}

	PAL_INLINE ivec128 &operator*=( ivec128 a )
	{
		_vec = manip_traits::mul( _vec, a );
		return *this;
	}
	PAL_INLINE ivec128 &operator*=( value_type a )
	{
		_vec = manip_traits::mul( _vec, ivec128( a ) );
		return *this;
	}

	/// @brief factory to create a zero-initialized value
	static PAL_INLINE ivec128 zero( void ) { return ivec128( _mm_setzero_si128() ); }
	/// @brief factory to splat an integer
	///
	/// Provided as a convenience, same as constructing with a single
	/// value.
	static PAL_INLINE ivec128 splat( value_type a ) { return ivec128( manip_traits::splat( static_cast<intrin_type>( a ) ) ); }

private:
	__m128i _vec;
};

template <typename T>
std::ostream &operator<<( std::ostream &os, ivec128<T> v )
{
	os << "{ ";
	for ( int i = 0; i < ivec128<T>::value_count; ++i )
	{
		if ( i > 0 )
			os << ", ";
		os << v[i];
	}
	os << " }";
	return os;
}

typedef ivec128<int8_t> cvec16;
typedef ivec128<uint8_t> ucvec16;
typedef ivec128<int16_t> svec8;
typedef ivec128<uint16_t> usvec8;
typedef ivec128<int32_t> lvec4;
typedef ivec128<uint32_t> ulvec4;
typedef ivec128<int64_t> llvec2;
typedef ivec128<uint64_t> ullvec2;

/// @brief declare a specialization of vector_limits for ivec128
template <typename itype> struct vector_limits< ivec128<itype> > : public std::numeric_limits<itype>
{
	typedef itype value_type;

	static const int bits = 128;
	static const int bytes = 16;
	static const int value_count = detail::ivec128_traits<sizeof(value_type)>::value_count;
	static const int value_bits = 8*sizeof(value_type);

	static const value_type sign_mask = value_type( uint64_t(1) << (value_bits - 1) );
};

}

#endif // _PAL_X86_IVEC128_T_H_
