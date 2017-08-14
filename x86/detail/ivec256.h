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
# error "Never use detail/ivec256.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_DETAIL_IVEC256_H_
# define _PAL_X86_DETAIL_IVEC256_H_ 1

namespace PAL_NAMESPACE
{

namespace detail
{

template <size_t T> struct ivec256_traits {};

template <> struct ivec256_traits<1>
{
	typedef char itype;
	static const int value_count = 32;
	static PAL_INLINE __m256i splat( itype v ) { return _mm256_set1_epi8( v ); }
	static PAL_INLINE __m256i init( itype a0, itype a1, itype a2, itype a3,
									itype a4, itype a5, itype a6, itype a7,
									itype a8, itype a9, itype a10, itype a11,
									itype a12, itype a13, itype a14, itype a15,
									itype a16, itype a17, itype a18, itype a19,
									itype a20, itype a21, itype a22, itype a23,
									itype a24, itype a25, itype a26, itype a27,
									itype a28, itype a29, itype a30, itype a31 )
	{
		return _mm256_set_epi8(
			a31, a30, a29, a28, a27, a26, a25, a24, a23, a22, a21, a20, a19, a18, a17, a16,
			a15, a14, a13, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, a0 );
	}
	static PAL_INLINE __m256i addu( __m256i a, __m256i b ) { return _mm256_add_epi8( a, b ); }
	static PAL_INLINE __m256i adds( __m256i a, __m256i b ) { return _mm256_adds_epi8( a, b ); }
	static PAL_INLINE __m256i subu( __m256i a, __m256i b ) { return _mm256_sub_epi8( a, b ); }
	static PAL_INLINE __m256i subs( __m256i a, __m256i b ) { return _mm256_subs_epi8( a, b ); }
	static PAL_INLINE itype access( __m256i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v32qi)a)[idx];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_256 itype vals[value_count];
		_mm256_store_ps( reinterpret_cast<__m256i *>( vals ), a );
		return vals[idx];
#endif
	}
};

template <> struct ivec256_traits<2>
{
	typedef short itype;
	static const int value_count = 16;
	static PAL_INLINE __m256i splat( itype v ) { return _mm256_set1_epi16( v ); }
	static PAL_INLINE __m256i init( itype a0, itype a1, itype a2, itype a3,
									itype a4, itype a5, itype a6, itype a7,
									itype a8, itype a9, itype a10, itype a11,
									itype a12, itype a13, itype a14, itype a15 )
	{
		return _mm256_set_epi16( a15, a14, a13, a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1, a0 );
	}
	static PAL_INLINE __m256i addu( __m256i a, __m256i b ) { return _mm256_add_epi16( a, b ); }
	static PAL_INLINE __m256i adds( __m256i a, __m256i b ) { return _mm256_adds_epi16( a, b ); }
	static PAL_INLINE __m256i subu( __m256i a, __m256i b ) { return _mm256_sub_epi16( a, b ); }
	static PAL_INLINE __m256i subs( __m256i a, __m256i b ) { return _mm256_subs_epi16( a, b ); }
	static PAL_INLINE itype access( __m256i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return ((__v16hi)a)[idx];
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_256 itype vals[value_count];
		_mm256_store_ps( reinterpret_cast<__m256i *>( vals ), a );
		return vals[idx];
#endif
	}
};

template <> struct ivec256_traits<4>
{
	typedef int itype;
	static const int value_count = 8;
	static PAL_INLINE __m256i splat( itype v ) { return _mm256_set1_epi32( v ); }
	static PAL_INLINE __m256i init( itype a0, itype a1, itype a2, itype a3,
									itype a4, itype a5, itype a6, itype a7 )
	{
		return _mm256_set_epi32( a7, a6, a5, a4, a3, a2, a1, a0 );
	}
	static PAL_INLINE __m256i addu( __m256i a, __m256i b ) { return _mm256_add_epi32( a, b ); }
	// huh, gcc doesn't seem to provide a signed 32-bit add
	static PAL_INLINE __m256i adds( __m256i a, __m256i b ) { return _mm256_add_epi32( a, b ); }
	static PAL_INLINE __m256i subu( __m256i a, __m256i b ) { return _mm256_sub_epi32( a, b ); }
	// huh, gcc doesn't seem to provide a signed 32-bit subtract
	static PAL_INLINE __m256i subs( __m256i a, __m256i b ) { return _mm256_sub_epi32( a, b ); }
	static PAL_INLINE itype access( __m256i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return static_cast<itype>( ((__v8si)a)[idx] );
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_256 itype vals[value_count];
		_mm256_store_ps( reinterpret_cast<__m256i *>( vals ), a );
		return vals[idx];
#endif
	}
};

template <> struct ivec256_traits<8>
{
	typedef long long itype;
	static const int value_count = 4;
	static PAL_INLINE __m256i splat( itype v ) { return _mm256_set1_epi64x( v ); }
	static PAL_INLINE __m256i init( itype a0, itype a1, itype a2, itype a3 )
	{
		return _mm256_set_epi64x( a3, a2, a1, a0 );
	}
	static PAL_INLINE __m256i addu( __m256i a, __m256i b ) { return _mm256_add_epi64( a, b ); }
	// huh, gcc doesn't seem to provide a signed 64-bit add
	static PAL_INLINE __m256i adds( __m256i a, __m256i b ) { return _mm256_add_epi64( a, b ); }
	static PAL_INLINE __m256i subu( __m256i a, __m256i b ) { return _mm256_sub_epi64( a, b ); }
	// huh, gcc doesn't seem to provide a signed 64-bit subtract
	static PAL_INLINE __m256i subs( __m256i a, __m256i b ) { return _mm256_sub_epi64( a, b ); }
	static PAL_INLINE itype access( __m256i a, int idx )
	{
#ifdef PAL_HAS_DIRECT_VEC_ACCESS
		return static_cast<itype>( ((__v4di)a)[idx] );
#elif defined(PAL_HAS_UNION_VEC_ACCESS)
		union 
		{
			__m256i vec;
			itype vals[value_count];
		};
		vec = a;
		return vals[idx];
#else
		PAL_ALIGN_256 itype vals[value_count];
		_mm256_store_ps( reinterpret_cast<__m256i *>( vals ), a );
		return vals[idx];
#endif
	}
};

} // namespace detail

} // namespace pal

#endif // _PAL_X86_DETAIL_IVEC256_H_
