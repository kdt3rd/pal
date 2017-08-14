//
// Copyright (c) 2016-2017 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#ifndef _PAL_H_
# define _PAL_H_ 1

#include "common/compiler.h"
#include <cmath>
#include <cfloat>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <iostream>
#include <initializer_list>
#include <array>

/// @brief namespace that will be used by this library
///
/// if some incompatible API change happens or for some reason there
/// is a conflict with another package, change this.
#define PAL_NAMESPACE pal

#include "common/type_utils.h"

/// @brief The top-level namespace for all elements declared.
///
/// This header-only library will declare a variable number of classes
/// and utility functions, based on what CPU features are enabled. As
/// an example, under x86 with some level of SSE enabled, this will
/// declare (at least) mask4, ivec4, fvec4 covering use of 4 floats in
/// a single register. Additional types such as dvec2 (2 doubles),
/// ivec16 (16 uint16_t), and so on may be enabled.
///
namespace PAL_NAMESPACE
{

/// @brief Provides information about the vector type contained.
///
/// Similar in spirit (and derived from) std::numeric_limits. The
/// intention is to have this be usable in templated routines to
/// enable metaprogramming for vector types without having to
/// re-implement functions for each of the vector types declared (4,
/// 8, 16 floats per).
///
/// This should provide some constants used for masking values within
/// the SIMD value, as well as the more common constants such as how
/// many values are contained within the SIMD value.
///
template <typename vtype> struct vector_limits {};

}

// Set up any enabled MMX/SSE/AVX types and functions.
//
// for a great reference for the functions, see:
//
// https://software.intel.com/sites/landingpage/IntrinsicsGuide
# include "x86/simd_types.h"
// TODO: add "missing" types when SSE features aren't enabled
// like dvec4 and fvec8 when not on AVX
# include "x86/simd_constants.h"
# include "x86/simd_permute.h"
# include "x86/simd_load_store.h"
# include "x86/simd_math.h"
# include "x86/simd_log_exp.h"

//# include "neon/simd_types.h"

//# include "noarch/simd_types.h"

// include the buffer processing implementations
# include "buffer_process.h"

#endif // _PAL_H_
