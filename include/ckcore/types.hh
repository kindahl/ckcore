/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2009 Christian Kindahl
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
 * @file include/ckcore/types.hh
 * @brief Defines custom types used by the library.
 */

#pragma once
#include <string>
#include <sstream>

namespace ckcore
{
#ifdef _WINDOWS
#ifdef _UNICODE
    typedef wchar_t tchar;
	typedef std::wstring tstring;
	typedef std::wstringstream tstringstream;
#ifndef ckT
#define ckT(quote) L##quote
#endif
#else
	typedef char tchar;
	typedef std::string tstring;
	typedef std::stringstream tstringstream;
#ifndef ckT
#define ckT
#endif
#endif
    typedef __int64 tint64;
    typedef unsigned __int64 tuint64;
    typedef long tint32;
    typedef unsigned long tuint32;
	typedef short tint16;
	typedef unsigned short tuint16;
#endif

#ifdef _UNIX
    typedef char tchar;
    typedef long long tint64;
    typedef unsigned long long tuint64;
    typedef int tint32;
    typedef unsigned int tuint32;
	typedef short tint16;
	typedef unsigned short tuint16;
    typedef std::string tstring;
	typedef std::stringstream tstringstream;
#ifndef ckT
#define ckT
#endif
#endif
};

