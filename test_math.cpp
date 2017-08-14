#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <iomanip>
#include <vector>

#include "pal.h"

void testNormLoop( float *beg, const float *end )
{
}

struct Op
{
	inline float operator()( float a ) { return a + 0.3F; }
	PAL_INLINE pal::fvec4 operator()( pal::fvec4 a ) { return a + pal::fvec4(0.3F); }
#ifdef PAL_HAS_FVEC8
	PAL_INLINE pal::fvec8 operator()( pal::fvec8 a ) { return a + pal::fvec8(0.3F); }
#endif
};

struct speed_test1
{
	template <typename T>
	PAL_INLINE T operator()( T f ) { return ( ( f + 0.3F ) * f - 0.1F ) - sqrtf( f ); }
};

template <uint32_t n> 
struct BitScanR {
    enum {val = (
        n >= 0x10 ? 4 + (BitScanR<(n>>4)>::val) :
        n  <    2 ? 0 :
        n  <    4 ? 1 :
        n  <    8 ? 2 : 3 )                       };
};
template <> struct BitScanR<0> {enum {val = 0};};          // Avoid infinite template recursion

#define bit_scan_reverse_const(n)  (BitScanR<n>::val)      // n must be a valid compile-time constant

int main( int argc, char *argv[] )
{
	const uint32_t d1 = 3;
    const int32_t sh = bit_scan_reverse_const(d1-1);
    const int32_t mult = int(1 + (uint64_t(1) << (32+sh)) / uint32_t(d1) - (int64_t(1) << 32));   // multiplier
	std::cout << "d1 " << d1 << " --> mult " << mult << " sh " << sh << std::endl;
	const int32_t dx = 127;
	int64_t m = int64_t(dx) * int64_t(mult);
	uint64_t m2 = (uint64_t(m) >> 32);
	int32_t t7 = int32_t(m2);
	int32_t t8 = t7 + dx;
	int32_t t9 = t8 >> sh;
	// ignore the sign stuff for now
	int32_t out = t9;
	std::cout << " x " << dx << " / " << d1 << " --> m " << m << " m2 " << m2 << " " << out << std::endl;
	if ( argc != 3 )
	{
		std::cout << "Usage: " << argv[0] << " <x> <y>\n" << std::endl;
		return -1;
	}
	float x = atof( argv[1] );
	float y = atof( argv[2] );
	std::cout << "x: " << x << " y: " << y << " copysign( x, y ): " << copysignf( x, y ) << std::endl;

	pal::fvec4 xx( x );
	pal::fvec4 rxx = pal::recip( xx );
	std::cout << "recip rxx: " << rxx[0] << ' ' << rxx[1] << ' ' << rxx[2] << ' ' << rxx[3] << ' ' << std::endl;

	pal::fvec4 vals( 0.5, 1, 2, 3 );
	pal::fvec4 lvals = pal::log2f( vals );
	std::cout << "log2f: "
			  << lvals[0] << " (" << log2f( 0.5F ) << " 0.5) "
			  << lvals[1] << " (" << log2f( 1.F ) << " 1) "
			  << lvals[2] << " (" << log2f( 2.F ) << " 2) "
			  << lvals[3] << " (" << log2f( 3.F ) << " 3) "
			  << std::endl;
	pal::fvec4 lfvals = pal::logf( vals );
	std::cout << "logf: "
			  << lfvals[0] << " (" << logf( 0.5F ) << " 0.5) "
			  << lfvals[1] << " (" << logf( 1.F ) << " 1) "
			  << lfvals[2] << " (" << logf( 2.F ) << " 2) "
			  << lfvals[3] << " (" << logf( 3.F ) << " 3) "
			  << std::endl;
	pal::fvec4 l10vals = pal::log10f( vals );
	std::cout << "logf: "
			  << l10vals[0] << " (" << log10f( 0.5F ) << " 0.5) "
			  << l10vals[1] << " (" << log10f( 1.F ) << " 1) "
			  << l10vals[2] << " (" << log10f( 2.F ) << " 2) "
			  << l10vals[3] << " (" << log10f( 3.F ) << " 3) "
			  << std::endl;

	std::vector<float> theVecs;
	theVecs.resize( 43236152, 0.F );
	pal::process_inplace( theVecs.data(), theVecs.data() + theVecs.size(), Op() );
#if __cplusplus >= 201402L
	pal::process_inplace( theVecs.data(), theVecs.data() + theVecs.size(), [](auto f) { return ( ( f + 0.3F ) * f - 0.1F ) - sqrtf( f ); } );
#else
	pal::process_inplace( theVecs.data(), theVecs.data() + theVecs.size(), speed_test1() );
#endif
//	pal::process_inplace( theVecs.data() + 3, theVecs.data() + theVecs.size(), [](auto f) { return f + 0.3F; } );
//	pal::process_inplace( theVecs.data(), theVecs.data() + 3, [](auto f) { return f + 0.3F; } );
//	for ( size_t i = 0; i != theVecs.size(); ++i )
//	{
//		if ( theVecs[i] < 0.6F || theVecs[i] > 0.6F )
//			std::cout << "VEC " << i << " doesn't match: " << theVecs[i] << " vs 0.6" << std::endl;
//	}

#if 0
	union
	{
		unsigned int i;
		float f;
	} arr[4];
	arr[0].f = x;
	arr[1].f = y;
	arr[2].i = 0x7F800000;
	arr[3].f = 0.F/0.F;

	union
	{
		unsigned int i;
		float f;
	} arr2[4];
	arr2[0].f = x;
	arr2[1].f = y;
	arr2[2].i = 0xFF800000;
	arr2[3].f = x;

	__m128 vx = _mm_loadu_ps( reinterpret_cast<const float *>( arr ) );
	__m128 vy = _mm_loadu_ps( reinterpret_cast<const float *>( arr2 ) );
	__m128 mnx = _mm_min_ps( vx, vy );
	std::cout << "minps mnx: " << ((__v4sf)mnx)[0] << ' ' << ((__v4sf)mnx)[1] << ' ' << ((__v4sf)mnx)[2] << ' ' << ((__v4sf)mnx)[3] << ' ' << std::endl;
	__m128 mxx = _mm_max_ps( vy, vx );
	std::cout << "minps mxx: " << ((__v4sf)mxx)[0] << ' ' << ((__v4sf)mxx)[1] << ' ' << ((__v4sf)mxx)[2] << ' ' << ((__v4sf)mxx)[3] << ' ' << std::endl;

	__m128i foo = _mm_cmpeq_epi32( _mm_castps_si128( vx ), _mm_castps_si128( vx ) );
	__m128i bar = _mm_srli_epi32( foo, 1 );
	__m128 absx = _mm_and_ps( vx, _mm_castsi128_ps( _mm_srli_epi32( _mm_cmpeq_epi32( _mm_castps_si128( vx ), _mm_castps_si128( vx ) ), 1 ) ) );
	std::cout << "absx: " << ((__v4sf)absx)[0] << ' ' << ((__v4sf)absx)[1] << ' ' << ((__v4sf)absx)[2] << ' ' << ((__v4sf)absx)[3] << ' ' << std::setw(8) << std::setfill('0') << std::hex << ((__v4su)bar)[3] << std::dec << std::endl;

	__m128 cx = _mm_cmpeq_ps( vx, _mm_setzero_ps() );
	std::cout << "cmpeq 0 ps: movemask 0x" << std::setw(2) << std::setfill('0') << std::hex << _mm_movemask_ps( cx ) << std::dec << std::endl;
	__m128 ncx = _mm_cmpneq_ps( vx, _mm_setzero_ps() );
	std::cout << "cmpneq 0 ps: movemask 0x" << std::setw(2) << std::setfill('0') << std::hex << _mm_movemask_ps( ncx ) << std::dec << std::endl;
	__m128 acx = _mm_and_ps( _mm_cmpord_ps( vx, vx ), _mm_cmpunord_ps( vx, _mm_mul_ps( _mm_setzero_ps(), vx ) ) );
	std::cout << "cmpord 0 ps: movemask 0x" << std::setw(2) << std::setfill('0') << std::hex << _mm_movemask_ps( acx ) << std::dec << std::endl;
//	__m128 vx = _mm_set1_ps( x );
//	__m128 vy = _mm_set1_ps( y );
//	// (b0 < 0) ? -a0 : (b0 == 0) ? 0 : a0;
//
//	std::cout << "input vx: " << ((__v4sf)vx)[0] << ' ' << ((__v4sf)vx)[1] << ' ' << ((__v4sf)vx)[2] << ' ' << ((__v4sf)vx)[3] << ' ' << std::endl;
//
//	__m128 sx = _mm_castsi128_ps( _mm_sign_epi32( _mm_castps_si128( vx ), _mm_castps_si128( vy ) ) );
//	__m128 sx = _mm_castsi128_ps( _mm_abs_epi32( _mm_castps_si128( vx ) ) );
//
//	std::cout << "output sx: " << ((__v4sf)sx)[0] << ' ' << ((__v4sf)sx)[1] << ' ' << ((__v4sf)sx)[2] << ' ' << ((__v4sf)sx)[3] << ' ' << std::endl;
#endif
	return 0;
}
