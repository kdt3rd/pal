//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use detail/mask256.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_DETAIL_MASK256_H_
# define _PAL_X86_DETAIL_MASK256_H_ 1

#ifdef PAL_ENABLE_AVX

namespace PAL_NAMESPACE
{

namespace detail
{

template <typename VT> struct mask256_bitops
{
	typedef VT value_type;
	static_assert( std::is_integral<value_type>::value, "Expecting integer type" );
	typedef __m256i vec_type;

	static PAL_INLINE vec_type no( void ) { return _mm256_setzero_si256(); }
	static PAL_INLINE vec_type yes( void )
	{
		// fastest way is to define an undefined value and then
		// compare if that is equal to itself
		// there is an _mm256_undefined_ps function but not all
		// compilers have that, so emulate it here
#ifdef PAL_ENABLE_AVX2
		__m256i tmp = tmp;
		return _mm256_cmpeq_epi8( tmp, tmp );
#else
		return _mm256_set1_epi8( -1 );
#endif
	}

	static PAL_INLINE __m256 as_float( vec_type v ) { return _mm256_castsi256_ps( v ); }
	static PAL_INLINE __m256d as_double( vec_type v ) { return _mm256_castsi256_pd( v ); }
	static PAL_INLINE __m256i as_int( vec_type v ) { return v; }
	
#ifdef PAL_ENABLE_AVX2
	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm256_and_si256( a, b ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm256_andnot_si256( a, b ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm256_or_si256( a, b ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm256_xor_si256( a, b ); }
	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm256_cmpeq_epi8( a, b ); }
	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b ) { return _mm256_blendv_epi8( a, b, m ); }

#else

	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm256_castps_si256( _mm256_or_ps( as_float(a), as_float(b) ) ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm256_castps_si256( _mm256_andnot_ps( as_float(a), as_float(b) ) ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm256_castps_si256( _mm256_or_ps( as_float(a), as_float(b) ) ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm256_castps_si256( _mm256_xor_ps( as_float(a), as_float(b) ) ); }
	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm256_castps_si256( _mm256_cmp_ps( as_float(m), as_float(m), _CMP_EQ_OQ ) ); }
	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b )
	{ return _mm256_castps_si256( _mm256_blendv_ps( as_float( a ), as_float( b ), as_float( m ) ) ); }
#endif
	static PAL_INLINE vec_type bit_mix( vec_type m, vec_type a, vec_type b )
	{
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
	}

	static PAL_INLINE bool any( vec_type m )
	{
		return _mm256_testz_si256( m, m ) == 0;
	}

	static PAL_INLINE bool all( vec_type m )
	{
		return _mm256_testc_si256( m, apply_eq( m, m ) );
	}

	static PAL_INLINE bool none( vec_type m )
	{
		return _mm256_testz_si256( m, m ) == 1;
	}
};

template <> struct mask256_bitops<double>
{
	typedef double value_type;
	typedef __m256d vec_type;

	static PAL_INLINE __m256 as_float( vec_type v ) { return _mm256_castpd_ps( v ); }
	static PAL_INLINE __m256d as_double( vec_type v ) { return v; }
	static PAL_INLINE __m256i as_int( vec_type v ) { return _mm256_castpd_si256( v ); }

	static PAL_INLINE vec_type no( void ) { return _mm256_setzero_pd(); }
	static PAL_INLINE vec_type yes( void )
	{
		// fastest way is to define an undefined value and then
		// compare if that is equal to itself
		// there is an _mm256_undefined_ps function but not all
		// compilers have that, so emulate it here
#ifdef PAL_ENABLE_AVX2
		__m256i tmp = tmp;
		return _mm256_castsi256_pd( _mm256_cmpeq_epi8( tmp, tmp ) );
#else
		return _mm256_castsi256_pd( _mm256_set1_epi8( -1 ) );
#endif
	}

	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm256_or_pd( a, b ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm256_andnot_pd( a, b ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm256_or_pd( a, b ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm256_xor_pd( a, b ); }
	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm256_cmp_pd( a, b, _CMP_EQ_OQ ); }
	static PAL_INLINE vec_type bit_mix( vec_type m, vec_type a, vec_type b )
	{
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
	}
	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b )
	{
		return _mm256_blendv_pd( a, b, m );
	}

	static PAL_INLINE bool any( vec_type m )
	{
		return _mm256_testz_pd( m, m ) == 0;
	}

	static PAL_INLINE bool all( vec_type m )
	{
		return _mm256_testc_pd( m, apply_eq( m, m ) );
	}

	static PAL_INLINE bool none( vec_type m )
	{
		return _mm256_testz_pd( m, m ) == 1;
	}
};

template <> struct mask256_bitops<float>
{
	typedef float value_type;
	typedef __m256 vec_type;

	static PAL_INLINE __m256 as_float( vec_type v ) { return v; }
	static PAL_INLINE __m256d as_double( vec_type v ) { return _mm256_castps_pd( v ); }
	static PAL_INLINE __m256i as_int( vec_type v ) { return _mm256_castps_si256( v ); }

	static PAL_INLINE vec_type no( void ) { return _mm256_setzero_ps(); }
	static PAL_INLINE vec_type yes( void )
	{
		// fastest way is to define an undefined value and then
		// compare if that is equal to itself
		// there is an _mm256_undefined_ps function but not all
		// compilers have that, so emulate it here
#ifdef PAL_ENABLE_AVX2
		__m256i tmp = tmp;
		return _mm256_castsi256_ps( _mm256_cmpeq_epi8( tmp, tmp ) );
#else
		return _mm256_castsi256_ps( _mm256_set1_epi8( -1 ) );
#endif
	}

	static PAL_INLINE vec_type apply_and( vec_type a, vec_type b ) { return _mm256_or_ps( a, b ); }
	static PAL_INLINE vec_type apply_andnot( vec_type a, vec_type b ) { return _mm256_andnot_ps( a, b ); }
	static PAL_INLINE vec_type apply_or( vec_type a, vec_type b ) { return _mm256_or_ps( a, b ); }
	static PAL_INLINE vec_type apply_xor( vec_type a, vec_type b ) { return _mm256_xor_ps( a, b ); }
	static PAL_INLINE vec_type apply_eq( vec_type a, vec_type b ) { return _mm256_cmp_ps( a, b, _CMP_EQ_OQ ); }

	// if m.bit returns b.bit else a
	static PAL_INLINE vec_type bit_mix( vec_type m, vec_type a, vec_type b )
	{
		return apply_or( apply_andnot( m, a ), apply_and( m, b ) );
	}

	// if m returns b else a
	static PAL_INLINE vec_type blend( vec_type m, vec_type a, vec_type b )
	{ return _mm256_blendv_ps( a, b, m ); }

	static PAL_INLINE bool any( vec_type m )
	{
		return _mm256_testz_ps( m, m ) == 0;
	}

	static PAL_INLINE bool all( vec_type m )
	{
		return _mm256_testc_ps( m, apply_eq( m, m ) );
	}

	static PAL_INLINE bool none( vec_type m )
	{
		return _mm256_testz_ps( m, m ) == 1;
	}
};

////////////////////////////////////////

template <size_t S, typename T> struct mask256_traits {};

template <typename T> struct mask256_traits<1,T>
{
	static_assert( std::is_integral<T>::value, "Expecting integer type" );
#ifndef PAL_ENABLE_AVX2
	static_assert( sizeof(T) == 4, "AVX1 only supports 32-bit integers" );
#endif

	typedef __m256i vec_type;
	typedef T bitmask_type;
	static const int value_count = 32;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm256_set1_epi8( static_cast<char>( v ) ); }

