//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/ivec4_math.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_LVEC4_MATH_H_
# define _PAL_X86_SSE_LVEC4_MATH_H_ 1

namespace PAL_NAMESPACE
{

/// @brief return the max of the 2 numbers
PAL_INLINE lvec4 max( lvec4 a, lvec4 b )
{
#ifdef PAL_ENABLE_SSE4_1
	return lvec4( _mm_max_epi32( a, b ) );
#else
	typedef typename lvec4::mask_type mvec4;
	mvec4 t = a > b;
	return t.blend( b, a );
#endif
}

/// @brief return the min of the 2 numbers
PAL_INLINE lvec4 min( lvec4 a, lvec4 b )
{
#ifdef PAL_ENABLE_SSE4_1
	return lvec4( _mm_min_epi32( a, b ) );
#else
	typedef typename lvec4::mask_type mvec4;
	mvec4 t = a > b;
	return t.blend( a, b );
#endif
}

} // namespace pal

#endif // _PAL_X86_LVEC4_MATH_H_
