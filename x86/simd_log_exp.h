//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/simd_log_exp.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SIMD_LOG_EXP_H_
# define _PAL_X86_SIMD_LOG_EXP_H_ 1

namespace PAL_NAMESPACE
{

////////////////////////////////////////

/// @brief computes ilogb for the provided float vector
///
/// This should match the output of the standard C library but do n
/// values at once
template <typename VT>
PAL_INLINE PAL_ENABLE_ANY_FLOAT(VT)::int_vec_type
ilogb( VT x )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;
	typedef float_extract_constants<fvec> extract;
	typedef vector_limits<fvec> limits;
	typedef vector_limits<ivec> ilimits;

	ivec e = lsr( ( x.as_int() & extract::exponent_mask() ),
				  limits::mantissa_bits );
	ivec r = e - extract::bias();
	r = ifthen( e == ivec::zero(), ivec::splat( FP_ILOGB0 ), r );
	r = ifthen( isnan( x ), ivec::splat( FP_ILOGBNAN ), r );
	r = ifthen( isinf( x ), ivec::splat( ilimits::max() ), r );
	return r;
}

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)::int_vec_type
ilogbf( VT x )
{
	// through the magic of templating, these are the same and
	// don't incur float/double conversion...
	return ilogb( x );
}

////////////////////////////////////////

/// @brief implements ldexp to load the exponent into the vector
///
/// This should match the output of the standard C library but do n
/// values at once
template <typename VT>
PAL_INLINE PAL_ENABLE_ANY_FLOAT(VT)
ldexp( VT x, typename VT::int_vec_type e )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;
	typedef typename fvec::mask_type mvec;
	typedef float_constants<fvec> constants;
	typedef float_extract_constants<fvec> extract;
	typedef vector_limits<fvec> limits;

	mvec ebits( extract::exponent_mask().as_float() );
	ivec cur_e = lsr( ( x.as_int() & ebits.as_int() ), limits::mantissa_bits );

	// per man page, same value is returned for NaN, +/- inf so make pw2 0 in that case
	e = ifthen( x == fvec::zero() || isnan( x ) || isinf( x ), ivec::zero(), e );
	cur_e += e;

	fvec repl_e = ( cur_e << limits::mantissa_bits ).as_float();
	fvec r = ebits.bit_mix( x, repl_e );

	// handle denormals?
	// denormals have a biased exponent of 0...
	//r = ifthen( den, fvec::zero(), r );

	// special conditions per man page
	r = ifthen( cur_e > extract::biased_exponent_range(),
				copysignf( constants::infinity(), x ), r );
	// per man page if pw2 is zero, return original
	return ifthen( e == ivec::zero(), x, r );
}

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
ldexpf( VT x, typename VT::int_vec_type e )
{
	// through the magic of templating, these are the same and
	// don't incur float/double conversion...
	return ldexp( x, e );
}

////////////////////////////////////////

/// @brief computes frexp ala c library
///
/// NB: unlike C library, this takes the output exponent as
/// a reference instead of a pointer, since in most cases
/// the exponent is required, and this eliminates null pointer
/// checks, etc.
template <typename VT>
inline PAL_ENABLE_ANY_FLOAT(VT)
frexp( VT x, typename VT::int_vec_type &e )
{
	typedef VT fvec;
	typedef typename VT::int_vec_type ivec;
	typedef typename ivec::mask_type mvec;
	typedef float_extract_constants<fvec> extract;
	typedef vector_limits<fvec> limits;

	// get the exponent and subtract the bias
	e = lsr( ( x.as_int() & extract::exponent_mask() ), limits::mantissa_bits );
	e -= extract::bias_m1();

	fvec ret = x & mvec::splat_mask( ~limits::exponent_mask );
	ret = ret | mvec::splat_mask( uint64_t(limits::exponent_bias-1)<<limits::mantissa_bits );

	// TODO: figure out normalized numbers...
	mvec bad = ( x == fvec::zero() ) || isnan( x ) || isinf( x );

	ret = ifthen( bad, x, ret );
	// match glibc which sets unchanged values to 0
	e = bad.notIn( e );

    return ret;
}

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
frexpf( VT x, typename VT::int_vec_type &e )
{
	// through the magic of templating, these are the same and
	// don't incur float/double conversion...
	return frexp( x, e );
}

////////////////////////////////////////

