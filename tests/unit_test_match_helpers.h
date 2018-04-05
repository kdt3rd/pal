//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#ifndef _PAL_UNIT_TEST_MATCH_HELPERS_H_
#define _PAL_UNIT_TEST_MATCH_HELPERS_H_ 1

////////////////////////////////////////

#include "unit_test.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>
#include <limits>
#include <cstdlib>

////////////////////////////////////////

template <typename T>
float from_int( T x )
{
	static_assert( sizeof(T) <= sizeof(float), "missing from_int overload for 64-bit" );
	union
	{
		T i;
		float f;
	} v;
	v.i = x;
	return v.f;
}

template <typename T>
T as_int( T x )
{
	return x;
}

template <typename T>
T as_sint( T x )
{
	return x;
}


////////////////////////////////////////

float from_int( uint32_t x )
{
	union
	{
		uint32_t i;
		float f;
	} v;
	v.i = x;
	return v.f;
}

uint32_t as_int( float x )
{
	union
	{
		uint32_t i;
		float f;
	} v;
	v.f = x;
	return v.i;
}

int32_t as_sint( float x )
{
	union
	{
		int32_t i;
		float f;
	} v;
	v.f = x;
	return v.i;
}

////////////////////////////////////////

double from_int( uint64_t x )
{
	union
	{
		uint64_t i;
		double f;
	} v;
	v.i = x;
	return v.f;
}

uint64_t as_int( double x )
{
	union
	{
		uint64_t i;
		double f;
	} v;
	v.f = x;
	return v.i;
}

int64_t as_sint( double x )
{
	union
	{
		int64_t i;
		double f;
	} v;
	v.f = x;
	return v.i;
}

template <typename T, size_t N>
std::ostream &operator<<( std::ostream &os, const std::array<T, N> &v )
{
	os << "{ ";
	os << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
	for ( size_t i = 0; i != N; ++i )
	{
		if ( i > 0 )
			os << ", ";
		os << v[i];
	}
	os << std::setprecision(6);
	os << " }";
	return os;
}

////////////////////////////////////////

template <typename V>
struct match_test
{
	typedef V vec;
	typedef typename V::value_type value_type;
	static constexpr int value_count = V::value_count;
	typedef std::array<value_type, value_count> scalar;

	match_test( vec a, scalar b ) : _v( a ), _s( b ) {}
	template <size_t N>
	match_test( vec a, const value_type (&b)[N] ) : _v( a )
	{
		static_assert( N == value_count, "expect float set for array to match vector size" );
		std::copy( std::begin( b ), std::end( b ), _s.begin() );
	}

	const scalar &expect( void ) const { return _s; }
	const vec &calc( void ) const { return _v; }

	bool exact_equal( void ) const
	{
		// compare the bits to avoid NaN comparison returning false
		bool ret = true;
		for ( int i = 0; i != value_count; ++i )
			ret = ret && ( as_int( _v[i] ) == as_int( _s[i] ) );
		return ret;
	}

	bool delta_equal( value_type eps = std::numeric_limits<value_type>::epsilon() ) const
	{
		bool ret = true;
		for ( int i = 0; i != value_count; ++i )
			ret = ret && ( std::abs( _v[i] - _s[i] ) <= eps );
		return ret;
	}

	scalar delta( void ) const
	{
		scalar ret;
		for ( int i = 0; i != value_count; ++i )
			ret[i] = _v[i] - _s[i];
		return ret;
	}

	bool relative_equal( value_type percentage = 0.1F,
						 value_type eps = std::numeric_limits<value_type>::epsilon() ) const
	{
		bool ret = true;
		for ( int i = 0; i != value_count; ++i )
			ret = ret && compareRel( _v[i], _s[i], percentage / value_type(100), eps );
		return ret;
	}

	scalar relative_error( void ) const
	{
		scalar ret;
		for ( int i = 0; i != value_count; ++i )
			ret[i] = relError( _v[i], _s[i] );
		return ret;
	}


	bool ulp_equal( int ulps = 1 ) const
	{
		bool ret = true;
		for ( int i = 0; i != value_count; ++i )
			ret = ret && ( ulpsDelta( _v[i], _s[i] ) <= uint32_t(ulps) );
		return ret;
	}

	std::array<int, value_count> ulp_error( void ) const
	{
		std::array<int, value_count> ret;
		for ( int i = 0; i != value_count; ++i )
			ret[i] = ulpsDelta( _v[i], _s[i] );
		return ret;
	}

	static uint32_t ulpsDelta( value_type a, value_type b )
	{
		int aI = as_sint( a );
		int bI = as_sint( b );
		// if we just return return aI - bI there can be issues
		// with zero crossing because of the 2s complement sign

		if ( aI < 0 )
			aI = -( 0x7FFFFFFF & aI );

		if ( bI < 0 )
			bI = -( 0x7FFFFFFF & bI );

		return static_cast<uint32_t>( std::abs( aI - bI ) );
	}

