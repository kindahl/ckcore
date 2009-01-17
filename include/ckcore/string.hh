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
 * @file include/ckcore/string.hh
 * @brief String helper functions.
 */

#pragma once
#include "ckcore/types.hh"

// Not very nice since they are not in the namespace.
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
/**
 * Wrapper around sscanf function.
 * @param [in] str The input string to scan.
 * @param [in] format The input string format.
 * @return If successfull the function returns the number of arguments matched
 *         and assigned, if the function failed EOF (macro) is returned.
 */
#define asscanf sscanf
#endif

namespace ckcore
{
    namespace string
    {
		int astrcmp(const tchar *str1,const tchar *str2);
		int astrncmp(const tchar *str1,const tchar *str2,size_t n);
		int astrcmpi(const tchar *str1,const tchar *str2);
		int astrncmpi(const tchar *str1,const tchar *str2,size_t n);
		size_t astrlen(const tchar *str);
		tchar *astrcpy(tchar *str1,const tchar *str2);
		tchar *astrncpy(tchar *str1,const tchar *str2,size_t n);

        wchar_t *ansi_to_utf16(const char *ansi,wchar_t *utf,int utf_len);
        char *utf16_to_ansi(const wchar_t *utf,char *ansi,int ansi_len);

		/**
		 * Converts an ANSI string if necessary into UTF-16 format. If UTF-16 is not
		 * used the very same ANSI string is returned.
		 * @param [in] str The ANSI string to return and maybe convert.
		 * @return The specified ANSI string either in ANSI or in UTF-16 format.
		 */
		template<size_t S>
		tstring ansi_to_auto(const char *str)
		{
#if defined(_WINDOWS) && defined(_UNICODE)
			tchar res[S];
			return tstring(ansi_to_utf16(str,res,sizeof(res)/sizeof(tchar)));
#else
			return tstring(str);
#endif
		}

		/**
		 * Converts an ANSI or a UTF-16 string (depending on compilation options)
		 * into ANSI format.
		 * @param [in] str The ANSI or UTF-16 string to convert.
		 * @return The specified string converted into ANSI format.
		 */
		template<size_t S>
		std::string auto_to_ansi(const tchar *str)
		{
#if defined(_WINDOWS) && defined(_UNICODE)
			char res[S];
			return std::string(utf16_to_ansi(str,res,sizeof(res)));
#else
			return std::string(str);
#endif
		}
    };
};

