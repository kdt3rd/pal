//
// Copyright (c) 2016 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use detail/mask128.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_DETAIL_MASK128_H_
# define _PAL_X86_DETAIL_MASK128_H_ 1

namespace PAL_NAMESPACE
{

namespace detail
{

template <typename VT> struct mask128_bitops
{
	typedef VT value_type;
	static_assert( std::is_integral<value_type>::value, "Expecting integer type" );
	typedef __m128i vec_type;

	static PAL_INLINE vec_type no( void ) { return _mm_setzero_si128(); }
	static PAL_INLINE vec_type yes( void )
	{
		// fastest way is to define an undefined value and then
		// compare if that is equal to itself
		// there is an _mm_undefined_ps function but not all
		// compilers have that, so emulate it here
		vec_type tmp = tmp;
		return _mm_cmpeq_epi8( tmp, tmp );
	}

	static PAL_INLINE vec_type from_other( __m128 v ) { return _mm_castps_si128( v ); }
	static PAL_INLINE vec_type from_other( __m128d v ) { return _mm_castpd_si128( v ); }
	static PAL_INLINE vec_type from_other( __m128i v ) { return v; }

	static PAL_INLINE __m128 as_float( vec_type v ) { return _mm_castsi128_ps( v ); }
	static PAL_INLINE __m128d as_double( vec_type v ) { return _mm_castsi128_pd( v ); }
	static PAL_INLINE __m128i as_int( vec_type v ) { return v; }
	
	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm_and_si128( a, b ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm_andnot_si128( a, b ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm_or_si128( a, b ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm_xor_si128( a, b ); }

	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm_cmpeq_epi8( a, b ); }

	// if m.bit returns b.bit else a
	static PAL_INLINE vec_type bit_mix( vec_type m, vec_type a, vec_type b )
	{
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
	}

	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_blendv_epi8( a, b, m );
#else
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
#endif
	}

	static PAL_INLINE bool any( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_zeros( m, m ) == 0;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_epi8( m ) != 0;
#endif
	}

	static PAL_INLINE bool all( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_ones( m ) == 1;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_epi8( m ) == 0xFFFF;
#endif
	}

	static PAL_INLINE bool none( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_zeros( m, m ) == 1;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_epi8( m ) == 0;
#endif
	}
};

template <> struct mask128_bitops<double>
{
	typedef double value_type;
	typedef __m128d vec_type;

	static PAL_INLINE vec_type no( void ) { return _mm_setzero_pd(); }
	static PAL_INLINE vec_type yes( void )
	{
		// fastest way is to define an undefined value and then
		// compare if that is equal to itself
		// there is an _mm_undefined_ps function but not all
		// compilers have that, so emulate it here
		__m128i tmp = tmp;
		return _mm_castsi128_pd( _mm_cmpeq_epi8( tmp, tmp ) );
	}

	static PAL_INLINE vec_type from_other( __m128 v ) { return _mm_castps_pd( v ); }
	static PAL_INLINE vec_type from_other( __m128d v ) { return v; }
	static PAL_INLINE vec_type from_other( __m128i v ) { return _mm_castsi128_pd( v ); }

	static PAL_INLINE __m128 as_float( vec_type v ) { return _mm_castpd_ps( v ); }
	static PAL_INLINE __m128d as_double( vec_type v ) { return v; }
	static PAL_INLINE __m128i as_int( vec_type v ) { return _mm_castpd_si128( v ); }
	
	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm_and_pd( a, b ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm_andnot_pd( a, b ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm_or_pd( a, b ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm_xor_pd( a, b ); }

	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm_castsi128_pd( _mm_cmpeq_epi8( as_int( a ), as_int( b ) ) ); }

	// if m.bit returns b.bit else a
	static PAL_INLINE vec_type bit_mix( vec_type m, vec_type a, vec_type b )
	{
		// slightly higher throughput until skylake on the integer engine
		return _mm_castsi128_pd(
			_mm_or_si128(
				_mm_andnot_si128( _mm_castpd_si128( m ), _mm_castpd_si128( a ) ),
				_mm_and_si128( _mm_castpd_si128( m ), _mm_castpd_si128( b ) ) ) );
	}

	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_blendv_pd( a, b, m );
#else
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
#endif
	}

	static PAL_INLINE bool any( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_zeros( as_int( m ), as_int( m ) ) == 0;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_pd( m ) != 0;
#endif
	}

	static PAL_INLINE bool all( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_ones( as_int( m ) ) == 1;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_pd( m ) == 0x3;
#endif
	}

	static PAL_INLINE bool none( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_zeros( as_int( m ), as_int( m ) ) == 1;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_pd( m ) == 0;
#endif
	}
};

template <> struct mask128_bitops<float>
{
	typedef float value_type;
	typedef __m128 vec_type;

	static PAL_INLINE vec_type no( void ) { return _mm_setzero_ps(); }
	static PAL_INLINE vec_type yes( void )
	{
		// fastest way is to define an undefined value and then
		// compare if that is equal to itself
		// there is an _mm_undefined_ps function but not all
		// compilers have that, so emulate it here
		__m128i tmp = tmp;
		return _mm_castsi128_ps( _mm_cmpeq_epi8( tmp, tmp ) );
	}

	static PAL_INLINE vec_type from_other( __m128 v ) { return v; }
	static PAL_INLINE vec_type from_other( __m128d v ) { return _mm_castpd_ps( v ); }
	static PAL_INLINE vec_type from_other( __m128i v ) { return _mm_castsi128_ps( v ); }

	static PAL_INLINE __m128 as_float( vec_type v ) { return v; }
	static PAL_INLINE __m128d as_double( vec_type v ) { return _mm_castps_pd( v ); }
	static PAL_INLINE __m128i as_int( vec_type v ) { return _mm_castps_si128( v ); }
	
	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm_and_ps( a, b ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm_andnot_ps( a, b ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm_or_ps( a, b ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm_xor_ps( a, b ); }

	// TODO: determine if domain change from float to int triggers. if not, then switch this to cmpeq_epi8 for SSE2 and beyond
	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm_cmpeq_ps( a, b ); }

	// if m.bit returns b.bit else a
	static PAL_INLINE vec_type bit_mix( vec_type m, vec_type a, vec_type b )
	{
		// slightly higher throughput until skylake on the integer engine
		return _mm_castsi128_ps(
			_mm_or_si128(
				_mm_andnot_si128( _mm_castps_si128( m ), _mm_castps_si128( a ) ),
				_mm_and_si128( _mm_castps_si128( m ), _mm_castps_si128( b ) ) ) );
	}

	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_blendv_ps( a, b, m );
#else
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
#endif
	}

	static PAL_INLINE bool any( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_zeros( as_int( m ), as_int( m ) ) == 0;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_ps( m ) != 0;
#endif
	}

	static PAL_INLINE bool all( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_ones( as_int( m ) ) == 1;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_ps( m ) == 0xF;
#endif
	}

	static PAL_INLINE bool none( vec_type m )
	{
#ifdef PAL_ENABLE_SSE4_1
		return _mm_test_all_zeros( as_int( m ), as_int( m ) ) == 1;
#else
		// technically, only tests high bit, but meh
		return _mm_movemask_ps( m ) == 0;
#endif
	}
};

////////////////////////////////////////

template <size_t S, typename T> struct mask128_traits {};

template <typename T> struct mask128_traits<1,T>
{
	static_assert( std::is_integral<T>::value, "Expecting integer type" );
	typedef __m128i vec_type;
	typedef T bitmask_type;
	static const int value_count = 16;

	template <typename V>
	static PAL_INLINE vec_type splat( V v ) { return _mm_set1_epi8( static_cast<char>( v ) ); }

	static PAL_INLINE vec_type init( bool a0, bool a1, bool a2, bool a3,
									 bool a4, bool a5, bool a6, bool a7,
									 bool a8, bool a9, bool a10, bool a11,
									 bool a12, bool a13, bool a14, bool a15 )
	{
		return _mm_set_epi8( a15?-1:0, a14?-1:0, a13?-1:0, a12?-1:0, a11?-1:0, a10?-1:0, a9?-1:0, a8?-1:0, a7?-1:0, a6?-1:0, a5?-1:0, a4?-1:0, a3?-1:0, a2?-1:0, a1?-1:0, a0?-1:0 );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v16qi)v)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_128 bitmask_type vals[value_count];
		_mm_store_si128( reinterpret_cast<__m128i *>( vals ), v );
		return vals[i];
#endif
	}
};

