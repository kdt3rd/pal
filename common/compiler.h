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
# error "Never use <pal/common/compiler.h> directly. Include <pal.h> instead."
#endif


#ifndef _PAL_COMMON_COMPILER_H_
# define _PAL_COMMON_COMPILER_H_ 1

#if defined(__GNUC__) || defined(__clang__)
# define PAL_INLINE inline __attribute__((always_inline, artificial))
#else
# if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#  define PAL_INLINE __forceinline
# else
#  warning "Unhandled compiler in simd/common/compiler.h"
#  define PAL_INLINE inline
# endif
#endif

#if defined(_MSC_VER)
# define PAL_ALIGN_128 __declspec(align(16))
# define PAL_ALIGN_256 __declspec(align(32))
# define PAL_ALIGN_512 __declspec(align(64))
# define PAL_RESTRICT_KEYWORD __restrict
#else
# define PAL_ALIGN_128 __attribute__ ((aligned (16)))
# define PAL_ALIGN_256 __attribute__ ((aligned (32)))
# define PAL_ALIGN_512 __attribute__ ((aligned (64)))
# define PAL_RESTRICT_KEYWORD __restrict__
#endif

#define PAL_RESTRICT_PTR(T) T * PAL_RESTRICT_KEYWORD

// can values be accessed 'directly' from the vector?
#if defined(__clang__) || ( defined(__GNUC__) && (__GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ > 6 ) ) )
# define PAL_HAS_DIRECT_VEC_ACCESS 1
#else
# define PAL_HAS_UNION_VEC_ACCESS 1
#endif

// gcc, clang, icc should set one of these indicating we
// are compiling for an x86-based platform
// but also allow for "x86 emulation", as provided by IBM under PPC
#if defined(__x86_64__) || defined(__x86_64) || defined(__x86__) || defined(PAL_X86_EMULATION)

// is ifdef this accurate, in that linux (gxx) has x86intrin.h but
// under os/x, clang doesn't provide x86intrin.h. Does clang under
// linux also not provide it?
# if defined(LINUX) || defined(__linux__)
#  include <x86intrin.h>
# else
#  include <xmmintrin.h>
#  include <immintrin.h>
# endif

# define PAL_ENABLE_X86_SIMD 1

# if defined(__MMX__)
#  define PAL_ENABLE_MMX 1
// do we need to bother with this anymore?
//#  define PAL_ENABLE_64BIT_X86_VALUES
# endif
# if defined(__SSE__)
#  define PAL_ENABLE_SSE 1
// technically this could be here, but it's basically only floats
// and really annoying in a modern world - is anyone going to be
// compiling for < SSE2????
//#  define PAL_ENABLE_128BIT_X86_VALUES 1
# endif
# if defined(__SSE2__)
#  define PAL_ENABLE_SSE2 1
#  define PAL_ENABLE_128BIT_X86_VALUES 1
# endif
# if defined(__SSE3__)
#  define PAL_ENABLE_SSE3 1
# endif
# if defined(__SSSE3__)
#  define PAL_ENABLE_SSSE3 1
# endif
# if defined(__SSE4A__)
#  define PAL_ENABLE_SSE4_A 1
# endif
# if defined(__SSE4_1__)
// this is a very important extension. It enables round/ceil/floor,
// blend, dot product, etc.
#  define PAL_ENABLE_SSE4_1 1
# endif
# if defined(__SSE4_2__)
// mostly string / stream processing, do we expose any of this?
// but maybe the crc32 functions????
#  define PAL_ENABLE_SSE4_2 1
# endif
# if defined(__AVX__)
#  define PAL_ENABLE_AVX 1
#  define PAL_ENABLE_256BIT_X86_VALUES 1
# endif
# if defined(__AVX2__)
// YUMMY gather instructions...
#  define PAL_ENABLE_AVX2 1
# endif
# if defined(__AVX512F__)
#  define PAL_ENABLE_AVX_512 1
#  define PAL_ENABLE_512BIT_X86_VALUES 1
# endif
// TODO: add checks for various extnsions of AVX512

// The following are useful processor extensions that
// may be enabled by compiling for one of the above

//#if defined (__AES__)
// add this if we ever expose any aes routines wmmintrin.h
//#endif
# if defined(__FMA__)
// ternary ops!!!
#  define PAL_ENABLE_FMA_EXT 1
# endif
# if defined(__FMA4__)
#  define PAL_ENABLE_FMA4_EXT 1
# endif
# if defined(__F16C__)
#  define PAL_ENABLE_HALF_FLOAT_EXT 1
# endif

#endif // X86 platform

// check windows / msvc
#if defined(_M_X64) || ( defined(_M_IX86_FP) && (_M_IX86_FP >= 1) )
// match defines for windows to intel, gcc, clang, etc
# define FP_ILOGB0 _FP_ILOGB0
# define FP_ILOGBNAN _FP_ILOGBNAN

// doesn't seem to be a good way to check which are enabled?
# define PAL_ENABLE_SSE 1
# define PAL_ENABLE_SSE2 1
# define PAL_ENABLE_SSE3 1
// todo: what about the other sse levels / avx / extensions???

#endif

#endif // _PAL_COMMON_COMPILER_H_
