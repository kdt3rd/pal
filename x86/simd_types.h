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
# error "Never use <pal/x86/simd_types.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_TYPES_H_
# define _PAL_X86_TYPES_H_ 1

# ifdef PAL_ENABLE_X86_SIMD
#  ifdef PAL_ENABLE_128BIT_X86_VALUES
#   include "mask128_t.h"
#   include "ivec128_t.h"
#   include "fvec128_t.h"
#   include "dvec128_t.h"
#  else
#   error "X86 is apparent target, but missing minimum SSE2 128-bit types"
#  endif // PAL_ENABLE_128BIT_X86_VALUES

#  ifdef PAL_ENABLE_256BIT_X86_VALUES
#   include "mask256_t.h"
#   include "ivec256_t.h"
#   include "fvec256_t.h"
#   include "dvec256_t.h"
#  endif // PAL_ENABLE_256BIT_X86_VALUES

#  ifdef PAL_ENABLE_512BIT_X86_VALUES
// TODO: consider mask types propagating into the above
//#   include "mask512_t.h"
//#   include "ivec512_t.h"
//#   include "fvec512_t.h"
#  endif // PAL_ENABLE_512BIT_X86_VALUES

# endif // PAL_ENABLE_X86_SIMD

#endif // _PAL_X86_TYPES_H_
