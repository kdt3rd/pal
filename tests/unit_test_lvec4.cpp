//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "unit_test.h"
#include "unit_test_match_helpers.h"
#include <pal.h>
#include <cfloat>
#include <cmath>

typedef match_test<PAL_NAMESPACE::lvec4> match;

static void
add_class_tests( unit_test &test )
{
	test["limits_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		typedef vector_limits<fvec4> limits;
		TEST_VAL_EQ(test, "value_count", limits::value_count, 4 );
		TEST_VAL_EQ(test, "bytes", limits::bytes, 16 );
		TEST_VAL_EQ(test, "bits", limits::bits, 128 );
		TEST_VAL_EQ(test, "value_bits", limits::value_bits, 32 );
		TEST_VAL_EQ_HEX(test, "sign_mask", limits::sign_mask, 0x80000000 );
	};

	test["class_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "zero",
						 []() { return match( lvec4::zero(), {0,0,0,0} ); } );
		TEST_CODE_VAL_EQ(test, "splat",
						 []() { return match( lvec4::splat( 42 ), {42,42,42,42} ); } );
		TEST_CODE_VAL_EQ(test, "ctor1",
						 []() { return match( lvec4( 42 ), {42,42,42,42} ); } );
		TEST_CODE_VAL_EQ(test, "ctor4",
						 []() { return match( lvec4(1,2,3,4), {1,2,3,4} ); } );
		TEST_CODE_VAL_EQ(test, "ctor_static_array4",
						 []() {
							 int f[4] = {42,42,42,42};
							 return match( lvec4( f ), f ); } );
		TEST_CODE_VAL_EQ(test, "ctor_std_array4",
						 []() {
							 std::array<int, 4> f{42,42,42,42};
							 return match( lvec4( f ), f ); } );
		TEST_CODE_VAL_EQ(test, "assign1",
						 []() {
							 lvec4 tmp = lvec4::zero();
							 tmp = 4;
							 return match( tmp, {4,4,4,4} );
						 } );
		TEST_CODE_VAL_EQ(test, "add1_inplace",
						 []() {
							 lvec4 tmp( 1 );
							 tmp += 1;
							 return match( tmp, {2,2,2,2} );
						 } );
		TEST_CODE_VAL_EQ(test, "add4_inplace",
						 []() {
							 lvec4 tmp( 1 );
							 tmp += tmp;
							 return match( tmp, {2,2,2,2} );
						 } );
		TEST_CODE_VAL_EQ(test, "sub1_inplace",
						 []() {
							 lvec4 tmp( 5 );
							 tmp -= 3;
							 return match( tmp, {2,2,2,2} );
						 } );
		TEST_CODE_VAL_EQ(test, "sub4_inplace",
						 []() {
							 lvec4 tmp( 5 );
							 lvec4 tmp2( 3 );
							 tmp -= tmp2;
							 return match( tmp, {2,2,2,2} );
						 } );
		TEST_CODE_VAL_EQ(test, "mul1_inplace",
						 []() {
							 lvec4 tmp( 1, 2, 3, 4 );
							 tmp *= 4;
							 return match( tmp, {4,8,12,16} );
						 } );
		TEST_CODE_VAL_EQ(test, "mul4_inplace",
						 []() {
							 lvec4 tmp( 5, 10, 15, -20 );
							 lvec4 tmp2( 2 );
							 tmp *= tmp2;
							 return match( tmp, {10,20,30,-40} );
						 } );
	};
}

