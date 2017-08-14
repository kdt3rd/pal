//
// Copyright (c) 2016-2017 Kimball Thurston
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
# error "Never use pal/buffer_process.h directly. Include <pal.h> instead."
#endif

#ifndef _PAL_BUFFER_PROCESS_H_
# define _PAL_BUFFER_PROCESS_H_ 1

namespace PAL_NAMESPACE
{

template <typename F>
inline void
process_inplace( PAL_RESTRICT_PTR(float) buffer, size_t nLeft, F func )
{
#if defined(PAL_HAS_FVEC8) || defined(PAL_HAS_FVEC4)

#if defined(PAL_HAS_FVEC8)
	static const int kAlignMask = 0x1F;
	static const int kAlignNumber = 8;
#elif defined(PAL_HAS_FVEC4)
	static const int kAlignMask = 0xF;
	static const int kAlignNumber = 4;
#endif
	// prefetches L1 cache line size (64 bytes currently on x86)
	// while we get set up
	prefetch_readwrite( buffer );
	// we can do this all unaligned on modern CPUs, is the
	// alignment check and scalar loop worth it?

	// So, if we want any of this to be worth it on modern
	// architectures, we want a memory alignment to be such that we do
	// not cross cache line boundaries on load / store. Otherwise we
	// can just use the unaligned accessors and not worry about it.
	int nToAlign = reinterpret_cast<intptr_t>( buffer ) & kAlignMask;

	// if it's 4-byte aligned we can perform loop peeling
	// and get to cache aligned
	if ( ( nToAlign & 0x3 ) == 0 )
	{
		nToAlign = ( nToAlign >> 2 );
		if ( nToAlign > 0 )
		{
			while ( nLeft > 0 && nToAlign < kAlignNumber )
			{
				*buffer = func( *buffer );
				++buffer;
				++nToAlign;
				--nLeft;
			}
		}

		while ( nLeft >= 16 )
		{
			// get the next loop's worth (the next cache line)
			prefetch_readwrite( buffer + 16 );

#if defined(PAL_HAS_FVEC8)
			store_aligned( buffer, func( load8f_aligned( buffer ) ) );
			buffer += 8;
			store_aligned( buffer, func( load8f_aligned( buffer ) ) );
			buffer += 8;
#elif defined(PAL_HAS_FVEC4)
			store_aligned( buffer, func( load4f_aligned( buffer ) ) );
			buffer += 4;
			store_aligned( buffer, func( load4f_aligned( buffer ) ) );
			buffer += 4;
			store_aligned( buffer, func( load4f_aligned( buffer ) ) );
			buffer += 4;
			store_aligned( buffer, func( load4f_aligned( buffer ) ) );
			buffer += 4;
#endif
			nLeft -= 16;
		}
		switch ( nLeft )
		{
			case 15:
			case 14:
			case 13:
			case 12:
#if defined(PAL_HAS_FVEC8)
				store_aligned( buffer, func( load8f_aligned( buffer ) ) );
				buffer += 8;
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
#elif defined(PAL_HAS_FVEC4)
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
#endif
				nLeft -= 12;
				break;
			case 11:
			case 10:
			case 9:
			case 8:
#if defined(PAL_HAS_FVEC8)
				store_aligned( buffer, func( load8f_aligned( buffer ) ) );
				buffer += 8;
#elif defined(PAL_HAS_FVEC4)
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
#endif
				nLeft -= 8;
				break;
			case 7:
			case 6:
			case 5:
			case 4:
#if defined(PAL_HAS_FVEC4)
				store_aligned( buffer, func( load4f_aligned( buffer ) ) );
				buffer += 4;
#endif
				nLeft -= 4;
				break;
			default:
				break;
		}
		switch ( nLeft )
		{
			case 3:
				*buffer = func( *buffer );
				++buffer;
				// FALLTHROUGH
			case 2:
				*buffer = func( *buffer );
				++buffer;
				// FALLTHROUGH
			case 1:
				*buffer = func( *buffer );
				++buffer;
				// FALLTHROUGH
			case 0:
				// NB: EXIT POINT
				return;
			default:
				// this should be an error?
				break;
		}
	}
	else
	{
		// can never be aligned, just run unaligned and leave however
		// many for the scalar loop
#if defined(PAL_HAS_FVEC8)
		while ( nLeft >= 8 )
		{
			store( buffer, func( load8f( buffer ) ) );
			buffer += 8;
			nLeft -= 8;
		}
#endif
#if defined(PAL_HAS_FVEC4)
		while ( nLeft >= 4 )
		{
			store( buffer, func( load4f( buffer ) ) );
			buffer += 4;
			nLeft -= 4;
		}
#endif
		// fall through and let scalar code take care of any
		// remaining
	}
#endif
	// Finish off any remaining scalars
	while ( nLeft > 0 )
	{
		*buffer = func( *buffer );
		++buffer;
		--nLeft;
	}
}

template <typename F>
PAL_INLINE void
process_inplace( PAL_RESTRICT_PTR(float) buffer, PAL_RESTRICT_PTR(float) end, F && func )
{
	size_t nLeft = end - buffer;
	process_inplace( buffer, nLeft, std::forward<F>( func ) );
}

template <typename F>
PAL_INLINE void
process( PAL_RESTRICT_PTR(float) out, PAL_RESTRICT_PTR(const float) in, size_t nLeft, F && func )
{
	if ( out == in )
	{
		process_inplace( out, nLeft, std::forward<F>( func ) );
		return;
	}

#if defined(PAL_HAS_FVEC8) || defined(PAL_HAS_FVEC4) || defined(PAL_HAS_FVEC2)
#if defined(PAL_HAS_FVEC8)
	static const int kAlignMask = 0x1F;
	static const int kAlignNumber = 8;
#elif defined(PAL_HAS_FVEC4)
	static const int kAlignMask = 0xF;
	static const int kAlignNumber = 4;
#elif defined(PAL_HAS_FVEC2)
	static const int kAlignMask = 0x7;
	static const int kAlignNumber = 2;
#endif
	// prefetches L1 cache line size (64 bytes currently on x86)
	// while we get set up
	prefetch_readwrite( out );
	prefetch_read( in );
	// we can do this all unaligned on modern CPUs, is the
	// alignment check and scalar loop worth it?
	int nToAlignO = reinterpret_cast<intptr_t>( out ) & kAlignMask;
	int nToAlignI = reinterpret_cast<intptr_t>( in ) & kAlignMask;

	// TODO: Write tests to see if all this loop peeling and alignment
	// stuff is really worth it on modern architectures
	//
	// if it's 4-byte aligned we can do this aligned
	if ( nToAlignO == nToAlignI && ( nToAlignO & 0x3 ) == 0 )
	{
		nToAlignO = ( nToAlignO >> 2 );
		if ( nToAlignO > 0 )
		{
			while ( nLeft > 0 && nToAlignO < kAlignNumber )
			{
				*out = func( *in );
				++out; ++in;
				++nToAlignO;
				--nLeft;
			}
		}

		while ( nLeft >= 16 )
		{
			// get the next loop's worth
			prefetch_readwrite( out + 16 );
			prefetch_read( in + 16 );

#if defined(PAL_HAS_FVEC8)
			store_aligned( out, func( load8f_aligned( in ) ) );
			out += 8; in += 8;
			store_aligned( out, func( load8f_aligned( in ) ) );
			out += 8; in += 8;
#elif defined(PAL_HAS_FVEC4)
			store_aligned( out, func( load4f_aligned( in ) ) );
			out += 4; in += 4;
			store_aligned( out, func( load4f_aligned( in ) ) );
			out += 4; in += 4;
			store_aligned( out, func( load4f_aligned( in ) ) );
			out += 4; in += 4;
			store_aligned( out, func( load4f_aligned( in ) ) );
			out += 4; in += 4;
#elif defined(PAL_HAS_FVEC2)
			for ( int xx = 0; xx < 8; ++x )
			{
				store_aligned( out, func( load2f_aligned( in ) ) );
				out += 2; in += 2;
			}
#endif
			nLeft -= 16;
		}
		switch ( nLeft )
		{
			case 15:
			case 14:
			case 13:
			case 12:
#if defined(PAL_HAS_FVEC8)
				store_aligned( out, func( load8f_aligned( in ) ) );
				out += 8; in += 8;
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
#elif defined(PAL_HAS_FVEC4)
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
#elif defined(PAL_HAS_FVEC2)
				for ( int xx = 0; xx < 6; ++x )
				{
					store_aligned( out, func( load2f_aligned( in ) ) );
					out += 2; in += 2;
				}
#endif
				nLeft -= 12;
				break;
			case 11:
			case 10:
			case 9:
			case 8:
#if defined(PAL_HAS_FVEC8)
				store_aligned( out, func( load8f_aligned( in ) ) );
				out += 8; in += 8;
#elif defined(PAL_HAS_FVEC4)
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
#elif defined(PAL_HAS_FVEC2)
				for ( int xx = 0; xx < 4; ++x )
				{
					store_aligned( out, func( load2f_aligned( in ) ) );
					out += 2; in += 2;
				}
#endif
				nLeft -= 8;
				break;
			case 7:
			case 6:
			case 5:
			case 4:
#if defined(PAL_HAS_FVEC4)
				store_aligned( out, func( load4f_aligned( in ) ) );
				out += 4; in += 4;
#elif defined(PAL_HAS_FVEC2)
				for ( int xx = 0; xx < 2; ++x )
				{
					store_aligned( out, func( load2f_aligned( in ) ) );
					out += 2; in += 2;
				}
#endif
				nLeft -= 4;
				break;
			default:
				break;
		}
		switch ( nLeft )
		{
			case 3:
				*out = func( *in );
				++out; ++in;
				// FALLTHROUGH
			case 2:
				*out = func( *in );
				++out; ++in;
				// FALLTHROUGH
			case 1:
				*out = func( *in );
				++out; ++in;
				// FALLTHROUGH
			case 0:
				// NB: EXIT POINT
				return;
			default:
				break;
		}
	}
	else
	{
		// can never be aligned, just run unaligned and leave however
		// many for the scalar loop
#if defined(PAL_HAS_FVEC8)
		while ( nLeft >= 8 )
		{
			store( out, func( load8f( in ) ) );
			out += 8; in += 8;
			nLeft -= 8;
		}
#endif
#if defined(PAL_HAS_FVEC4)
		while ( nLeft >= 4 )
		{
			store( out, func( load4f( in ) ) );
			out += 4; in += 4;
			nLeft -= 4;
		}
#endif
#if defined(PAL_HAS_FVEC2)
		while ( nLeft >= 2 )
		{
			store( out, func( load2f( in ) ) );
			out += 2; in += 2;
			nLeft -= 2;
		}
#endif
		// fall through  and let scalar code take care of any
		// remaining
	}
#endif
	// Finish off any remaining scalars
	while ( nLeft > 0 )
	{
		*out = func( *in );
		++out; ++in;
		--nLeft;
	}
}

template <typename F>
PAL_INLINE void
process( PAL_RESTRICT_PTR(float) out, PAL_RESTRICT_PTR(float) end, PAL_RESTRICT_PTR(const float) in, F && func )
{
	if ( out == in )
	{
		process_inplace( out, end, std::forward<F>( func ) );
	}
	else
	{
		size_t nLeft = end - out;
		process( out, in, nLeft, std::forward<F>( func ) );
	}
}

} // namespace pal

#endif // _PAL_BUFFER_PROCESS_H_
