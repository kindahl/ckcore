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
 * @file src/string.hh
 * @brief String helper functions.
 */

#pragma once
#include "ckcore/types.hh"

// No nice:
#ifdef _WINDOWS
#ifdef _UNICODE
//#define ckcore::string::asscanf swscanf
#define asscanf	swscanf
#else
//#define ckcore::string::asscanf sscanf
#define asscanf sscanf
#endif
#else
//#define ckcore:string::asscanf sscanf
#define asscanf sscanf
#endif

namespace ckcore
{
    namespace string
    {
		int astrcmp(const tchar *str1,const tchar *str2);
		int astrncmp(const tchar *str1,const tchar *str2,size_t n);
		size_t astrlen(const tchar *str);
		tchar *astrcpy(tchar *str1,const tchar *str2);
		tchar *astrncpy(tchar *str1,const tchar *str2,size_t n);

        wchar_t *ansi_to_utf16(const char *ansi,wchar_t *utf,int utf_len);
        char *utf16_to_ansi(const wchar_t *utf,char *ansi,int ansi_len);
    };
};

