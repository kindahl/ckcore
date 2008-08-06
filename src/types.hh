/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2008 Christian Kindahl
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file src/types.hh
 * @brief Defines custom types used by the library.
 */

#pragma once
#include <string>

namespace ckCore
{
#ifdef _WINDOWS
#ifdef _UNICODE
    typedef wchar_t tchar;
	typedef std::wstring tstring;
#ifndef ckT
#define ckT(quote) L##quote
#endif
#else
	typedef char tchar;
	typedef std::string tstring;
#ifndef ckT
#define ckT
#endif
#endif
    typedef __int64 tint64;
#endif

#ifdef _LINUX
    typedef char tchar;
    typedef long long tint64;
    typedef std::string tstring;
#endif
};

