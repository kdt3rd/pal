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
# error "Never use <pal/x86/simd_permute.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_PERMUTE_H_
# define _PAL_X86_PERMUTE_H_ 1

namespace PAL_NAMESPACE
{

template <int a, int b, int c, int d>
PAL_INLINE fvec4 permute( fvec4 v )
{
	// this should use 1 fewer register to do it in sse2 epi32 
	return fvec4( _mm_castsi128_ps( _mm_shuffle_epi32( v.as_int(), _MM_SHUFFLE( d, c, b, a ) ) ) );
//	return fvec4( _mm_shuffle_ps( v, v, _MM_SHUFFLE( d, c, b, a ) ) );
}

template <int a, int b, int c, int d>
PAL_INLINE lvec4 permute( lvec4 v )
{
	return lvec4( _mm_shuffle_epi32( v , _MM_SHUFFLE( d, c, b, a ) ) );
}

template <int a, int b>
PAL_INLINE dvec2 permute( dvec2 v )
{
	return dvec2( _mm_shuffle_pd( v, v, (a&0x1)|((b&0x1)<<1) ) );
}

#ifdef PAL_ENABLE_SSE3
// misc specializations enabled by SSE3. There are probably more.
// TODO: are these worth the maintenance? They might be since they
// avoid the cast to int which might switch int vs. float domain
// on the processor, depending on how that works...
template <> PAL_INLINE fvec4 permute<0,0,2,2>( fvec4 v )
{
	return fvec4( _mm_moveldup_ps( v ) );
}
template <> PAL_INLINE fvec4 permute<1,1,3,3>( fvec4 v )
{
	return fvec4( _mm_movehdup_ps( v ) );
}
template <> PAL_INLINE fvec4 permute<0,1,0,1>( fvec4 v )
{
	return fvec4( _mm_castpd_ps( _mm_movedup_pd( _mm_castps_pd( v ) ) ) );
}
#endif

#ifdef PAL_ENABLE_AVX
//template <int a, int b, int c, int d, int e, int f, int g, int h>
//PAL_INLINE fvec8 permute( fvec8 v )
//{
//	__m128 lo = _mm256_castps256_ps128( v );
//	__m128 hi = _mm256_extractf128_ps( v, 1 );
//}
#endif

} // namespace pal

#endif // _PAL_X86_PERMUTE_H_
