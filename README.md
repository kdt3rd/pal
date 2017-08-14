PAL
---

The Parallel Acceleration Library (pal) is a collection of types, classes and utility templates and wrappers.

This is a header-only library, and should be compatible with clang, gcc, icc, msvc. Only gcc and clang have received heavy testing. Other compilers should work if they support C++11, although if the SIMD intrinsics are somehow named different for x86, then there may be more work and updating common/compiler.h

test programs have been provided that can be used to examine how well the library works as well as examples of use beyond the developer documentation.

