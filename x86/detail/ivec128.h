//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use detail/ivec128.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_DETAIL_IVEC128_H_
# define _PAL_X86_DETAIL_IVEC128_H_ 1

namespace PAL_NAMESPACE
{

namespace detail
{

template <size_t T> struct ivec128_traits {};

template <> struct ivec128_traits<1>
{
	typedef char itype;
	static const int value_count = 16;
	static PAL_INLINE __m128i splat( itype v ) { return _mm_set1_epi8( v ); }
	static PAL_INLINE __m128i init( itype a0, itype a1, itype a2, itype a3,
									itype a4, itype a5, itype a6, itype a7,
									itype a8, itype a9, itype a10, itype a11,
									itype a12, itype a13, itype a14, itype a15 )
	{
		return _mm_set_epi8( a15, a14, a13, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, a0 );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const U (&a)[16] )
	{
		return _mm_set_epi8( itype(a[15]), itype(a[14]),
							 itype(a[13]), itype(a[12]),
							 itype(a[11]), itype(a[10]),
							 itype(a[9]), itype(a[8]),
							 itype(a[7]), itype(a[6]),
							 itype(a[5]), itype(a[4]),
							 itype(a[3]), itype(a[2]),
							 itype(a[1]), itype(a[0]) );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const std::array<U,16> &a )
	{
		return _mm_set_epi8( itype(a[15]), itype(a[14]),
							 itype(a[13]), itype(a[12]),
							 itype(a[11]), itype(a[10]),
							 itype(a[9]), itype(a[8]),
							 itype(a[7]), itype(a[6]),
							 itype(a[5]), itype(a[4]),
							 itype(a[3]), itype(a[2]),
							 itype(a[1]), itype(a[0]) );
	}

		
	static PAL_INLINE __m128i addu( __m128i a, __m128i b ) { return _mm_add_epi8( a, b ); }
	static PAL_INLINE __m128i adds( __m128i a, __m128i b ) { return _mm_adds_epi8( a, b ); }

	static PAL_INLINE __m128i subu( __m128i a, __m128i b ) { return _mm_sub_epi8( a, b ); }
	static PAL_INLINE __m128i subs( __m128i a, __m128i b ) { return _mm_subs_epi8( a, b ); }

	static PAL_INLINE __m128i mul( __m128i a, __m128i b )
	{
		// no 8-bit mul in sse2, do as 16
		__m128i m0 = _mm_mullo_epi16( a, b );
		__m128i m1 = _mm_slli_epi16(
			_mm_mullo_epi16( _mm_srli_epi16( a, 8 ),
							 _mm_srli_epi16( b, 8 ) ),
			8 );
#ifdef PAL_ENABLE_SSE4_1
		return _mm_blendv_epi8( m1, m0, _mm_set1_epi32( 0x00FF00FF ) );
#else
		__m128i mask = _mm_set1_epi32( 0x00FF00FF);
		return _mm_or_si128( _mm_and_si128( mask, m0 ),
							 _mm_andnot_si128( mask, m1 ) );
#endif
	}
	
	static PAL_INLINE itype access( __m128i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v16qi)a)[idx];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_128 itype vals[value_count];
		_mm_store_ps( reinterpret_cast<__m128i *>( vals ), a );
		return vals[idx];
#endif
	}
};

template <> struct ivec128_traits<2>
{
	typedef short itype;
	static const int value_count = 8;
	static PAL_INLINE __m128i splat( itype v ) { return _mm_set1_epi16( v ); }
	static PAL_INLINE __m128i init( itype a0, itype a1, itype a2, itype a3,
									itype a4, itype a5, itype a6, itype a7 )
	{
		return _mm_set_epi16( a7, a6, a5, a4, a3, a2, a1, a0 );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const U (&a)[8] )
	{
		return _mm_set_epi16( itype(a[7]), itype(a[6]), 
							  itype(a[5]), itype(a[4]),
							  itype(a[3]), itype(a[2]),
							  itype(a[1]), itype(a[0]) );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const std::array<U,8> &a )
	{
		return _mm_set_epi16( itype(a[7]), itype(a[6]),
							  itype(a[5]), itype(a[4]),
							  itype(a[3]), itype(a[2]),
							  itype(a[1]), itype(a[0]) );
	}

	static PAL_INLINE __m128i addu( __m128i a, __m128i b ) { return _mm_add_epi16( a, b ); }
	static PAL_INLINE __m128i adds( __m128i a, __m128i b ) { return _mm_adds_epi16( a, b ); }

	static PAL_INLINE __m128i subu( __m128i a, __m128i b ) { return _mm_sub_epi16( a, b ); }
	static PAL_INLINE __m128i subs( __m128i a, __m128i b ) { return _mm_subs_epi16( a, b ); }

	static PAL_INLINE __m128i mul( __m128i a, __m128i b )
	{ return _mm_mullo_epi16( a, b ); }

	static PAL_INLINE itype access( __m128i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v8hi)a)[idx];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_128 itype vals[value_count];
		_mm_store_ps( reinterpret_cast<__m128i *>( vals ), a );
		return vals[idx];
