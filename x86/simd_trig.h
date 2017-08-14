//
// Copyright (c) 2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#if !defined _PAL_H_
# error "Never use <pal/x86/simd_trig.h> directly. Include <pal.h> instead."
#endif

#ifndef _PAL_X86_SIMD_TRIG_H_
# define _PAL_X86_SIMD_TRIG_H_ 1

namespace PAL_NAMESPACE
{

template <typename VT>
inline
typename is_float_vec<VT>::type::int_vec_type
reduce_pi_2( VT &x, VT &y, VT v )
{
}

template <typename VT>
inline
typename is_float_vec<VT>::type
sinf( VT v )
{
	typedef VT fvec;

	fvec pi_4 = float_constants<fvec>::pi_quarter();
	fvec c = v / pi_4;
	lvec n = c.convert_to_int_trunc();
	fvec red = nmadd( vec::convert_int( n ), pi_4, v );
	
	const fvec half = float_constants<fvec>::one_half();
	const fvec S1 = fvec( 0xbe2aaaab ); // -1.6666667163e-01
	const fvec S2 = fvec( 0x3c088889 ); // 8.3333337680e-03
	const fvec S3 = fvec( 0xb9500d01 ); // -1.9841270114e-04
	const fvec S4 = fvec( 0x3638ef1b ); // 2.7557314297e-06
	const fvec S5 = fvec( 0xb2d72f34 ); // -2.5050759689e-08
	const fvec S6 = fvec( 0x2f2ec9d3 ); // 1.5896910177e-10
	fvec x = red;
	fvec z = x * x;
	fvec v = x * z;
	fvec r = fma( S6, z, S5 );
	r = fma( r, z, S4 );
	r = fma( r, z, S3 );
	r = fma( r, z, S2 );
	return ifthen( iy == 0,
				   fma( v, fma( r, z, S1 ), x ),
				   x - nmsub( v, S1, fms( nmadd( r, v, half * y ), z, y ) ) );
}

} // namespace pal


#endif // _PAL_X86_SIMD_TRIG_H_