template <typename T> struct mask128_traits<2,T>
{
	static_assert( std::is_integral<T>::value, "Expecting integer type" );
	typedef __m128i vec_type;
	typedef T bitmask_type;
	static const int value_count = 8;

	template <typename V>
	static PAL_INLINE vec_type splat( V v ) { return _mm_set1_epi16( static_cast<short>( v ) ); }

	static PAL_INLINE vec_type init( bool a0, bool a1, bool a2, bool a3,
									 bool a4, bool a5, bool a6, bool a7 )
	{
		return _mm_set_epi16( a7?-1:0, a6?-1:0, a5?-1:0, a4?-1:0, a3?-1:0, a2?-1:0, a1?-1:0, a0?-1:0 );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v8hi)v)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_128 bitmask_type vals[value_count];
		_mm_store_si128( reinterpret_cast<__m128i *>( vals ), v );
		return vals[i];
#endif
	}
};

template <typename T> struct mask128_traits<4,T>
{
	static_assert( std::is_integral<T>::value, "Expecting integer type" );
	typedef __m128i vec_type;
	typedef T bitmask_type;
	static const int value_count = 4;

	template <typename V>
	static PAL_INLINE vec_type splat( V v ) { return _mm_set1_epi32( static_cast<int>( v ) ); }

	static PAL_INLINE vec_type init( bool a0, bool a1, bool a2, bool a3 )
	{
		return _mm_set_epi32( a3?-1:0, a2?-1:0, a1?-1:0, a0?-1:0 );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v4si)v)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128i vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_128 bitmask_type vals[value_count];
		_mm_store_si128( reinterpret_cast<__m128i *>( vals ), v );
		return vals[i];