// TODO: force inline?
template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
log2f( VT d )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;
	typedef float_constants<fvec> fconst;
	typedef float_extract_constants<fvec> econst;
	typedef vector_limits<fvec> limits;

	// based on sun microsystems / freebsd implementation
	ivec id = d.as_int();
	// reduce to [sqrt(2)/2, sqrt(2)]
	id += ivec( 0x3F800000 - 0x3f3504f3 );
	ivec k = lsr( id & econst::exponent_mask(), limits::mantissa_bits ) - econst::bias();

	id = ( id & econst::mantissa_mask() ) + ivec( 0x3f3504f3 );

	fvec x = id.as_float();

	fvec f = x - fconst::one();

	const fvec ivln2hi( float(1.4428710938e+00) );
	const fvec ivln2lo( float(-1.7605285393e-04) );
	const fvec Lg1( float(0.66666662693) );
	const fvec Lg2( float(0.40000972152) );
	const fvec Lg3( float(0.28498786688) );
	const fvec Lg4( float(0.24279078841) );

	fvec s = f / ( fconst::two() + f );
	fvec z = square( s );
	fvec w = square( z );
	fvec R = fma( z, fma( w, Lg3, Lg1 ), w * fma( w, Lg4, Lg2 ) );

	fvec hfsq = fconst::one_half() * f * f;
	fvec hi = ( f - hfsq ) & fvec( uint32_t(0xFFFFF000) );

	fvec lo = fma( hfsq + R, s, f - hi - hfsq );

//	fvec ret = ( lo + hi ) * ivln2lo + lo * ivln2hi + hi * ivln2hi + fvec::convert_int( k );
	// if not careful, changing the order of ops results in
	// numerical imprecision too much and throws off the results
	fvec a = lo + hi;
	fvec ret = fma( a, ivln2lo, a * ivln2hi ) + fvec::convert_int( k );

	ret = ifthen( d < fvec::zero(), fvec( std::numeric_limits<float>::quiet_NaN() ), ret );
	ret = ifthen( d == fvec::zero(), fvec( - std::numeric_limits<float>::infinity() ), ret );
	ret = ifthen( isnan( d ) || isinf( d ), d, ret );
	return ret;
}

////////////////////////////////////////

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
logf( VT d )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;
	typedef float_constants<fvec> fconst;
	typedef float_extract_constants<fvec> econst;
	typedef vector_limits<fvec> limits;

	// based on sun microsystems / freebsd implementation
	ivec id = d.as_int();
	// reduce to [sqrt(2)/2, sqrt(2)]
	id += ivec( 0x3F800000 - 0x3f3504f3 );
	ivec k = lsr( id & econst::exponent_mask(), limits::mantissa_bits ) - econst::bias();

	id = ( id & econst::mantissa_mask() ) + ivec( 0x3f3504f3 );

	fvec x = id.as_float();

	fvec f = x - fconst::one();

	const fvec ln2hi( float(6.9313812256e-01) );
	const fvec ln2lo( float(9.0580006145e-06) );
	const fvec Lg1( float(0.66666662693) );
	const fvec Lg2( float(0.40000972152) );
	const fvec Lg3( float(0.28498786688) );
	const fvec Lg4( float(0.24279078841) );

	fvec s = f / ( fconst::two() + f );
	fvec z = square( s );
	fvec w = square( z );
	fvec R = fma( z, fma( w, Lg3, Lg1 ), w * fma( w, Lg4, Lg2 ) );

	fvec hfsq = fconst::one_half() * f * f;
	fvec dk = fvec::convert_int( k );
	// hrm, very sensitive to order of operations :(
	fvec ret = s*(hfsq+R) + dk*ln2lo - hfsq + f + dk*ln2hi;
//	fvec ret = s * ( hfsq + R ) + fms( dk, ln2lo, hfsq) + fma( dk, ln2hi, f );

	ret = ifthen( d < fvec::zero(), fvec( std::numeric_limits<float>::quiet_NaN() ), ret );
	ret = ifthen( d == fvec::zero(), fvec( - std::numeric_limits<float>::infinity() ), ret );
	ret = ifthen( isnan( d ) || isinf( d ), d, ret );
	return ret;
}

