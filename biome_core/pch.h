// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <cstdint>
#include <stdlib.h>
#include <malloc.h>
#include <type_traits>
#include <assert.h>
#include <algorithm>
#include <utility>
#include <limits>

#include "Handle/Handle.h"
#include "Core/Defines.h"
#include "Core/Globals.h"
#include "Core/Utilities.h"
#include "Math/Math.h"
#include "Memory/Memory.h"

#endif //PCH_H