#endif
	}
};

template <> struct mask128_traits<4,float>
{
	typedef __m128 vec_type;
	typedef uint32_t bitmask_type;

	static const int value_count = 4;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm_castsi128_ps( _mm_set1_epi32( static_cast<int>( v ) ) ); }

	static PAL_INLINE vec_type init( bool a0, bool a1, bool a2, bool a3 )
	{
		return _mm_castsi128_ps( _mm_set_epi32( a3?-1:0, a2?-1:0, a1?-1:0, a0?-1:0 ) );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		union
		{
			float f;
			bitmask_type b;
		} c;
		c.f = ((__v4sf)v)[i];
		return c.b;
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128 vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_128 bitmask_type vals[value_count];
		_mm_store_ps( reinterpret_cast<float *>( vals ), v );
		return vals[i];
#endif
	}
};

template <> struct mask128_traits<8,double>
{
	typedef __m128d vec_type;
	typedef uint64_t bitmask_type;

	static const int value_count = 2;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm_castsi128_pd( _mm_set1_epi64x( static_cast<long long>( v ) ) ); }

	static PAL_INLINE vec_type init( bool a0, bool a1 )
	{
		return _mm_castsi128_pd( _mm_set_epi64x( a1?-1:0, a0?-1:0 ) );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		union
		{
			double f;
			bitmask_type i;
		} c;
		c.f = ((__v2df)v)[i];
		return c.i;
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m128d vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_128 bitmask_type vals[value_count];
		_mm_store_pd( reinterpret_cast<double *>( vals ), v );
		return vals[i];
#endif
	}
};

//#ifndef PAL_ENABLE_SSE2
//template <> struct mask128_traits<4,float>
//{
//	typedef __m128 vec_type;
//	typedef uint32_t bitmask_type;
//
//	static const int value_count = 4;
//
//	template <typename V>
//	static PAL_INLINE vec_type splat( V v )
//	{
//		union 
//		{
//			float f;
//			int i;
//		} val;
//		val.i = static_cast<int>( v );
//		return _mm_set1_ps( val.f );
//	}
//
//	static PAL_INLINE vec_type init( bool a0, bool a1, bool a2, bool a3 )
//	{
//		return _mm_cmpgt_ps( _mm_set_ps( a3?1.F:0.F,
//										 a2?1.F:0.F,
//										 a1?1.F:0.F,
//										 a0?1.F:0.F ),
//							 _mm_setzero_ps() );
//	}
//};
//#endif // legacy old no SSE2 support

} // namespace detail

} // namespace pal

#endif // _PAL_X86_DETAIL_MASK128_H_