////////////////////////////////////////

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
log10f( VT d )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;
	typedef float_constants<fvec> fconst;
	typedef float_extract_constants<fvec> econst;
	typedef vector_limits<fvec> limits;

	// based on sun microsystems / freebsd implementation
	ivec id = d.as_int();
	// reduce to [sqrt(2)/2, sqrt(2)]
	id += ivec( 0x3F800000 - 0x3f3504f3 );
	ivec k = lsr( id & econst::exponent_mask(), limits::mantissa_bits ) - econst::bias();

	id = ( id & econst::mantissa_mask() ) + ivec( 0x3f3504f3 );

	fvec x = id.as_float();

	fvec f = x - fconst::one();

	const fvec ivln10hi( float(4.3432617188e-01) );
	const fvec ivln10lo( float(-3.1689971365e-05) );
	const fvec log10_2hi( float(3.0102920532e-01) );
	const fvec log10_2lo( float(7.9034151668e-07) );
	const fvec Lg1( float(0.66666662693) );
	const fvec Lg2( float(0.40000972152) );
	const fvec Lg3( float(0.28498786688) );
	const fvec Lg4( float(0.24279078841) );

	fvec s = f / ( fconst::two() + f );
	fvec z = square( s );
	fvec w = square( z );
	fvec R = fma( z, fma( w, Lg3, Lg1 ), w * fma( w, Lg4, Lg2 ) );

	fvec hfsq = fconst::one_half() * f * f;
	fvec hi = ( f - hfsq ) & fvec( uint32_t(0xFFFFF000) );

	fvec lo = fma( hfsq + R, s, f - hi - hfsq );

	fvec dk = fvec::convert_int( k );
	fvec ret = dk*log10_2lo + (lo+hi)*ivln10lo + lo*ivln10hi + hi*ivln10hi + dk*log10_2hi;

	ret = ifthen( d < fvec::zero(), fvec( std::numeric_limits<float>::quiet_NaN() ), ret );
	ret = ifthen( d == fvec::zero(), fvec( - std::numeric_limits<float>::infinity() ), ret );
	ret = ifthen( isnan( d ) || isinf( d ), d, ret );
	return ret;
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
faster_log2f( VT d )
{
	typedef VT fvec;

	fvec x = fvec::convert_int( d.as_int() );
	x *= fvec( 1.F/(1<<23) );
	x = x - fvec( 127.F );
	fvec y = x - floorf( x );
	return x + nmadd( y, y, y ) * fvec( 0.346607F );
}

////////////////////////////////////////

// TODO: force inline?
template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
fast_log2f( VT d )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;

	fvec x0 = d;
	fvec e = fvec::convert_int(
		lsr( d.as_int() & ivec( vector_limits<fvec>::exponent_mask ),
			 vector_limits<fvec>::mantissa_bits )
		- vector_limits<fvec>::exponent_bias );

	fvec m = ( ( d.as_int() & ivec( vector_limits<fvec>::mantissa_mask ) ) |
			   ivec( vector_limits<fvec>::exponent_bias << vector_limits<fvec>::mantissa_bits ) ).as_float();
	m -= float_constants<fvec>::one();

	// Chebyshev polynomial to approximate log2
	fvec y = fma( m, fvec( -0.0258411662F ), fvec( 0.1217970128F ) );
	y = fma( m, y, fvec( -0.2779042655F ) );
	y = fma( m, y, fvec( 0.4575485901F ) );
	y = fma( m, y, fvec( -0.7181451002F ) );
	y = fma( m, y, fvec( 1.4425449290F ) );

	y = fma( m, y, e );

	// this is a fast routine, let's drop the out of bounds checks
	fvec ret = y;
//	fvec ret = ifthen( x0 < fvec::zero(), fvec( std::numeric_limits<float>::quiet_NaN() ), y );

	// although log(0) may not be that out of bounds...
	ret = ifthen( x0 == fvec::zero(), fvec( - std::numeric_limits<float>::infinity() ), ret );
//	ret = ifthen( isnan( x0 ) || isinf( x0 ), x0, ret );
	return ret;
}

////////////////////////////////////////

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_logf( VT f )
{
	return fast_log2f( f ) * float_constants<VT>::log_2();
}

