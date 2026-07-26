#pragma once

#include <cmath>

// This Vectormath revision uses C float function names in namespace std.
// Some standard libraries expose those names as extensions and others do not.
#if !defined(_MSC_VER)
#define acosf acos
#define cosf cos
#define fabsf fabs
#define sinf sin
#define sqrtf sqrt
#define tanf tan
#endif

#include <vectormath.hpp>

#if !defined(_MSC_VER)
#undef acosf
#undef cosf
#undef fabsf
#undef sinf
#undef sqrtf
#undef tanf
#endif