	static value_type relError( value_type a, value_type b )
	{
		if ( std::abs( b ) > std::abs( a ) )
		{
			if ( b == 0 )
				return std::abs( a - b );
			return 100.F * std::abs( ( a - b ) / b );
		}
		else
		{
			if ( a == 0 )
				return std::abs( a - b );
			return 100.F * std::abs( ( a - b ) / a );
		}
	}

	static bool compareRel( value_type a, value_type b, value_type relErr, value_type absErr )
	{
	    if ( std::abs( a - b ) < absErr )
			return true;

		value_type err;
		if ( std::abs( b ) > std::abs( a ) )
			err = std::abs( ( a - b ) / b );
		else
			err = std::abs( ( a - b ) / a );
		if ( err < relErr )
			return true;
	}

private:
	vec _v;
	scalar _s;
};

////////////////////////////////////////

template <typename T, typename U>
inline void
TEST_VAL_EQ( unit_test &test, const std::string &tag, T val, U exp )
{
	std::stringstream msg_buf;
	if ( val == exp )
	{
		msg_buf << val;
		test.success( tag, msg_buf.str() );
	}
	else
	{
		msg_buf << "expected " << exp << ", got " << val;
		test.failure( tag, msg_buf.str() );
	}
}

template <typename T, typename U>
inline void
TEST_VAL_EQ_HEX( unit_test &test, const std::string &tag, T val, U exp )
{
	std::stringstream msg_buf;
	if ( val == T(exp) )
	{
		msg_buf << "0x"
				<< std::setw(sizeof(T)*2)
				<< std::setfill( '0' )
				<< std::hex
				<< val;
		test.success( tag, msg_buf.str() );
	}
	else
	{
		msg_buf << "expected 0x"
				<< std::setw(sizeof(T)*2)
				<< std::setfill( '0' )
				<< std::hex
				<< exp
				<< ", found 0x"
				<< std::setw(sizeof(T)*2)
				<< std::setfill( '0' )
				<< std::hex
				<< val;
		test.failure( tag, msg_buf.str() );
	}
}

// expects code to be a lambda or other operator/functor
// that returns a match object
template <typename F>
static void
TEST_CODE_VAL_EQ( unit_test &test, const std::string &tag, F code )
{
	auto t = code();

	if ( t.exact_equal() )
	{
		std::stringstream msg_buf;
		msg_buf << "<exact equal>";
		test.success( tag, msg_buf.str() );
	}
	else
	{
		std::stringstream msg_buf;
		msg_buf << "expect "
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< t.expect() << ", got "
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< t.calc()
				<< ", delta "
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< t.delta();
		test.failure( tag, msg_buf.str() );
	}
}

template <typename F>
static void
TEST_CODE_VAL_EQ_PREC( unit_test &test, const std::string &tag,
					   F code,
					   float eps = std::numeric_limits<float>::epsilon() )
{
	auto t = code();

	if ( t.delta_equal( eps ) )
	{
		std::stringstream msg_buf;
		msg_buf << "matches +/- abs delta of " << eps;
		test.success( tag, msg_buf.str() );
	}
	else
	{
		std::stringstream msg_buf;
		msg_buf << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< "expect " << t.expect()
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< ", got " << t.calc()
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< " delta " << t.delta()
				<< std::setprecision( 6 )
				<< ", tol +/- " << eps;
		test.failure( tag, msg_buf.str() );
	}
}

template <typename F>
static void
TEST_CODE_VAL_EQ_REL( unit_test &test, const std::string &tag,
					  F code,
					  float relativeErrPercentage )
{
	auto t = code();

	if ( t.relative_equal( relativeErrPercentage ) )
	{
		std::stringstream msg_buf;
		msg_buf << "matches +/- " << relativeErrPercentage << "%";
		test.success( tag, msg_buf.str() );
	}
	else
	{
		std::stringstream msg_buf;
		msg_buf << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< "expect " << t.expect()
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< ", got " << t.calc()
				<< std::setprecision( 6 )
				<< ", error " << t.relative_error()
				<< ", tol +/- " << relativeErrPercentage << "%";
		test.failure( tag, msg_buf.str() );
	}
}

template <typename F>
static void
TEST_CODE_VAL_EQ_ULPS( unit_test &test, const std::string &tag,
					   F code,
					   int ulps )
{
	auto t = code();

	if ( t.ulp_equal( ulps ) )
	{
		std::stringstream msg_buf;
		msg_buf << "matches +/- " << ulps << " ULP (err " << t.ulp_error() << ')';
		test.success( tag, msg_buf.str() );
	}
	else
	{
		std::stringstream msg_buf;
		msg_buf << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< "expect " << t.expect()
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< ", got " << t.calc()
				<< std::setprecision(std::numeric_limits<long double>::digits10 + 1)
				<< ", ulps " << t.ulp_error()
				<< std::setprecision( 6 )
				<< ", tol +/- " << ulps << " ULP";
		test.failure( tag, msg_buf.str() );
	}
}

template <typename F>
static void
TEST_CODE_LONG_RUN( unit_test &test, const std::string &tag,
					F code )
{
	test.start_run( tag );
	code();
}

#endif
