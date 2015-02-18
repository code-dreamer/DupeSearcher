// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(push)
#pragma warning(disable: 4571 4625 4626 4668 4820)

#include "targetver.h"

#include <stdio.h>
#include <conio.h>
#include <tchar.h>

// conflicts with std::numeric_limits<>::max
#define NOMINMAX

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#ifndef _INC_WINDOWS
#include <Windows.h>
#endif

#include <Shlwapi.h>

#include "std.h"
#include "Utils.h"
#include "md5.h"

#pragma warning(pop)