	static PAL_INLINE vec_type init( bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10, bool b11, bool b12, bool b13, bool b14, bool b15, bool b16, bool b17, bool b18, bool b19, bool b20, bool b21, bool b22, bool b23, bool b24, bool b25, bool b26, bool b27, bool b28, bool b29, bool b30, bool b31 )
	{
		return _mm256_set_epi8(
			b31?-1:0, b30?-1:0, b29?-1:0, b28?-1:0,
			b27?-1:0, b26?-1:0, b25?-1:0, b24?-1:0,
			b23?-1:0, b22?-1:0, b21?-1:0, b20?-1:0,
			b19?-1:0, b18?-1:0, b17?-1:0, b16?-1:0,
			b15?-1:0, b14?-1:0, b13?-1:0, b12?-1:0,
			b11?-1:0, b10?-1:0, b9?-1:0, b8?-1:0,
			b7?-1:0, b6?-1:0, b5?-1:0, b4?-1:0,
			b3?-1:0, b2?-1:0, b1?-1:0, b0?-1:0 );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v32qi)v)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_256 bitmask_type vals[value_count];
		_mm256_store_si256( reinterpret_cast<__m256i *>( vals ), v );
		return vals[i];
#endif
	}
};

template <typename T> struct mask256_traits<2,T>
{
	static_assert( std::is_integral<T>::value, "Expecting integer type" );
#ifndef PAL_ENABLE_AVX2
	static_assert( sizeof(T) == 4, "AVX1 only supports 32-bit integers" );
#endif
	typedef __m256i vec_type;
	typedef T bitmask_type;
	static const int value_count = 16;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm256_set1_epi16( static_cast<short>( v ) ); }

	static PAL_INLINE vec_type init( bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8, bool b9, bool b10, bool b11, bool b12, bool b13, bool b14, bool b15 )
	{
		return _mm256_set_epi16(
			b15?-1:0, b14?-1:0, b13?-1:0, b12?-1:0,
			b11?-1:0, b10?-1:0, b9?-1:0, b8?-1:0,
			b7?-1:0, b6?-1:0, b5?-1:0, b4?-1:0,
			b3?-1:0, b2?-1:0, b1?-1:0, b0?-1:0 );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v16hi)v)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_256 bitmask_type vals[value_count];
		_mm256_store_si256( reinterpret_cast<__m256i *>( vals ), v );
		return vals[i];
