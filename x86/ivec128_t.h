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
/// typedefs/using for ease in programming.
template <typename inttype>
class ivec128
{
	using manip_traits = detail::ivec128_traits<sizeof(inttype)>;
	using intrin_type = typename manip_traits::itype;
public:
	static_assert( std::is_integral<inttype>::value, "Expecting integer type" );
	static_assert( sizeof(inttype) <= 8, "Value must be 64-bits or smaller for ivec128" );

	using value_type = inttype;
	using bitmask_type = inttype;
	using mask_type = mask128<value_type>;

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

	template <typename O>
	PAL_INLINE ivec128( const ivec128<O> &o ) : _vec( static_cast<__m128i>( o ) ) {}

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

	PAL_INLINE void set( value_type v ) { _vec = splat( v ); }
	PAL_INLINE void set( __m128i v ) { _vec = v; }
	PAL_INLINE void set( ivec128 v ) { _vec = v._vec; }

	template <int i>
	void set( value_type v )
	{
		_vec = manip_traits::insert<i>( _vec, v );
	}

	template <int i>
	value_type get( value_type v )
	{
		_vec = manip_traits::access( _vec, i );
	}

	class setter
	{
	public:
		~setter() = default;
		setter( const setter & ) = delete;
		setter &operator=( const setter & ) = delete;
		setter( setter && ) = default;
		setter &operator=( setter && ) = default;
		PAL_INLINE setter &operator=( value_type v )
		{
			set( v );
			return *this;
		}
		PAL_INLINE operator value_type () const { return get(); }
		PAL_INLINE setter &operator+=( value_type v ) { set( get() + v ); return *this; }
		PAL_INLINE setter &operator-=( value_type v ) { set( get() - v ); return *this; }
		PAL_INLINE setter &operator*=( value_type v ) { set( get() * v ); return *this; }
		PAL_INLINE setter &operator/=( value_type v ) { set( get() / v ); return *this; }
	private:

		PAL_INLINE value_type get( void ) const { return (*_vec)[_i]; }
		PAL_INLINE void set( value_type v )
		{
			if ( _i == 0 )
				(*_vec).set<0>( v );
			else if ( _i == 1 )
				(*_vec).set<1>( v );
			else if ( _i == 2 )
				(*_vec).set<2>( v );
			else if ( _i == 3 )
				(*_vec).set<3>( v );
		}
		PAL_INLINE setter( ivec128 *v, int i ) : _vec( v ), _i( i ) {}
		ivec128 *_vec;
		int _i;
		friend class ivec128;
	};

	PAL_INLINE setter at( int i ) { return setter( this, i ); }

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

using cvec16 = ivec128<int8_t>;
using ucvec16 = ivec128<uint8_t>;
using svec8 = ivec128<int16_t>;
using usvec8 = ivec128<uint16_t>;
using lvec4 = ivec128<int32_t>;
using ulvec4 = ivec128<uint32_t>;
using llvec2 = ivec128<int64_t>;
using ullvec2 = ivec128<uint64_t>;

/// @brief declare a specialization of vector_limits for ivec128
template <typename itype> struct vector_limits< ivec128<itype> > : public std::numeric_limits<itype>
{
	using value_type = itype;

	static const int bits = 128;
	static const int bytes = 16;
	static const int value_count = detail::ivec128_traits<sizeof(value_type)>::value_count;
	static const int value_bits = 8*sizeof(value_type);

	static const value_type sign_mask = value_type( uint64_t(1) << (value_bits - 1) );
};

}

#endif // _PAL_X86_IVEC128_T_H_
