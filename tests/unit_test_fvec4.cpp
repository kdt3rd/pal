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

typedef match_test<PAL_NAMESPACE::fvec4> match;
typedef match_test<PAL_NAMESPACE::lvec4> intmatch;
static const int kFastULPprec = 16;

// these tests test all the various members of the fvec4 class
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
		TEST_VAL_EQ(test, "mantissa_bits", limits::mantissa_bits, 23 );
		TEST_VAL_EQ(test, "sign_bits", limits::sign_bits, 1 );
		TEST_VAL_EQ(test, "exponent_bits", limits::exponent_bits, 8 );
		TEST_VAL_EQ(test, "exponent_bias", limits::exponent_bias, 127 );
		TEST_VAL_EQ_HEX(test, "mantissa_mask", limits::mantissa_mask, 0x007FFFFF );
		TEST_VAL_EQ_HEX(test, "exponent_mask", limits::exponent_mask, 0x7F800000 );
		TEST_VAL_EQ_HEX(test, "sign_mask", limits::sign_mask, 0x80000000 );
	};

	test["class_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "zero",
						 []() { return match( fvec4::zero(), {0.F,0.F,0.F,0.F} ); } );
		TEST_CODE_VAL_EQ(test, "splat",
						 []() { return match( fvec4::splat( 42.F ), {42.F,42.F,42.F,42.F} ); } );
		TEST_CODE_VAL_EQ(test, "ctor1",
						 []() { return match( fvec4( 42.F ), {42.F,42.F,42.F,42.F} ); } );
		TEST_CODE_VAL_EQ(test, "ctor4",
						 []() { return match( fvec4(1.F,2.F,3.F,4.F), {1.F,2.F,3.F,4.F} ); } );
		TEST_CODE_VAL_EQ(test, "ctor_static_array4",
						 []() {
							 float f[4] = {42.F,42.F,42.F,42.F};
							 return match( fvec4( f ), f ); } );
		TEST_CODE_VAL_EQ(test, "ctor_std_array4",
						 []() {
							 std::array<float, 4> f{42.F,42.F,42.F,42.F};
							 return match( fvec4( f ), f ); } );
		TEST_CODE_VAL_EQ(test, "ctor_bitmask",
						 []() {
							 float f = from_int( 0x80000000 );
							 fvec4 tmp( uint32_t( 0x80000000 ) );

							 return match( tmp, {f,f,f,f} );
						 } );
		TEST_CODE_VAL_EQ(test, "assign1",
						 []() {
							 fvec4 tmp = fvec4::zero();
							 tmp = 4.F;
							 return match( tmp, {4.F,4.F,4.F,4.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "assign_idx",
						 []() {
							 fvec4 tmp = fvec4::zero();
							 tmp = 4.F;
							 tmp.set<2>( 2.F );
							 return match( tmp, {4.F,4.F,2.F,4.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "add1_inplace",
						 []() {
							 fvec4 tmp( 1.F );
							 tmp += 1.F;
							 return match( tmp, {2.F,2.F,2.F,2.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "add4_inplace",
						 []() {
							 fvec4 tmp( 1.F );
							 tmp += tmp;
							 return match( tmp, {2.F,2.F,2.F,2.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "sub1_inplace",
						 []() {
							 fvec4 tmp( 5.F );
							 tmp -= 3.F;
							 return match( tmp, {2.F,2.F,2.F,2.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "sub4_inplace",
						 []() {
							 fvec4 tmp( 5.F );
							 fvec4 tmp2( 3.F );
							 tmp -= tmp2;
							 return match( tmp, {2.F,2.F,2.F,2.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "mul1_inplace",
						 []() {
							 fvec4 tmp( 4.F );
							 tmp *= 4.F;
							 return match( tmp, {16.F,16.F,16.F,16.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "mul4_inplace",
						 []() {
							 fvec4 tmp( 5.F, 10.F, 15.F, 20.F );
							 fvec4 tmp2( 2.F );
							 tmp *= tmp2;
							 return match( tmp, {10.F,20.F,30.F,40.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "div1_inplace",
						 []() {
							 fvec4 tmp( 16.F );
							 tmp /= 4.F;
							 return match( tmp, {4.F,4.F,4.F,4.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "div4_inplace",
						 []() {
							 fvec4 tmp( 10.F, 20.F, 30.F, 40.F );
							 fvec4 tmp2( 2.F );
							 tmp /= tmp2;
							 return match( tmp, {5.F,10.F,15.F,20.F} );
						 } );
		TEST_CODE_VAL_EQ(test, "convert",
						 []() {
							 return match(
								 fvec4::convert_int(fvec4::int_vec_type(1,2,3,4)), {1.F,2.F,3.F,4.F} ); } );
	};
}

// test the external operators
static void
add_op_tests( unit_test &test )
{
	test["arith_op_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "unary +",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,-std::numeric_limits<float>::infinity()};
							 float cval[4];
							 fvec4 tmp( tval );
							 cval[0] = +tval[0];
							 cval[1] = +tval[1];
							 cval[2] = +tval[2];
							 cval[3] = +tval[3];
							 fvec4 r = +tmp;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "unary -",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,-std::numeric_limits<float>::infinity()};
							 float cval[4];
							 fvec4 tmp( tval );
							 cval[0] = -tval[0];
							 cval[1] = -tval[1];
							 cval[2] = -tval[2];
							 cval[3] = -tval[3];
							 fvec4 r = -tmp;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary +",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {0.3F,1.1F,-0.25F,-0.001F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = 0.2F + tval[0] + 0.2F + tval2[0];
							 cval[1] = 0.2F + tval[1] + 0.2F + tval2[1];
							 cval[2] = 0.2F + tval[2] + 0.2F + tval2[2];
							 cval[3] = 0.2F + tval[3] + 0.2F + tval2[3];
							 fvec4 r = 0.2F + tmp + 0.2F + tmp2;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary -",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {0.3F,1.1F,-0.25F,-0.001F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = 0.2F - tval[0] - 0.2F - tval2[0];
							 cval[1] = 0.2F - tval[1] - 0.2F - tval2[1];
							 cval[2] = 0.2F - tval[2] - 0.2F - tval2[2];
							 cval[3] = 0.2F - tval[3] - 0.2F - tval2[3];
							 fvec4 r = 0.2F - tmp - 0.2F - tmp2;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary *",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {0.3F,1.1F,-0.25F,-0.001F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = 0.2F * tval[0] * 0.2F * tval2[0];
							 cval[1] = 0.2F * tval[1] * 0.2F * tval2[1];
							 cval[2] = 0.2F * tval[2] * 0.2F * tval2[2];
							 cval[3] = 0.2F * tval[3] * 0.2F * tval2[3];
							 fvec4 r = 0.2F * tmp * 0.2F * tmp2;
							 return match( r, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "binary /",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {0.3F,1.1F,-0.25F,-0.001F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = 0.2F / tval[0] / 0.2F / tval2[0];
							 cval[1] = 0.2F / tval[1] / 0.2F / tval2[1];
							 cval[2] = 0.2F / tval[2] / 0.2F / tval2[2];
							 cval[3] = 0.2F / tval[3] / 0.2F / tval2[3];
							 fvec4 r = 0.2F / tmp / 0.2F / tmp2;
							 return match( r, cval );
						 } );
	};
	test["bitwise_op_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "operator&",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 uint32_t mask = 0x01234567;
							 float cval[4];
							 fvec4 tmp( tval );
							 cval[0] = from_int( as_int( tval[0] ) & mask );
							 cval[1] = from_int( as_int( tval[1] ) & mask );
							 cval[2] = from_int( as_int( tval[2] ) & mask );
							 cval[3] = from_int( as_int( tval[3] ) & mask );
							 return match( tmp & fvec4( mask ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator|",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 uint32_t mask = 0x01234567;
							 float cval[4];
							 fvec4 tmp( tval );
							 cval[0] = from_int( as_int( tval[0] ) | mask );
							 cval[1] = from_int( as_int( tval[1] ) | mask );
							 cval[2] = from_int( as_int( tval[2] ) | mask );
							 cval[3] = from_int( as_int( tval[3] ) | mask );
							 return match( tmp | fvec4( mask ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator^",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 uint32_t mask = 0x81234567;
							 float cval[4];
							 fvec4 tmp( tval );
							 cval[0] = from_int( as_int( tval[0] ) ^ mask );
							 cval[1] = from_int( as_int( tval[1] ) ^ mask );
							 cval[2] = from_int( as_int( tval[2] ) ^ mask );
							 cval[3] = from_int( as_int( tval[3] ) ^ mask );
							 return match( tmp ^ fvec4( mask ), cval );
						 } );
	};
	test["compare_op_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "operator==",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] == tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] == tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] == tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] == tval2[3] ? uint32_t(-1) : 0 );
							 return match( fvec4( (tmp == tmp2).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator!=",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] != tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] != tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] != tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] != tval2[3] ? uint32_t(-1) : 0 );
							 return match( fvec4( (tmp != tmp2).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator<",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] < tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] < tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] < tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] < tval2[3] ? uint32_t(-1) : 0 );
							 return match( fvec4( (tmp < tmp2).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator<=",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] <= tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] <= tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] <= tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] <= tval2[3] ? uint32_t(-1) : 0 );
							 return match( fvec4( (tmp <= tmp2).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator>",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] > tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] > tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] > tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] > tval2[3] ? uint32_t(-1) : 0 );
							 return match( fvec4( (tmp > tmp2).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "operator>=",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 cval[0] = from_int( tval[0] >= tval2[0] ? uint32_t(-1) : 0 );
							 cval[1] = from_int( tval[1] >= tval2[1] ? uint32_t(-1) : 0 );
							 cval[2] = from_int( tval[2] >= tval2[2] ? uint32_t(-1) : 0 );
							 cval[3] = from_int( tval[3] >= tval2[3] ? uint32_t(-1) : 0 );
							 return match( fvec4( (tmp >= tmp2).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "which",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,0.F};
							 float tval2[4] = {5.1F,-1.F,5.0F,0.F};
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 fvec4::mask_type m = ( tmp < tmp2 );

							 return match_val<int>( m.which(), m.active_mask( 0 ) );
						 } );
		TEST_CODE_VAL_EQ(test, "any",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 fvec4 tmp( tval );
							 auto m = isinf( tmp );
							 return match_val<bool>( m.any(), true );
						 } );
		TEST_CODE_VAL_EQ(test, "all",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F/0.F};
							 fvec4 tmp( tval );
							 auto m = isinf( tmp );
							 return match_val<bool>( m.all(), false );
						 } );
		TEST_CODE_VAL_EQ(test, "none",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,0.F};
							 fvec4 tmp( tval );
							 auto m = isinf( tmp );
							 return match_val<bool>( m.none(), true );
						 } );
	};
}

// test the fvec4-specific math functions
static void
add_load_store_tests( unit_test &test )
{
	test["load_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "load1f",
						 []() {
							 float tval = 42.F;
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval;
							 fvec4 tmp = load1f( &tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "load2f",
						 []() {
							 float tval[2] = {42.F,-1.F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 fvec4 tmp = load2f( tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "load3f",
						 []() {
							 float tval[3] = {42.F,-1.F,0.5F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 cval[2] = tval[2];
							 fvec4 tmp = load3f( tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "load4f",
						 []() {
							 float tval[4] = {42.F,-1.F,0.5F,18.F/0.F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 cval[2] = tval[2];
							 cval[3] = tval[3];
							 fvec4 tmp = load4f( tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "load4f_aligned",
						 []() {
							 float tval[4] PAL_ALIGN_128 = {42.F,-1.F,0.5F,18.F/0.F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 cval[2] = tval[2];
							 cval[3] = tval[3];
							 fvec4 tmp = load4f_aligned( tval );
							 return match( tmp, cval );
						 } );
	};
	test["store_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "store1",
						 []() {
							 float tval = 42.F;
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval;
							 fvec4 s( tval );
							 store1( &tval, s );
							 fvec4 tmp = load1f( &tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "store2",
						 []() {
							 float tval[2] = {42.F,-1.F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 fvec4 s = load2f( tval );
							 store2( tval, s );
							 fvec4 tmp = load2f( tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "store3",
						 []() {
							 float tval[3] = {42.F,-1.F,0.5F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 cval[2] = tval[2];
							 fvec4 s = load3f( tval );
							 store3( tval, s );
							 fvec4 tmp = load3f( tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "store",
						 []() {
							 float tval[4] = {42.F,-1.F,0.5F,18.F/0.F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 cval[2] = tval[2];
							 cval[3] = tval[3];
							 fvec4 s = load4f( tval );
							 store( tval, s );
							 fvec4 tmp = load4f( tval );
							 return match( tmp, cval );
						 } );
		TEST_CODE_VAL_EQ(test, "store_aligned",
						 []() {
							 float tval[4] PAL_ALIGN_128 = {42.F,-1.F,0.5F,18.F/0.F};
							 float cval[4] = {0.F,0.F,0.F,0.F};
							 cval[0] = tval[0];
							 cval[1] = tval[1];
							 cval[2] = tval[2];
							 cval[3] = tval[3];
							 fvec4 s = load4f( tval );
							 store_aligned( tval, s );
							 fvec4 tmp = load4f_aligned( tval );
							 return match( tmp, cval );
						 } );
	};
}

// test the fvec4-specific math functions
static void
add_math_tests( unit_test &test )
{
	test["simple_math_tests"] = [&]() {
		using namespace PAL_NAMESPACE;
		TEST_CODE_VAL_EQ(test, "max",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {5.1F,-1.1F,0.501F,-18.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::max( tval[i], tval2[i] );
							 return match( max( fvec4(tval), fvec4(tval2) ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "min",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {5.1F,-1.1F,0.501F,-18.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::min( tval[i], tval2[i] );
							 return match( min( fvec4(tval), fvec4(tval2) ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "fma",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {5.1F,-1.1F,0.501F,-18.F};
							 float tval3[4] = {0.1F,2.1F,3.501F,-4.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = tval[i] * tval2[i] + tval3[i];
							 return match( fma( fvec4(tval), fvec4(tval2), fvec4(tval3) ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "fms",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {5.1F,-1.1F,0.501F,-18.F};
							 float tval3[4] = {0.1F,2.1F,3.501F,-4.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = tval[i] * tval2[i] - tval3[i];
							 return match( fms( fvec4(tval), fvec4(tval2), fvec4(tval3) ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "nmadd",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {5.1F,-1.1F,0.501F,-18.F};
							 float tval3[4] = {0.1F,2.1F,3.501F,-4.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = -( tval[i] * tval2[i] ) + tval3[i];
							 return match( nmadd( fvec4(tval), fvec4(tval2), fvec4(tval3) ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "nmsub",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float tval2[4] = {5.1F,-1.1F,0.501F,-18.F};
							 float tval3[4] = {0.1F,2.1F,3.501F,-4.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = -( tval[i] * tval2[i] ) - tval3[i];
							 return match( nmsub( fvec4(tval), fvec4(tval2), fvec4(tval3) ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "hsum",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,18.F};
							 float cval[4];
							 cval[0] = tval[0]+tval[1]+tval[2]+tval[3];
							 for ( int i = 1; i != 4; ++i )
								 cval[i] = cval[0];
							 float hs = hsum( fvec4(tval) );
							 return match( fvec4(hs), cval );
						 } );

		TEST_CODE_VAL_EQ_ULPS(test, "faster_recip",
						 []() {
							 float tval[4] = {5000.F,-1.F,0.5F,18.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = 1.F / tval[i];
							 return match( faster_recip(fvec4(tval)), cval );
							  },
							  4096
							  );
		// newton raphson should double the precision, so should
		// be within 2 ULP
		TEST_CODE_VAL_EQ_ULPS(test, "fast_recip",
						 []() {
							 float tval[4] = {5.F,-1.F,0.5F,6743.149321F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = 1.F / tval[i];
							 return match( fast_recip(fvec4(tval)), cval );
							  }, 2 );
		TEST_CODE_VAL_EQ(test, "recip",
						 []() {
							 float tval[4] = {5000.F,-1.F,0.5F,18.F};
							 float cval[4];
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = 1.F / tval[i];
							 return match( recip(fvec4(tval)), cval );
							  } );
		TEST_CODE_VAL_EQ(test, "isnan",
						 []() {
							 float tval[4] = {std::numeric_limits<float>::quiet_NaN(),0.F,0.5F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = from_int( std::isnan( tval[i] ) ? uint32_t(-1) : 0 );
							 return match( fvec4( isnan( tmp ).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "isfinite",
						 []() {
							 float tval[4] = {std::numeric_limits<float>::quiet_NaN(),0.F,0.5F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = from_int( std::isfinite( tval[i] ) ? uint32_t(-1) : 0 );
							 return match( fvec4( isfinite( tmp ).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "isnormal",
						 []() {
							 float tval[4] = {std::numeric_limits<float>::quiet_NaN(),0.F,0.5F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = from_int( std::isnormal( tval[i] ) ? uint32_t(-1) : 0 );
							 return match( fvec4( isnormal( tmp ).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "isinf",
						 []() {
							 float tval[4] = {std::numeric_limits<float>::quiet_NaN(),0.F,0.5F,18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = from_int( std::isinf( tval[i] ) ? uint32_t(-1) : 0 );
							 return match( fvec4( isinf( tmp ).as_float() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "fpclassify",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {std::numeric_limits<float>::quiet_NaN(),0.F,0.5F,18.F/0.F};
							 int cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::fpclassify( tval[i] );
							 return intmatch( fpclassify( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "signbit",
						 []() {
							 float tval[4] = {-std::numeric_limits<float>::quiet_NaN(),0.F,-0.F,18.F/0.F};
							 int cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::signbit( tval[i] );
							 return intmatch( signbit( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "sqrtf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, 2.F, std::numeric_limits<float>::quiet_NaN(),18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::sqrt( tval[i] );
							 return match( sqrtf( tmp ), cval );
						 } );
		// 1.5 * 2^-12 so 4096 ulps
		TEST_CODE_VAL_EQ_ULPS(test, "faster_rsqrtf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, 2.F, std::numeric_limits<float>::quiet_NaN(),18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = 1.F / std::sqrt( tval[i] );
							 return match( faster_rsqrtf( tmp ), cval );
							  },
							  4096 );
		// should be double the precision given one round of NR
		// so 2 ULP but does not handle inf
		TEST_CODE_VAL_EQ_ULPS(test, "fast_rsqrtf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, 2.F, 4312.2341F,18.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = 1.F / std::sqrt( tval[i] );
							 return match( fast_rsqrtf( tmp ), cval );
							  },
							  2 );

		TEST_CODE_VAL_EQ(test, "rsqrtf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, 2.F, std::numeric_limits<float>::quiet_NaN(),18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = 1.F / std::sqrt( tval[i] );
							 return match( rsqrtf( tmp ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "fabsf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, -2.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::abs( tval[i] );
							 return match( fabsf( tmp ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "fabs",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, -2.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::abs( tval[i] );
							 return match( fabs( tmp ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "abs",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, -2.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::abs( tval[i] );
							 return match( abs( tmp ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "copysignf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F, 2.F, 3.F, 4.F};
							 float tval2[4] = {0.F, -0.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 tmp2( tval2 );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::copysignf( tval[i], tval2[i] );
							 return match( copysignf( tmp, tmp2 ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "truncf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F + std::numeric_limits<float>::epsilon(), 2.F, -3.5F, 4.5F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = truncf( tval[i] );
							 return match( truncf( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "floorf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F + std::numeric_limits<float>::epsilon(), 2.F, -3.5F, 4.5F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = floorf( tval[i] );
							 return match( floorf( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "ceilf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F + std::numeric_limits<float>::epsilon(), 2.F, -3.5F, 4.5F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = ceilf( tval[i] );
							 return match( ceilf( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "roundf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F + std::numeric_limits<float>::epsilon(), 2.F, 3.5F, -4.5F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = std::roundf( tval[i] );
							 return match( roundf( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "rintf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F + std::numeric_limits<float>::epsilon(), 2.F, 3.5F, -4.5F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = rintf( tval[i] );
							 return match( rintf( tmp ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "nearbyintf",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {1.F + std::numeric_limits<float>::epsilon(), 2.F, 3.5F, -4.5F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = nearbyintf( tval[i] );
							 return match( nearbyintf( tmp ), cval );
						 } );


		TEST_CODE_VAL_EQ(test, "clamp",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, 2.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 // remember to be careful about nan comparisons
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = ((tval[i] <= 1.F) ? (tval[i] >= 0.F ? tval[i] : 0.F) : 1.F);
							 return match( clamp( tmp, float_constants<fvec4>::zero(), float_constants<fvec4>::one() ), cval );
						 } );
		TEST_CODE_VAL_EQ(test, "clamp",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, 2.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 // remember to be careful about nan comparisons
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = ((tval[i] <= 1.F) ? (tval[i] >= 0.F ? tval[i] : 0.F) : 1.F);
							 return match( clamp( tmp, 0.F, 1.F ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "ifthen",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, -2.F, -std::numeric_limits<float>::quiet_NaN(),-18.F/0.F};
							 float trval[4] = {1.F, 2.F, 3.F, 4.F};
							 float faval[4] = {-1.F, -2.F, -3.F, -4.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 fvec4 t( trval );
							 fvec4 f( faval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = (tval[i] <= 0.F) ? trval[i] : faval[i];
							 return match( ifthen( tmp <= fvec4::zero(), t, f ), cval );
						 } );

		TEST_CODE_VAL_EQ(test, "square",
						 []() {
							 // TODO: add test for subnormal
							 float tval[4] = {0.2F, -2.F, 3.F, 12343.F};
							 float cval[4];
							 fvec4 tmp( tval );
							 for ( int i = 0; i != 4; ++i )
								 cval[i] = tval[i] * tval[i];
							 return match( square( tmp ), cval );
						 } );
	};
}

// test fvec4 against the exponential / log functions
static void
add_exp_tests( unit_test &test )
{
	using namespace PAL_NAMESPACE;
	test["exponentials"] = [&]() {
		TEST_CODE_VAL_EQ(
			test, "ilogb",
			[]() {
				float v[4] = { 0.0005F,-2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				int cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = std::ilogbf(v[i]);
				return intmatch( ilogb( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "ilogbf",
			[]() {
				float v[4] = { 0.0005F,-2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				int cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = std::ilogbf(v[i]);
				return intmatch( ilogbf( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "ldexp",
			[]() {
				float v[4] = { 0.0005F,-2.F,31.2F,123412.2F };
				float v2[4] = { 0.0001F,2.F,0.312F,0.1234122F };
				fvec4 tmp( v );
				fvec4 tmp2( v2 );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::ldexpf( v2[i], std::ilogbf( v[i] ) );
				return match( ldexp( tmp2, ilogb( tmp ) ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "ldexpf",
			[]() {
				float v[4] = { 0.0005F,-2.F,31.2F,123412.2F };
				float v2[4] = { 0.0001F,2.F,0.312F,0.1234122F };
				fvec4 tmp( v );
				fvec4 tmp2( v2 );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::ldexpf( v2[i], std::ilogbf( v[i] ) );
				return match( ldexpf( tmp2, ilogbf( tmp ) ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "frexp_v",
			[]() {
				float v[4] = { 0.0005F,-2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				fvec4::int_vec_type tmpe;
				float cval[4];
				int ival[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::frexpf( v[i], &ival[i] );
				fvec4 vt = frexpf( tmp, tmpe );
				return match( vt, cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "frexp_e",
			[]() {
				float v[4] = { 0.0005F,-2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				fvec4::int_vec_type tmpe;
				__attribute__((unused)) float cval[4];
				int ival[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::frexpf( v[i], &ival[i] );
				fvec4 vt = frexpf( tmp, tmpe );
				return intmatch( tmpe, ival );
			} );
		TEST_CODE_VAL_EQ(
			test, "log2f",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::log2f( v[i] );
				return match( log2f( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "logf",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::logf( v[i] );
				return match( logf( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "log10f",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::log10f( v[i] );
				return match( log10f( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ_ULPS(
			test, "faster_log2f",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::log2f( v[i] );
				return match( faster_log2f( tmp ), cval );
			}, 8192 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_log2f",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::log2f( v[i] );
				return match( fast_log2f( tmp ), cval );
			}, kFastULPprec );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_logf",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::logf( v[i] );
				return match( fast_logf( tmp ), cval );
			}, kFastULPprec );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_log10f",
			[]() {
				float v[4] = { 0.0005F,2.F,31.2F,123412.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::log10f( v[i] );
				return match( fast_log10f( tmp ), cval );
			}, kFastULPprec );
		TEST_CODE_VAL_EQ(
			test, "expf",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::expf( v[i] );
				return match( expf( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ(
			test, "exp2f",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::exp2f( v[i] );
				return match( exp2f( tmp ), cval );
			} );
		TEST_CODE_LONG_RUN(
			test, "exp2f_range",
			[&test]() {
				int maxULP = 0;
				int count = 0;
				float val;
				float abserr;
				for ( float x = -100.F; x <= 100.F; x += 0.0001F )
				{
					fvec4 tmp( x );
					float cval[4];
					cval[0] = std::exp2f( x );
					tmp = exp2f( tmp );
					match fcmp( tmp, cval );
					auto e = fcmp.ulp_error();
					int ulpDelta = e[0];
					if ( ulpDelta > maxULP )
					{
						maxULP = ulpDelta;
						val = x;
						abserr = ::fabsf( cval[0] - tmp[0] );
					}
					if ( (count++) % 1000 == 0 )
						test.run_tick( "exp2f_range" );
				}
				test.finish_run( "exp2f_range" );
				if ( maxULP <= 1 )
					test.success( "exp2f_range", "matches +/- 1 ULP" );
				else
				{
					std::stringstream msg;
					msg << "max error @ x " << val << " err " << abserr << " ULPS: " << maxULP;
					test.failure( "exp2f_range", msg.str() );
				}
			} );
		TEST_CODE_VAL_EQ(
			test, "exp10f",
			[]() {
				float v[4] = { -7.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::exp10f( v[i] );
				return match( exp10f( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_expf",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::expf( v[i] );
				return match( fast_expf( tmp ), cval );
			}, 1 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "faster_exp2f",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::exp2f( v[i] );
				return match( faster_exp2f( tmp ), cval );
			}, 32768 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_exp2f",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::exp2f( v[i] );
				return match( fast_exp2f( tmp ), cval );
			}, kFastULPprec );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_exp10f",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::exp10f( v[i] );
				return match( fast_exp10f( tmp ), cval );
			}, kFastULPprec );
		TEST_CODE_VAL_EQ_ULPS(
			test, "powf",
			[]() {
				float v[4] = { -37.124, 11.F, 2.F, 371.2F };
				float p[4] = { 2, 3.9998F, 0.1F, -0.723F };
				fvec4 tmpV( v );
				fvec4 tmpP( p );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::powf( v[i], p[i] );
				return match( powf( tmpV, tmpP ), cval );
			}, 4 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "powf addl",
			[]() {
				float v[4] = { 1.F, 0.F, 10000.F, -3.F };
				float p[4] = { 10000.F, 0.F, 10000.F, -0.723F };
				fvec4 tmpV( v );
				fvec4 tmpP( p );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::powf( v[i], p[i] );
				return match( powf( tmpV, tmpP ), cval );
			}, 4 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "powf_i",
			[]() {
				float v[4] = { 7.124, 0.0005F, 2.F, 371.2F };
//				int p[4] = { 2, 5, 1, -7.F };
				fvec4 tmpV( v );
//				ivec4 tmpP( p );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::powf( v[i], 3 );
				return match( powf( tmpV, 3 ), cval );
			}, 1 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "faster_powf",
			[]() {
				float v[4] = { 7.124, 0.15F, 0.534F, 371.2F };
				float p[4] = { 1/2.2F, 2.2F, 2.F, -0.723F };
				fvec4 tmpV( v );
				fvec4 tmpP( p );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::powf( v[i], p[i] );
				return match( faster_powf( tmpV, tmpP ), cval );
			}, 65536*4 );
		TEST_CODE_VAL_EQ_ULPS(
			test, "fast_powf",
			[]() {
				float v[4] = { -37.124, 0.0005F, 2.F, 371.2F };
				float p[4] = { 2, 5.F, 0.1F, -0.723F };
				fvec4 tmpV( v );
				fvec4 tmpP( p );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = ::powf( v[i], p[i] );
				return match( fast_powf( tmpV, tmpP ), cval );
			}, 64 );
#if 0
		TEST_CODE_LONG_RUN(
			test, "powf_range",
			[&test]() {
				int maxULP = 0;
				int count = 0;
				float val, valP, cV, sV, abserr;
				float maxAE, maeV, maeP, maeCV, maeSV;
				maxAE = 0.F;
				for ( float p = 0.F; p < 16.F; p += 0.035F )
				{
					fvec4 tmpP( p );
					for ( float x = 0.F; x <= 100.F; x += 0.0001F )
					{
						fvec4 tmp( x );
						float cval[4];
						cval[0] = ::powf( x, p );
						tmp = powf( tmp, tmpP );
						match fcmp( tmp, cval );
						auto e = fcmp.ulp_error();
						int ulpDelta = e[0];
						float tmpae = ::fabsf( cval[0] - tmp[0] );
						if ( ulpDelta > maxULP )
						{
							maxULP = ulpDelta;
							val = x;
							valP = p;
							cV = cval[0];
							sV = tmp[0];
							abserr = tmpae;
						}
						if ( tmpae > maxAE )
						{
							maeV = x;
							maeP = p;
							maeCV = cval[0];
							maeSV = tmp[0];
							maxAE = tmpae;
							
						}
						if ( (count++) % 10000 == 0 )
							test.run_tick( "powf_range" );
					}
				}
				test.finish_run( "powf_range" );
				if ( maxULP <= 4 )
					test.success( "powf_range", "matches +/- 1 ULP" );
				else
				{
					std::stringstream msg;
					msg << "max error @ pow( " << val << ", " << valP << " ) -> C lib " << cV << " sse " << sV << " abs err " << abserr << " ULPS: " << maxULP << " max AbsErr: " << maxAE << " @ " << maeV << ", " << maeP << " C " << maeCV << " SSE " << maeSV;
					test.failure( "powf_range", msg.str() );
				}
			} );
#endif
		TEST_CODE_VAL_EQ_ULPS(
			test, "cbrtf",
			[]() {
				float v[4] = { -37.124, 0.0027F, 2.F, 371.2F };
				fvec4 tmpV( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = std::cbrtf( v[i] );
				return match( cbrtf( tmpV ), cval );
			}, 2 );
	};
}

// test fvec4 against the exponential / log functions
static void
add_trig_tests( unit_test &test )
{
	using namespace PAL_NAMESPACE;
	test["trig"] = [&]() {
		TEST_CODE_VAL_EQ_PREC(
			test, "sinf",
			[]() {
				float v[4] = { 0.25F,-2.F,float(M_PI*2), 0.F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = std::sin( v[i] );
				return match( sinf( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ_PREC(
			test, "sinf_2",
			[]() {
				float v[4] = { -float(M_PI*17.0),float(M_PI*2) - 0.1F, 0.001F, float(-3.0*M_PI/2.0) };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = std::sin( v[i] );
				return match( sinf( tmp ), cval );
			} );
		TEST_CODE_VAL_EQ_PREC(
			test, "cosf",
			[]() {
				float v[4] = { 0.25F,-2.F,float(M_PI*2), 0.F };
				fvec4 tmp( v );
				float cval[4];
				for ( int i = 0; i != 4; ++i )
					cval[i] = std::cos( v[i] );
				return match( cosf( tmp ), cval );
			} );
	};
}

int main( int argc, char *argv[] )
{
	unit_test test( "fvec4" );

	add_class_tests( test );
	add_op_tests( test );
	add_load_store_tests( test );
	add_math_tests( test );
	add_exp_tests( test );
	add_trig_tests( test );

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
