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
# error "Never use <pal/x86/fvec8_operators.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SSE_FVEC8_OPERATORS_H_
# define _PAL_X86_SSE_FVEC8_OPERATORS_H_ 1

# ifdef PAL_HAS_FVEC8

namespace PAL_NAMESPACE
{

////////////////////////////////////////
// Unary operators

PAL_INLINE fvec8 operator+( fvec8 a ) { return a; }
PAL_INLINE fvec8 operator-( fvec8 a )
{
	static const lvec8 signMask = lvec8::splat( vector_limits<fvec8>::sign_mask );
	return fvec8( _mm256_xor_ps( a, signMask.as_float() ) );
}

////////////////////////////////////////
// Binary operators

PAL_INLINE fvec8 operator+( fvec8 a, fvec8 b )
{
	a += b; return a;
}
PAL_INLINE fvec8 operator+( fvec8 a, float b )
{
	a += b; return a;
}
PAL_INLINE fvec8 operator+( float a, fvec8 b )
{
	b += a; return b;
}

PAL_INLINE fvec8 operator-( fvec8 a, fvec8 b )
{
	a -= b; return a;
}
PAL_INLINE fvec8 operator-( fvec8 a, float b )
{
	a -= b; return a;
}
PAL_INLINE fvec8 operator-( float a, fvec8 b )
{
	fvec8 r( a );
	r -= b; return r;
}

PAL_INLINE fvec8 operator*( fvec8 a, fvec8 b )
{
	a *= b; return a;
}
PAL_INLINE fvec8 operator*( fvec8 a, float b )
{
	a *= b; return a;
}
PAL_INLINE fvec8 operator*( float a, fvec8 b )
{
	b *= a; return b;
}

PAL_INLINE fvec8 operator/( fvec8 a, fvec8 b )
{
	a /= b; return a;
}
PAL_INLINE fvec8 operator/( fvec8 a, float b )
{
	a /= b; return a;
}
PAL_INLINE fvec8 operator/( float a, fvec8 b )
{
	fvec8 r( a );
	r /= b; return r;
}

////////////////////////////////////////
// bit-wise operators

PAL_INLINE fvec8 operator&( fvec8 a, fvec8 b )
{
	return fvec8( _mm256_and_ps( a, b ) );
}

PAL_INLINE fvec8 operator|( fvec8 a, fvec8 b )
{
	return fvec8( _mm256_or_ps( a, b ) );
}

PAL_INLINE fvec8 operator^( fvec8 a, fvec8 b )
{
	return fvec8( _mm256_xor_ps( a, b ) );
}

////////////////////////////////////////
// Comparison operators

PAL_INLINE fvec8::mask_type operator==( fvec8 a, fvec8 b )
{
	return fvec8::mask_type( _mm256_cmp_ps( a, b, _CMP_EQ_OQ ) );
}
PAL_INLINE fvec8::mask_type operator!=( fvec8 a, fvec8 b )
{
	return fvec8::mask_type( _mm256_cmp_ps( a, b, _CMP_NEQ_OQ ) );
}
PAL_INLINE fvec8::mask_type operator<( fvec8 a, fvec8 b )
{
	return fvec8::mask_type( _mm256_cmp_ps( a, b, _CMP_LT_OQ ) );
}
PAL_INLINE fvec8::mask_type operator<=( fvec8 a, fvec8 b )
{
	return fvec8::mask_type( _mm256_cmp_ps( a, b, _CMP_LE_OQ ) );
}
PAL_INLINE fvec8::mask_type operator>( fvec8 a, fvec8 b )
{
	return fvec8::mask_type( _mm256_cmp_ps( a, b, _CMP_GT_OQ ) );
}
PAL_INLINE fvec8::mask_type operator>=( fvec8 a, fvec8 b )
{
	return fvec8::mask_type( _mm256_cmp_ps( a, b, _CMP_GE_OQ ) );
}

} // namespace pal
# endif // PAL_HAS_FVEC8

#endif // _PAL_X86_FVEC8_OPERATORS_H_
