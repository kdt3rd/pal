//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/sse_fvec4_operators.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_FVEC4_OPERATORS_H_
# define _PAL_X86_SSE_FVEC4_OPERATORS_H_ 1

namespace PAL_NAMESPACE
{

////////////////////////////////////////
// Unary operators

PAL_INLINE fvec4 operator+( fvec4 a ) { return a; }
PAL_INLINE fvec4 operator-( fvec4 a )
{
	return fvec4( _mm_xor_ps( a, int_constants<fvec4>::sign_bitmask() ) );
}

////////////////////////////////////////
// Binary operators

PAL_INLINE fvec4 operator+( fvec4 a, fvec4 b )
{
	a += b; return a;
}
PAL_INLINE fvec4 operator+( fvec4 a, float b )
{
	a += b; return a;
}
PAL_INLINE fvec4 operator+( float a, fvec4 b )
{
	b += a; return b;
}

PAL_INLINE fvec4 operator-( fvec4 a, fvec4 b )
{
	a -= b; return a;
}
PAL_INLINE fvec4 operator-( fvec4 a, float b )
{
	a -= b; return a;
}
PAL_INLINE fvec4 operator-( float a, fvec4 b )
{
	fvec4 r( a );
	r -= b; return r;
}

PAL_INLINE fvec4 operator*( fvec4 a, fvec4 b )
{
	a *= b; return a;
}
PAL_INLINE fvec4 operator*( fvec4 a, float b )
{
	a *= b; return a;
}
PAL_INLINE fvec4 operator*( float a, fvec4 b )
{
	b *= a; return b;
}

PAL_INLINE fvec4 operator/( fvec4 a, fvec4 b )
{
	a /= b; return a;
}
PAL_INLINE fvec4 operator/( fvec4 a, float b )
{
	a /= b; return a;
}
PAL_INLINE fvec4 operator/( float a, fvec4 b )
{
	fvec4 r( a );
	r /= b; return r;
}

////////////////////////////////////////
// bit-wise operators

PAL_INLINE fvec4 operator&( fvec4 a, fvec4 b )
{
	return fvec4( _mm_and_ps( a, b ) );
}
PAL_INLINE fvec4 operator&( fvec4::mask_type a, fvec4 b )
{
	return fvec4( _mm_and_ps( a, b ) );
}
PAL_INLINE fvec4 operator&( fvec4 a, fvec4::mask_type b )
{
	return fvec4( _mm_and_ps( a, b ) );
}

PAL_INLINE fvec4 operator|( fvec4 a, fvec4 b )
{
	return fvec4( _mm_or_ps( a, b ) );
}
PAL_INLINE fvec4 operator|( fvec4::mask_type a, fvec4 b )
{
	return fvec4( _mm_or_ps( a, b ) );
}
PAL_INLINE fvec4 operator|( fvec4 a, fvec4::mask_type b )
{
	return fvec4( _mm_or_ps( a, b ) );
}

PAL_INLINE fvec4 operator^( fvec4 a, fvec4 b )
{
	return fvec4( _mm_xor_ps( a, b ) );
}
PAL_INLINE fvec4 operator^( fvec4::mask_type a, fvec4 b )
{
	return fvec4( _mm_xor_ps( a, b ) );
}
PAL_INLINE fvec4 operator^( fvec4 a, fvec4::mask_type b )
{
	return fvec4( _mm_xor_ps( a, b ) );
}

////////////////////////////////////////
// Comparison operators

PAL_INLINE fvec4::mask_type operator==( fvec4 a, fvec4 b )
{
	return fvec4::mask_type( _mm_cmpeq_ps( a, b ) );
}
PAL_INLINE fvec4::mask_type operator!=( fvec4 a, fvec4 b )
{
	return fvec4::mask_type( _mm_cmpneq_ps( a, b ) );
}
PAL_INLINE fvec4::mask_type operator<( fvec4 a, fvec4 b )
{
	return fvec4::mask_type( _mm_cmplt_ps( a, b ) );
}
PAL_INLINE fvec4::mask_type operator<=( fvec4 a, fvec4 b )
{
	return fvec4::mask_type( _mm_cmple_ps( a, b ) );
}
PAL_INLINE fvec4::mask_type operator>( fvec4 a, fvec4 b )
{
	return fvec4::mask_type( _mm_cmpgt_ps( a, b ) );
}
PAL_INLINE fvec4::mask_type operator>=( fvec4 a, fvec4 b )
{
	return fvec4::mask_type( _mm_cmpge_ps( a, b ) );
}

} // namespace pal

#endif // _PAL_X86_FVEC4_OPERATORS_H_