static void
add_op_tests( unit_test &test )
{
	test["arith_op_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "unary +",
						 []() {
							 int tval[4] = {5,-1,0,int(0x80000000)};
							 int cval[4];
							 lvec4 tmp( tval );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = +tval[i];

							 lvec4 r = +tmp;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "unary -",
						 []() {
							 int tval[4] = {5,-1,0,int(0xFFFFFFFF)};
							 int cval[4];
							 lvec4 tmp( tval );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = -tval[i];

							 lvec4 r = -tmp;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary +",
						 []() {
							 int tval[4] = {5,-1,0,18};
							 int tval2[4] = {3,1,-1,27};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = tval[i] + tval2[i] + 42;
							 lvec4 r = tmp + tmp2 + 42;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary -",
						 []() {
							 int tval[4] = {5,-1,0,18};
							 int tval2[4] = {3,1,-25,-1};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = (tval[i] - 12) - tval2[i];
							 lvec4 r = (tmp - 12) - (tmp2);
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary *",
						 []() {
							 int tval[4] = {5,-1,0,18};
							 int tval2[4] = {3,1,-25,-1};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = (tval[i] * 2) * tval2[i];
							 lvec4 r = (tmp * 2) * tmp2;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary / (non-const)",
						 []() {
							 int tval[4] = {127,126,-30,18};
							 int cval[4];
							 lvec4 tmp( tval );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = (tval[i] / 3);
							 int_divisor d( 3 );
							 return match( tmp / d, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary / (const)",
						 []() {
							 int tval[4] = {127,126,-30,18};
							 int cval[4];
							 lvec4 tmp( tval );
							 for ( int i = 0; i < 4; ++i )
								 cval[i] = (tval[i] / 3);
							 return match( divide_by_const<3>( tmp ), cval );
						 } );
	};
#if 0
	test["bitwise_op_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "operator&",
						 []() {
							 int tval[4] = {5,-1,0.5F,18};
							 uint32_t mask = 0x01234567;
							 int cval[4];
							 lvec4 tmp( tval );
							 cval[0] = from_int( as_int( tval[0] ) & mask );
							 cval[1] = from_int( as_int( tval[1] ) & mask );
							 cval[2] = from_int( as_int( tval[2] ) & mask );
							 cval[3] = from_int( as_int( tval[3] ) & mask );
							 return match( tmp & lvec4( mask ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator|",
						 []() {
							 int tval[4] = {5,-1,0.5F,18};
							 uint32_t mask = 0x01234567;
							 int cval[4];
							 lvec4 tmp( tval );
							 cval[0] = from_int( as_int( tval[0] ) | mask );
							 cval[1] = from_int( as_int( tval[1] ) | mask );
							 cval[2] = from_int( as_int( tval[2] ) | mask );
							 cval[3] = from_int( as_int( tval[3] ) | mask );
							 return match( tmp | lvec4( mask ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator^",
						 []() {
							 int tval[4] = {5,-1,0.5F,18};
							 uint32_t mask = 0x81234567;
							 int cval[4];
							 lvec4 tmp( tval );
							 cval[0] = from_int( as_int( tval[0] ) ^ mask );
							 cval[1] = from_int( as_int( tval[1] ) ^ mask );
							 cval[2] = from_int( as_int( tval[2] ) ^ mask );
							 cval[3] = from_int( as_int( tval[3] ) ^ mask );
							 return match( tmp ^ lvec4( mask ), cval );
						 } );
	};
	test["compare_op_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "operator==",
						 []() {
							 int tval[4] = {5,-1,0.5F,18/0};
							 int tval2[4] = {5.1F,-1,5.0F,18/0};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] == tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] == tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] == tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] == tval2[3] ? uint32_t(-1) : 0 );
							 return match( lvec4( (tmp == tmp2).as_int() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator!=",
						 []() {
							 int tval[4] = {5,-1,0.5F,18/0};
							 int tval2[4] = {5.1F,-1,5.0F,18/0};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] != tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] != tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] != tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] != tval2[3] ? uint32_t(-1) : 0 );
							 return match( lvec4( (tmp != tmp2).as_int() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator<",
						 []() {
							 int tval[4] = {5,-1,0.5F,18/0};
							 int tval2[4] = {5.1F,-1,5.0F,18/0};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] < tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] < tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] < tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] < tval2[3] ? uint32_t(-1) : 0 );
							 return match( lvec4( (tmp < tmp2).as_int() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator<=",
						 []() {
							 int tval[4] = {5,-1,0.5F,18/0};
							 int tval2[4] = {5.1F,-1,5.0F,18/0};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] <= tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] <= tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] <= tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] <= tval2[3] ? uint32_t(-1) : 0 );
							 return match( lvec4( (tmp <= tmp2).as_int() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator>",
						 []() {
							 int tval[4] = {5,-1,0.5F,18/0};
							 int tval2[4] = {5.1F,-1,5.0F,18/0};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] > tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] > tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] > tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] > tval2[3] ? uint32_t(-1) : 0 );
							 return match( lvec4( (tmp > tmp2).as_int() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator>=",
						 []() {
							 int tval[4] = {5,-1,0.5F,18/0};
							 int tval2[4] = {5.1F,-1,5.0F,18/0};
							 int cval[4];
							 lvec4 tmp( tval );
							 lvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] >= tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] >= tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] >= tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] >= tval2[3] ? uint32_t(-1) : 0 );
							 return match( lvec4( (tmp >= tmp2).as_int() ), cval );
						 } );
	};
#endif
}

static void
add_load_store_tests( unit_test &test )
{
}

int main( int argc, char *argv[] )
{
	unit_test test( "lvec4" );

	add_class_tests( test );
	add_op_tests( test );
	add_load_store_tests( test );

	bool q = false;
	while ( argc > 1 )
	{
		--argc;
		std::string arg = argv[argc];
		if ( arg == "-h" || arg == "--help" )
		{
			std::cout << argv[0] << " [-q|--quiet] [test names ...]" << std::endl;
			return 0;
		}
		else if ( arg == "-q" || arg == "--quiet" )
			q = true;
		else
			test.add_to_run( std::move( arg ) );
	}

	return test.run( q );
}