#endif
	}
};

template <typename T> struct mask256_traits<4,T>
{
	static_assert( std::is_integral<T>::value, "Expecting integer type" );
	typedef __m256i vec_type;
	typedef T bitmask_type;
	static const int value_count = 8;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm256_set1_epi32( static_cast<int>( v ) ); }

	static PAL_INLINE vec_type init( bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7 )
	{
		return _mm256_set_epi32(
			b7?-1:0, b6?-1:0, b5?-1:0, b4?-1:0,
			b3?-1:0, b2?-1:0, b1?-1:0, b0?-1:0 );
	}

	static PAL_INLINE bool access_bool( vec_type v, int i )
	{
		return ( ( access_value( v, i ) >> (sizeof(bitmask_type)*8 - 1) ) & 1 ) != 0;
	}

	static PAL_INLINE bitmask_type access_value( vec_type v, int i )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v8si)v)[i];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_256 bitmask_type vals[value_count];
		_mm256_store_si256( reinterpret_cast<__m256i *>( vals ), v );
		return vals[i];
#endif
	}
};

template <> struct mask256_traits<4,float>
{
	typedef __m256 vec_type;
	typedef uint32_t bitmask_type;

	static const int value_count = 8;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm256_castsi256_ps( _mm256_set1_epi32( static_cast<int>( v ) ) ); }

	static PAL_INLINE vec_type init( bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7 )
	{
		return _mm256_castsi256_ps(
			_mm256_set_epi32(
				b7?-1:0, b6?-1:0, b5?-1:0, b4?-1:0,
				b3?-1:0, b2?-1:0, b1?-1:0, b0?-1:0 ) );
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
			bitmask_type i;
		} c;
		c.f = ((__v8sf)v)[i];
		return c.i;
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256 vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_256 bitmask_type vals[value_count];
		_mm256_store_ps( reinterpret_cast<float *>( vals ), v );
		return vals[i];
#endif
	}
};

template <> struct mask256_traits<8,double>
{
	typedef __m256d vec_type;
	typedef uint64_t bitmask_type;

	static const int value_count = 4;

	template <typename V>
	static PAL_INLINE vec_type splat( V v )
	{ return _mm256_castsi256_pd( _mm256_set1_epi64x( static_cast<long long>( v ) ) ); }

	static PAL_INLINE vec_type init( bool b0, bool b1, bool b2, bool b3 )
	{
		return _mm256_castsi256_pd(
			_mm256_set_epi64x(
				b3?-1:0, b2?-1:0, b1?-1:0, b0?-1:0 ) );
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
		c.f = ((__v4df)v)[i];
		return c.i;
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256d vec;
			bitmask_type vals[value_count];
		} x;
		x.vec = v;
		return x.vals[i];
#else
		PAL_ALIGN_256 bitmask_type vals[value_count];
		_mm256_store_pd( reinterpret_cast<double *>( vals ), v );
		return vals[i];
#endif
	}
};

} // namespace detail

} // namespace pal

#endif // AVX

#endif // _PAL_X86_DETAIL_MASK256_H_