////////////////////////////////////////

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_log10f( VT f )
{
	return fast_log2f( f ) * float_constants<VT>::log_2_10();
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
expf( VT x0 )
{
	typedef VT fvec;
	typedef typename VT::int_vec_type ivec;
	typedef typename ivec::mask_type mvec;
	typedef float_constants<fvec> fconst;
	typedef float_extract_constants<fvec> econst;
	typedef vector_limits<fvec> limits;

	// based on sun microsystems / freebsd implementation
	const fvec ln2hi( uint32_t(0x3f317200) );
	const fvec ln2lo( uint32_t(0x35bfbe8e) );
	const fvec invln2( uint32_t(0x3fb8aa3b) );
	const fvec P1( 1.6666625440e-1f );
	const fvec P2( -2.7667332906e-3f );

	ivec hx = x0.as_int();
	hx = hx & int_constants<ivec>::nonsign_bitmask();
	mvec do_inverse = (x0 < fconst::zero());
	mvec gt_05_ln2 = hx > ivec( 0x3eb17218 );
	mvec gt_15_ln2 = hx > ivec( 0x3f851592 );

	const fvec one = fconst::one();
	fvec signmul = ifthen( do_inverse, - one, one );
	const fvec half_sign = fconst::one_half() * signmul;

	ivec k_15_ln2 = fma( invln2, x0, half_sign ).convert_to_int();
	ivec k_not15_ln2 = signmul.convert_to_int();
	ivec k = ifthen( gt_15_ln2, k_15_ln2, k_not15_ln2 );

	fvec fk = fvec::convert_int( k );

	fvec hi = ifthen( gt_05_ln2, nmadd( fk, ln2hi, x0 ), x0 );
	fvec lo = ifthen( gt_05_ln2, fk * ln2lo, fconst::zero() );

	fvec x = ifthen( gt_05_ln2, hi - lo, x0 );
	k = ifthen( gt_05_ln2, k, ivec::zero() );

	// x in primary range...
	fvec xx = x * x;
	fvec c = x - xx * (P1 + xx * P2);
	fvec y = one + ( x * c / (fconst::two() - c) - lo + hi );

	const ivec testMaxK = ivec( limits::exponent_bias );
	const ivec testMinK = ivec( -( limits::exponent_bias - 1 ) );
	ivec n = ( clamp( k, testMinK, testMaxK ) + econst::bias() );
	ivec sn = n << limits::mantissa_bits;
	fvec ey = y * sn.as_float();

	ey = ifthen( k > testMaxK, fconst::infinity(), ey );
	ey = ifthen( k < testMinK, fconst::zero(), ey );
	y = ifthen( k == ivec::zero(), y, ey );
	y = ifthen( hx <= ivec( 0x39000000 ), one + x0, y );
	y = ifthen( hx >= ivec( 0x42aeac50 ), ifthen( do_inverse, fconst::zero(), fconst::infinity() ), y );
	y = ifthen( isnan( x0 ), x, y );
	y = ifthen( isinf( x0 ) & (x0 < fconst::zero()), fconst::zero(), y );

	return y;
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
exp2f( VT x0 )
{
	typedef VT fvec;
	typedef typename VT::int_vec_type ivec;
	typedef typename VT::mask_type mvec;
	typedef float_constants<fvec> fconst;
	typedef int_constants<ivec> iconst;

	const fvec P0( float(1.535336188319500E-004) );
	const fvec P1( float(1.339887440266574E-003) );
	const fvec P2( float(9.618437357674640E-003) );
	const fvec P3( float(5.550332471162809E-002) );
	const fvec P4( float(2.402264791363012E-001) );
	const fvec P5( float(6.931472028550421E-001) );

	fvec px = floorf( x0 );
	ivec i0 = px.convert_to_int();
	fvec x = x0 - px;
	mvec gt05 = x > fconst::one_half();
	i0 = ifthen( gt05, i0 + iconst::one(), i0 );
	x = ifthen( gt05, x - fconst::one(), x );

	fvec peval = P0;
	peval = fma( peval, x, P1 );
	peval = fma( peval, x, P2 );
	peval = fma( peval, x, P3 );
	peval = fma( peval, x, P4 );
	peval = fma( peval, x, P5 );
	px = fma( x, peval, fconst::one() );
	fvec rval = ldexpf( px, i0 );
	rval = ifthen( x0 == fconst::zero(), fconst::one(), rval );
	rval = ifthen( x0 > fvec( 127.F ), fconst::infinity(), rval );
	rval = ifthen( x0 < fvec( -126.F ), fconst::zero(), rval );
	return rval;
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
exp10f( VT x0 )
{
	typedef VT fvec;
	typedef typename VT::int_vec_type ivec;
	typedef float_constants<fvec> fconst;

	const fvec P0( float(2.063216740311022E-001) );
	const fvec P1( float(5.420251702225484E-001) );
	const fvec P2( float(1.171292686296281E+000) );
	const fvec P3( float(2.034649854009453E+000) );
	const fvec P4( float(2.650948748208892E+000) );
	const fvec P5( float(2.302585167056758E+000) );

	const fvec LOG210( float(3.32192809488736234787e0) );
	const fvec LG102A( float(3.00781250000000000000E-1) );
	const fvec LG102B( float(2.48745663981195213739E-4) );
	const fvec MAXL10( float(38.230809449325611792) );

	fvec qx = floorf( fma( x0, LOG210, fconst::one_half() ) );
	ivec n = qx.convert_to_int();
	fvec x = x0 - qx * LG102A;
	x -= qx * LG102B;

	fvec r = P0;
	r = fma( r, x, P1 );
	r = fma( r, x, P2 );
	r = fma( r, x, P3 );
	r = fma( r, x, P4 );
	r = fma( r, x, P5 );
	r = fma( x, r, fconst::one() );
	r = ldexpf( r, n );

	r = ifthen( x0 == fconst::zero(), fconst::one(), r );
	r = ifthen( x0 > fvec( MAXL10 ), fconst::infinity(), r );
	r = ifthen( x0 < fvec( -MAXL10 ), fconst::zero(), r );
	return r;
}

////////////////////////////////////////

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_expf( VT x0 )
{
	typedef VT fvec;
	typedef typename VT::int_vec_type ivec;
	typedef float_constants<fvec> fconst;

	const fvec C1( float(0.693359375) );
	const fvec C2( float(-2.12194440e-4) );

	const fvec LOG2EF( float(1.44269504088896341) );
	const fvec MAXLOGF( float(88.02969187150841) );
	const fvec MINLOGF( float(-88.7228391116729996) );

	const fvec P0( float(1.9875691500E-4) );
	const fvec P1( float(1.3981999507E-3) );
	const fvec P2( float(8.3334519073E-3) );
	const fvec P3( float(4.1665795894E-2) );
	const fvec P4( float(1.6666665459E-1) );
	const fvec P5( float(5.0000001201E-1) );

	fvec z = floorf( fma( LOG2EF, x0, fconst::one_half() ) );
	fvec x = nmadd( z, C2, nmadd( z, C1, x0 ) );
	ivec n = z.convert_to_int();
	z = x * x;

	fvec r = fma(
		fma(
			fma(
				fma(
					fma(
						fma( x, P0, P1 ),
						x, P2 ),
					x, P3 ),
				x, P4 ),
			x, P5 ),
		z, x + fconst::one() );
	r = ldexpf( r, n );

	r = ifthen( x0 == fconst::zero(), fconst::one(), r );
	r = ifthen( x0 > MAXLOGF, fconst::infinity(), r );
	r = ifthen( x0 < MINLOGF, fconst::zero(), r );
	return r;
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
faster_exp2f( VT d )
{
	typedef VT fvec;
	typedef typename fvec::int_vec_type ivec;
	typedef float_constants<fvec> fconst;

	fvec y = d - floorf( d );
	y = nmadd( y, y, y ) * fvec( 0.33971F );

	fvec x = d + fvec( 127.F ) - y;
	x = x * fvec( float((1<<23)) );
	ivec tmp = x.convert_to_int();
	x = tmp.as_float();
#ifdef PAL_ENABLE_SSE4_1
	// we know the blend only pays attention to the first bit
	return ifthen( x & int_constants<fvec>::sign_bitmask(), fconst::infinity(), x );
#else
	return ifthen( signbit(x) > ivec::zero(), fconst::infinity(), x );
#endif
}

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_exp2f( VT v )
{
	return fast_expf( v * float_constants<VT>::log_2() );
}

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_exp10f( VT v )
{
	return fast_expf( v * float_constants<VT>::log_10() );
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_ANY_FLOAT(VT)
pow( VT v, VT p )
{
	typedef VT vec;
	typedef typename VT::value_type value_type;
	std::array<value_type, vec::value_count> vals;
	for ( int i = 0; i != vec::value_count; ++i )
		vals[i] = std::pow( v[i], p[i] );

	return VT( vals );
//	VT norm_res = exp2( p * log2( v ) );
//	return norm_res;
	// special cases:
	// if v < 0 is finite and p non-integer -> NaN
	// overflow -> inf
	// underflow -> 0
	// if v is 1 --> 1 (even if p is NaN)
	// if p is 0 --> 1 (even if v is NaN)
	// if NaN -> NaN
	// if v is +/-0 and p is odd integer > 0 -> v
	// if v is 0 and p > 0 and not odd integer -> 0
	// if v is -1 and p is +/-inf -> 1
	// if |v| < 1 and p is -inf -> +inf
	// if |v| < 1 and p is +inf -> 0
	// if |v| > 1 and p is -inf -> 0
	// if |v| > 1 and p is +inf -> +inf
	// if v is -inf and p is an odd integer < 0 -> -0
	// if v is -inf and p < 0 and not an odd integer -> 0
	// if v is -inf and p > 0 and odd integer -> -inf
	// if v is -inf and p > 0 and not odd integer -> +inf
	// if v is +inf and p < 0 -> +0
	// if v is +inf and p > 0 -> +inf
	// if v is +/-0 and p < 0 and odd integer -> +/-inf
	// if v is +/-0 and p < 0 not odd int -> +inf
}

template <typename VT>
PAL_INLINE PAL_ENABLE_ANY_FLOAT(VT)
pow( VT v, typename VT::value_type p )
{
	// can we pre-test p for any of the conditions and
	// short circuit???
	return pow( v, VT(p) );
}

template <typename VT>
inline PAL_ENABLE_ANY_FLOAT(VT)
pow( VT v, int p )
{
	switch ( p )
	{
		case 0: return float_constants<VT>::one();
		case -1: return recip( v );
		case 1: return v;
		case -2: return recip( square( v ) );
		case 2: return square( v );
		case 3: return v * v * v;
		case 4: return square( square( v ) );
		default:
			break;
	}

	bool dorecp = false;
	if ( p < 0 )
	{
		dorecp = true;
		p = -p;
	}
	VT a = float_constants<VT>::one();
	// we know p is > 2 because of what we handled in the switch
	VT sqv = square( v );
	while ( p > 1 )
	{
		a *= sqv;
		p -= 2;
	}
	if ( p == 1 )
		a *= v;

	return dorecp ? recip( a ) : a;
}

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
powf( VT v, VT p )
{
//	return pow( v, p );
	typedef VT fvec;
	typedef typename VT::mask_type mvec;
	typedef typename VT::int_vec_type ivec;
	fvec aval = fabsf( v );
	// TODO: need to increase precision of log2f estimate * p?
	// this gives us ~18 bits of mantissa precision, maybe 19
	// although is sometimes better
	fvec norm_res = exp2f( p * log2f( aval ) );

	const fvec zero = float_constants<fvec>::zero();
	const fvec one = float_constants<fvec>::one();
	const fvec inf = float_constants<fvec>::infinity();
	const ivec ione = int_constants<ivec>::one();

	// special cases:
	// if v < 0 is finite and p non-integer -> NaN
	// overflow -> inf
	// underflow -> 0
	// if v is 1 --> 1 (even if p is NaN)
	// if p is 0 --> 1 (even if v is NaN)
	// if NaN -> NaN
	// if v is +/-0 and p is odd integer > 0 -> v
	// if v is 0 and p > 0 and not odd integer -> 0
	// if v is -1 and p is +/-inf -> 1
	// if |v| < 1 and p is -inf -> +inf
	// if |v| < 1 and p is +inf -> 0
	// if |v| > 1 and p is -inf -> 0
	// if |v| > 1 and p is +inf -> +inf
	// if v is -inf and p is an odd integer < 0 -> -0
	// if v is -inf and p < 0 and not an odd integer -> 0
	// if v is -inf and p > 0 and odd integer -> -inf
	// if v is -inf and p > 0 and not odd integer -> +inf
	// if v is +inf and p < 0 -> +0
	// if v is +inf and p > 0 -> +inf
	// if v is +/-0 and p < 0 and odd integer -> +/-inf
	// if v is +/-0 and p < 0 not odd int -> +inf
	mvec v_is1 = ( v == one );
	mvec v_is0 = ( v == zero );
	mvec p_is0 = ( p == zero );
	mvec p_isInt = ( p == truncf(p) );
	mvec p_isOdd = ( p_isInt & mvec( ( p.convert_to_int_trunc() & ione ) > zero.as_int() ) );
	mvec p_isInf = isinf( p );
	mvec v_isInf = isinf( v );
	mvec v_isNeg = signbit( v ) == ione;
	mvec p_isNeg = signbit( p ) == ione;

	fvec res = norm_res;
	res = ifthen( v_isInf,
				  ifthen( v_isNeg,
						  ifthen( !p_isOdd && p_isNeg, -zero,
								  ifthen( p_isNeg, zero,
										  ifthen( p_isOdd,
												  copysignf( inf, v ),
												  float_constants<fvec>::infinity() ) ) ),
						  ifthen( p_isNeg, zero, float_constants<fvec>::infinity() ) ),
				  res );
	res = ifthen( v_is0,
				  ifthen( p_isNeg,
						  ifthen( p_isOdd, copysignf( inf, v ), inf ),
						  ifthen( p_isOdd, v, zero ) ),
				  res );
	res = ifthen( abs( v ) < one,
				  ifthen( p_isInf,
						  ifthen( p_isNeg, -p, zero ),
						  res ),
				  // v >= 1, we have a check for == 1 later
				  ifthen( p_isInf,
						  ifthen( p_isNeg, zero, p ),
						  res ) );
	res = ifthen( v == (- one) && p_isInf, one, res );

	res = ifthen( v_isNeg && ! p_isInt, - float_constants<fvec>::nan(), res );
	res = ifthen( isnan( v ), v, res );
	res = ifthen( isnan( p ), p, res );
	res = ifthen( v_is1, v, res );
	res = ifthen( p_is0, one, res );

	return res;
}

template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
powf( VT v, float p )
{
	return powf( v, VT(p) );
}

/// @brief compute powf with an integer power
template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
powf( VT v, int p )
{
	return pow( v, p );
}

////////////////////////////////////////

/// @brief computes a rough approximation of powf for normal values
template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
faster_powf( VT v, VT p )
{
	return faster_exp2f( p * faster_log2f( v ) );
}

/// @brief computes a rough approximation of powf for normal values
template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
faster_powf( VT v, float p )
{
	return faster_exp2f( p * faster_log2f( v ) );
}

////////////////////////////////////////

/// @brief compute fast_powf - a (close) approximation of powf(x,y) that doesn't handle all scenarios
///
/// This computes powf(x,y) as expf( y * logf(x) ) using logarithm
/// identities
template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_powf( VT v, VT p )
{
	return fast_expf( p * fast_logf( v ) );
}

/// @brief compute fast_powf with the same power for all vector items
template <typename VT>
PAL_INLINE PAL_ENABLE_FLOAT(VT)
fast_powf( VT v, float p )
{
	return fast_expf( VT::splat( p ) * fast_logf( v ) );
}

////////////////////////////////////////

template <typename VT>
inline PAL_ENABLE_FLOAT(VT)
cbrtf( VT v )
{
	typedef VT fvec;
	typedef typename VT::int_vec_type lvec;

	lvec xe;
	// reduct to 1.0 to 0.5
	fvec xm = frexpf( fabsf( v ), xe );

	// newton raphson equation:
	// x_1 = (1/3)(2*x1 + v / x^2);
	// or is it -2x?
	//
	// halley's
	// x_1 = x * ( x^3 + 2 * v) / (2 * x^3 + v)

	// approximation
	fvec x = fma( xm, nmadd( xm, fvec(0.191502161678719066F), fvec(0.697570460207922770F) ), fvec(0.492659620528969547F) );

	// one round of Halley's to refine
	fvec x3 = x * x * x;
	fvec x_1 = x * fma( xm, float_constants<fvec>::two(), x3 ) / fma( float_constants<fvec>::two(), x3, xm );

	// multiply by factor depending on the rounding necessary
	// to handle remainder of xe / 3
	lvec one = int_constants<lvec>::one();
	lvec three = lvec(3);
	lvec nxe = divide_by_const<3>( xe );
	lvec rem = xe - three * nxe;

	auto isone  = abs( rem ) == one;
	fvec y = ifthen( rem == lvec::zero(), x_1,
					 ifthen( xe < lvec::zero(),
							 ifthen( isone,
									 x_1 * fvec(float(1.0/1.2599210498948731648)),
									 x_1 * fvec(float(1.0/1.5874010519681994748)) ),
							 ifthen( isone,
									 x_1 * fvec(float(1.2599210498948731648)),
									 x_1 * fvec(float(1.5874010519681994748)) ) ) );

	y = ldexpf( copysign( y, v ), nxe );
	// handle inf and nan
	return ifthen( isnormal( v ), y, v );
}

} // namespace pal


#endif // _PAL_X86_SIMD_LOG_EXP_H_

