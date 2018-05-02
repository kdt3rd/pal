//
// Copyright (c) 2016 Kimball Thurston
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#if !defined _PAL_H_
# error "Never use <pal/x86/mask128_t.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_MASK128_T_H_
# define _PAL_X86_MASK128_T_H_ 1

#include "detail/mask128.h"

namespace PAL_NAMESPACE
{

/// @brief Represents a bit mask of 128 bits
///
/// This is normally to be used with another 128-bit type, such as
/// fvec4 or ivec4, to mask and select between multiple values. As
/// such, it is different from both in the operations that are
/// available
template <typename mtype>
class mask128
{
public:
	static_assert( sizeof(mtype) <= 8, "Value must be 64-bits or smaller for mask128" );

	using value_type = mtype;
	using value_traits = detail::mask128_traits<sizeof(mtype), mtype>;
	using vec_type = typename value_traits::vec_type;
	using bitmask_type = typename value_traits::bitmask_type;
	using manip_traits = detail::mask128_bitops<value_type>;

	static const int value_count = value_traits::value_count;

	static PAL_INLINE mask128 no( void ) { return mask128( manip_traits::no() ); }
	static PAL_INLINE mask128 yes( void ) { return mask128( manip_traits::yes() ); }

	/// @brief splat a single boolean across all bits
	explicit PAL_INLINE mask128( bool b ) : _vec( b ? yes() : no() ) {}

	template <typename... Args>
	PAL_INLINE mask128( bool a0, bool a1, Args &&... vals )
		: _vec( value_traits::init( static_cast<bool>( a0 ),
									static_cast<bool>( a1 ),
									static_cast<bool>( vals )... ) )
	{
		static_assert( (sizeof...(Args) + 2) == static_cast<size_t>(value_count), "unexpected argument count to constructor of mask128" );
	}

	/// @brief construct with a (vector) float value.
	///
	/// User is expected to make sure it has the correct mask values (all bytes 0xFF as appropriate)
	explicit PAL_INLINE mask128( vec_type v ) : _vec( v ) {}

	template <typename U>
	PAL_INLINE mask128( mask128<U> v ) : _vec( manip_traits::from_other( v ) ) {}

	PAL_INLINE __m128 as_float( void ) const { return manip_traits::as_float( _vec ); }
	PAL_INLINE __m128d as_double( void ) const { return manip_traits::as_double( _vec ); }
	PAL_INLINE __m128i as_int( void ) const { return manip_traits::as_int( _vec ); }

	/// @brief operator to allow transparent usage in intrinsics
	///
	/// This does not allow arbitrary type exchanges since we make the
	/// constructor explicit.
	PAL_INLINE operator vec_type( void ) const { return _vec; }

	/// @defgroup declare default construction / copy semantics
	/// @{
	mask128( void ) = default;
	~mask128( void ) = default;
	mask128( const mask128 & ) = default;
	mask128( mask128 && ) = default;
	mask128 &operator=( const mask128 & ) = default;
	mask128 &operator=( mask128 && ) = default;
	/// @}

	/// @brief set all bits to zero
	PAL_INLINE void clear( void ) { _vec = no(); }

	/// @defgroup bit-wise self-modifying functions
	/// @{

	PAL_INLINE mask128 &operator&=( mask128 a )
	{
		_vec = manip_traits::apply_and( _vec, a );
		return *this;
	}
	PAL_INLINE mask128 &operator|=( mask128 a )
	{
		_vec = manip_traits::apply_or( _vec, a );
		return *this;
	}
	PAL_INLINE mask128 &operator^=( mask128 a )
	{
		_vec = manip_traits::apply_xor( _vec, a );
		return *this;
	}

	PAL_INLINE mask128 operator~( void ) const
	{
		return mask128( manip_traits::apply_xor( _vec, yes() ) );
	}
	/// @}

	/// @defgroup mixing functions
	/// @{

	PAL_INLINE vec_type in( vec_type v ) const
	{
		return manip_traits::apply_and( _vec, v );
	}

	PAL_INLINE vec_type notIn( vec_type v ) const
	{
		return manip_traits::apply_andnot( _vec, v );
	}

	/// @brief if first bit set, returns b, else returns a
	///
	/// NB: This is only guaranteed to check a single bit of the mask
	/// (although depending on the implementation, may check more). if
	/// you need true bit-wise mixing, it's better to use the @sa
	/// bit_mix function
	PAL_INLINE vec_type blend( vec_type a, vec_type b ) const
	{
		return manip_traits::blend( _vec, a, b );
	}

