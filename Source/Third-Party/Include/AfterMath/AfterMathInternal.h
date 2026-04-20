#pragma once
#include <vcruntime.h>
#include <algorithm>

#define AFTERMATH_BEGIN namespace AfterMath {
#define AFTERMATH_END }

#if _HAS_CXX17
#define AFTERMATH_INLINE_VAR inline
#else
#define AFTERMATH_INLINE_VAR
#endif

#if _HAS_CXX20
#define AFTERMATH_CONSTEXPR20 constexpr
#else
#define AFTERMATH_CONSTEXPR20 inline
#endif