#endif
	}
};

template <> struct ivec128_traits<4>
{
	typedef int itype;
	static const int value_count = 4;
	static PAL_INLINE __m128i splat( itype v ) { return _mm_set1_epi32( v ); }
	static PAL_INLINE __m128i init( itype a0, itype a1, itype a2, itype a3 )
	{
		return _mm_set_epi32( a3, a2, a1, a0 );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const U (&a)[4] )
	{
		return _mm_set_epi32( itype(a[3]), itype(a[2]),
							  itype(a[1]), itype(a[0]) );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const std::array<U,4> &a )
	{
		return _mm_set_epi32( itype(a[3]), itype(a[2]),
							  itype(a[1]), itype(a[0]) );
	}

	static PAL_INLINE __m128i addu( __m128i a, __m128i b ) { return _mm_add_epi32( a, b ); }
	// huh, gcc doesn't seem to provide a signed 32-bit add
	static PAL_INLINE __m128i adds( __m128i a, __m128i b ) { return _mm_add_epi32( a, b ); }

	static PAL_INLINE __m128i subu( __m128i a, __m128i b ) { return _mm_sub_epi32( a, b ); }
	// huh, gcc doesn't seem to provide a signed 32-bit subtract
	static PAL_INLINE __m128i subs( __m128i a, __m128i b ) { return _mm_sub_epi32( a, b ); }

	static PAL_INLINE __m128i mul( __m128i a, __m128i b )
	{
#if PAL_ENABLE_SSE4_1
		return _mm_mullo_epi32( a, b );
#else
		__m128i a02 = _mm_mul_epu32( a, b );
		__m128i a13 = _mm_mul_epu32( _mm_srli_epi64( a, 32 ),
									 _mm_srli_epi64( b, 32 ) );
		return _mm_unpacklo_epi64( _mm_unpacklo_epi32( a02, a13 ),
								   _mm_unpackhi_epi32( a02, a13 ) );
#endif
	}
	
	static PAL_INLINE itype access( __m128i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v4si)a)[idx];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_128 itype vals[value_count];
		_mm_store_ps( reinterpret_cast<__m128i *>( vals ), a );
		return vals[idx];
#endif
	}
};

template <> struct ivec128_traits<8>
{
	typedef long long itype;
	static const int value_count = 2;
	static PAL_INLINE __m128i splat( itype v ) { return _mm_set1_epi64x( v ); }
	static PAL_INLINE __m128i init( itype a0, itype a1 )
	{
		return _mm_set_epi64x( a1, a0 );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const U (&a)[2] )
	{
		return _mm_set_epi64x( itype(a[1]), itype(a[0]) );
	}
	template <typename U>
	static PAL_INLINE __m128i init( const std::array<U,4> &a )
	{
		return _mm_set_epi64x( itype(a[1]), itype(a[0]) );
	}

	static PAL_INLINE __m128i addu( __m128i a, __m128i b ) { return _mm_add_epi64( a, b ); }
	// huh, gcc doesn't seem to provide a signed 64-bit add
	static PAL_INLINE __m128i adds( __m128i a, __m128i b ) { return _mm_add_epi64( a, b ); }

	static PAL_INLINE __m128i subu( __m128i a, __m128i b ) { return _mm_sub_epi64( a, b ); }
	// huh, gcc doesn't seem to provide a signed 64-bit subtract
	static PAL_INLINE __m128i subs( __m128i a, __m128i b ) { return _mm_sub_epi64( a, b ); }

	static PAL_INLINE __m128i mul( __m128i a, __m128i b )
	{
		// TODO: in AVX512 there is a mullo_epi64...
		return _mm_set_epi64x( access( a, 1 ) * access( b, 1 ),
							   access( a, 0 ) * access( b, 0 ) );
	}

	static PAL_INLINE itype access( __m128i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v2di)a)[idx];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_128 itype vals[value_count];
		_mm_store_ps( reinterpret_cast<__m128i *>( vals ), a );
		return vals[idx];
#endif
	}
};

} // namespace detail

} // namespace pal

#endif // _PAL_X86_DETAIL_IVEC128_H_