	/// @brief apply bit-wise mixing based on the bits in the mask.
	///
	/// if the bit is 1, return b, else return a (similar to @sa
	/// blend, but guaranteeing every bit is checked).
	PAL_INLINE vec_type bit_mix( vec_type a, vec_type b ) const
	{
		return manip_traits::bit_mix( _vec, a, b );
	}
	
	/// @}

	/// @defgroup test functions
	/// @{

	/// @brief returns a mask indicating what elements are set @sa active_mask
	PAL_INLINE int which( void ) const
	{
		return manip_traits::any( _vec );
	}

	/// @brief returns a mask for a specific element index
	PAL_INLINE int active_mask( const int i ) const
	{
		return manip_traits::active_mask( i );
	}

	/// @brief returns true if any bits are set
	PAL_INLINE bool any( void ) const
	{
		return manip_traits::any( _vec );
	}
	/// @brief returns true if all bits are set
	PAL_INLINE bool all( void ) const
	{
		return manip_traits::all( _vec );
	}
	/// @brief returns true if none of the bits are set
	PAL_INLINE bool none( void ) const
	{
		return manip_traits::none( _vec );
	}

	/// @brief retrieve a true/false for an item in the same manner as
	/// the various blend functions guarantee (they test the high bit)
	PAL_INLINE bool access_bool( int i ) const
	{
		return value_traits::access_bool( _vec, i );
	}

	/// @brief retrieve a mask value. should probably only be used for debugging
	PAL_INLINE bitmask_type access_value( int i ) const
	{
		return value_traits::access_value( _vec, i );
	}

	/// @brief static factory method
	///
	/// This is provided solely as a convenience for consistency or
	/// obviousness rather than a bare temporary constructed.
	static PAL_INLINE mask128 splat( bool b ) { return mask128( b ); }

	template <typename T>
	static PAL_INLINE mask128 splat_mask( T v )
	{ return mask128( value_traits::splat( static_cast<bitmask_type>( v ) ) ); }			

private:
	vec_type _vec;
};

template <typename T>
std::ostream &operator<<( std::ostream &os, mask128<T> v )
{
	typedef typename std::conditional<sizeof(T)<=4, uint32_t, uint64_t>::type cast_type;
	os << "{ ";
	for ( int i = 0; i < mask128<T>::value_count; ++i )
	{
		if ( i > 0 )
			os << ", ";
		os << (v.access_bool(i)?"true":"false") << " (0x"
		   << std::hex << std::setfill('0') << std::setw( sizeof(T) * 2 )
		   << static_cast<cast_type>( v.access_value( i ) )
		   << std::dec << ')';
	}
	os << " }";
	return os;
}

/// @defgroup 128-bit bit-wise operators
/// @{
template <typename T>
PAL_INLINE mask128<T> operator!( mask128<T> a )
{
	typedef typename mask128<T>::manip_traits mt;
	return mask128<T>( mt::apply_xor( a, mt::yes() ) );
}

template <typename T>
PAL_INLINE mask128<T> operator~( mask128<T> a )
{
	return ! a;
}

template <typename T>
PAL_INLINE mask128<T> operator&( mask128<T> a, mask128<T> b )
{
	typedef typename mask128<T>::manip_traits mt;
	return mask128<T>( mt::apply_and( a, b ) );
}

template <typename T>
PAL_INLINE mask128<T> operator&&( mask128<T> a, mask128<T> b )
{
	return a & b;
}

template <typename T>
PAL_INLINE mask128<T> operator|( mask128<T> a, mask128<T> b )
{
	typedef typename mask128<T>::manip_traits mt;
	return mask128<T>( mt::apply_or( a, b ) );
}

template <typename T>
PAL_INLINE mask128<T> operator||( mask128<T> a, mask128<T> b )
{
	return a | b;
}

template <typename T>
PAL_INLINE mask128<T> operator^( mask128<T> a, mask128<T> b )
{
	typedef typename mask128<T>::manip_traits mt;
	return mask128<T>( mt::apply_xor( a, b ) );
}
/// @}


////////////////////////////////////////


/// @defgroup 128-bit comparison operators
/// @{

template <typename T>
PAL_INLINE mask128<T> operator==( mask128<T> a, mask128<T> b )
{
	typedef typename mask128<T>::manip_traits mt;
	return mask128<T>( mt::apply_eq( a, b ) );
}

template <typename T>
PAL_INLINE mask128<T> operator!=( mask128<T> a, mask128<T> b )
{
	typedef typename mask128<T>::manip_traits mt;
	return mask128<T>( mt::apply_xor( a, b ) );
}

/// @}


/// @brief declare specialization of vector_limits for mask128
template <typename T> struct vector_limits< mask128<T> > : public std::numeric_limits<bool>
{
	static const int bits = 128;
	static const int bytes = 16;
};

}

#endif // _PAL_X86_MASK128_T_H_



