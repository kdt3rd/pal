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
# error "Never use <pal/x86/simd_load_store.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_LOAD_STORE_H_
# define _PAL_X86_LOAD_STORE_H_ 1

namespace PAL_NAMESPACE
{

/// @brief Declare a barrier to prevent optimization across a value
///
/// This can be useful to prevent the optimizer from removing a trick or re-ordering
template <typename T>
PAL_INLINE void barrier( T &v )
{
#ifdef _MSC_VER
	_ReadWriteBarrier();
#else
	// TODO: Add other compiler support, move to common/compiler.h?
	__asm__( "" : "+x"(v) );
#endif
}

/// @brief triggers a prefetch
PAL_INLINE void prefetch_read( const void *buffer )
{
	_mm_prefetch( buffer, _MM_HINT_T0 );
}

/// @brief triggers a prefetch
PAL_INLINE void prefetch_readwrite( const void *buffer )
{
	// should this be _MM_HINT_ET0? Have to have prefetchw support
	_mm_prefetch( buffer, _MM_HINT_T0 );
}

/// @brief load from any address
///
/// On modern CPU, this should be the same speed as @sa load_aligned, but for legacy CPU, the implementor may wish to take care
template <typename itype>
PAL_INLINE ivec128<itype> load( const itype *in )
{
	return ivec128<itype>( _mm_castps_epi32( _mm_loadu_ps( in ) ) );
}

/// @brief load from a known aligned address
template <typename itype>
PAL_INLINE ivec128<itype> load_aligned( const itype *in )
{
	return ivec128<itype>( _mm_castps_epi32( _mm_load_ps( in ) ) );
}

////////////////////////////////////////

/// @brief load a single value
PAL_INLINE fvec4 load1f( const float *in )
{
	return fvec4( _mm_load_ss( in ) );
}

/// @brief load 2 values
PAL_INLINE fvec4 load2f( const float *in )
{
	// load as a double and recast it
	return fvec4( _mm_castpd_ps( _mm_load_sd( reinterpret_cast<const double *>(in) ) ) );
//	return fvec4( _mm_setr_ps( in[0], in[1], 0.F, 0.F ) );
}

/// @brief load 3 values
PAL_INLINE fvec4 load3f( const float *in )
{
	// is there a faster way to do this?
	return fvec4( _mm_setr_ps( in[0], in[1], in[2], 0.F ) );
}

/// @brief load from any address
///
/// On modern CPU, this should be the same speed as @sa load4f_aligned, but for legacy CPU, the implementor may wish to take care
PAL_INLINE fvec4 load4f( const float *in )
{
	return fvec4( _mm_loadu_ps( in ) );
}

/// @brief load from a known aligned address
PAL_INLINE fvec4 load4f_aligned( const float *in )
{
	return fvec4( _mm_load_ps( in ) );
}

#ifdef PAL_HAS_FVEC8
/// @brief load from any address
///
/// On modern CPU, this should be the same speed as @sa load4f_aligned, but for legacy CPU, the implementor may wish to take care
PAL_INLINE fvec8 load8f( const float *in )
{
	return fvec8( _mm256_loadu_ps( in ) );
}

/// @brief load from a known aligned address
PAL_INLINE fvec8 load8f_aligned( const float *in )
{
	return fvec8( _mm256_load_ps( in ) );
}
#endif // PAL_HAS_FVEC8

////////////////////////////////////////
////////////////////////////////////////

PAL_INLINE void store1( float *out, fvec4 v )
{
	_mm_store_ss( out, v );
}

PAL_INLINE void store2( float *out, fvec4 v )
{
	// recast as a double and store
	_mm_store_sd( reinterpret_cast<double *>(out), _mm_castps_pd( v ) );
//	out[0] = v[0];
//	out[1] = v[1];
//	_mm_store_ss( out, v );
//	_mm_store_ss( out + 1, swizzle<1,1,1,1>( v ) );
}

PAL_INLINE void store3( float *out, fvec4 v )
{
	out[0] = v[0];
	out[1] = v[1];
	out[2] = v[2];
}

PAL_INLINE void store( float *out, fvec4 v )
{
	_mm_storeu_ps( out, v );
}

/// @brief this is probably preferred if the implementation
/// knows the data is aligned
PAL_INLINE void store_aligned( float *out, fvec4 v )
{
	_mm_store_ps( out, v );
}

/// @brief writes values to memory, skipping data cache.
///
/// if you will be reading the data back in soon, or the amount of
/// data you will be writing is less than the various L1/L2 cache
/// sizes, this is probably not beneficial.
PAL_INLINE void stream_aligned( float *out, fvec4 v )
{
	_mm_stream_ps( out, v );
}

/// @brief store an integer type
template <typename itype>
PAL_INLINE void store( itype *out, ivec128<itype> v )
{
	store( reinterpret_cast<float *>( out ), v.as_float() );
}

/// @brief this is probably preferred if the implementation
/// knows the data is aligned
template <typename itype>
PAL_INLINE void store_aligned( itype *out, ivec128<itype> v )
{
	store_aligned( reinterpret_cast<float *>( out ), v.as_float() );
}

#ifdef PAL_HAS_FVEC8
PAL_INLINE void store( float *out, fvec8 v )
{
	_mm256_storeu_ps( out, v );
}

PAL_INLINE void store_aligned( float *out, fvec8 v )
{
	_mm256_store_ps( out, v );
}

PAL_INLINE void stream_aligned( float *out, fvec8 v )
{
	_mm256_stream_ps( out, v );
}
#endif

#ifdef PAL_HAS_FVEC16
PAL_INLINE void store( float *out, fvec16 v )
{
	_mm512_storeu_ps( out, v );
}

PAL_INLINE void store_aligned( float *out, fvec16 v )
{
	_mm512_store_ps( out, v );
}

PAL_INLINE void stream_aligned( float *out, fvec16 v )
{
	_mm512_stream_ps( out, v );
}
#endif

} // namespace pal

#endif // _PAL_X86_LOAD_STORE_H_
