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

#include <stdarg.h>
#include <cstring>
#ifdef _WINDOWS
#include <windows.h>
#endif
#include "ckcore/string.hh"

namespace ckcore
{
    namespace string
    {
		/**
         * Wrapper around the strcmp-like functions to be compatible with
		 * different character encodings.
         * @param [in] str1 The first string.
		 * @param [in] str2 The second string.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
	     *         identical to str2 0 is returned, if str1 is greater than
		 *         str2 > 0 is returned.
         */
		int astrcmp(const tchar *str1,const tchar *str2)
		{
#ifdef _WINDOWS
#ifdef _UNICODE
			return wcscmp(str1,str2);
#else
			return strcmp(str1,str2);
#endif
#else
			return strcmp(str1,str2);
#endif
		}

		/**
         * Wrapper around the strncmp-like functions to be compatible with
		 * different character encodings.
         * @param [in] str1 The first string.
		 * @param [in] str2 The second string.
		 * @param [in] n The number of characters to compare.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
	     *         identical to str2 0 is returned, if str1 is greater than
		 *         str2 > 0 is returned.
         */
		int astrncmp(const tchar *str1,const tchar *str2,size_t n)
		{
#ifdef _WINDOWS
#ifdef _UNICODE
			return wcsncmp(str1,str2,n);
#else
			return strncmp(str1,str2,n);
#endif
#else
			return strncmp(str1,str2,n);
#endif
		}

		/**
         * Wrapper around the strlen-like functions to be compatible with
		 * different character encodings.
         * @param [in] str1 The first string.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
	     *         identical to str2 0 is returned, if str1 is greater than
		 *         str2 > 0 is returned.
         */
		size_t astrlen(const tchar *str)
		{
#ifdef _WINDOWS
#ifdef _UNICODE
			return wcslen(str);
#else
			return strlen(str);
#endif
#else
			return strlen(str);
#endif
		}

		/**
         * Wrapper around the strcpy-like functions to be compatible with
		 * different character encodings.
         * @param [in] str1 The first string.
		 * @param [in] str2 The second string.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
	     *         identical to str2 0 is returned, if str1 is greater than
		 *         str2 > 0 is returned.
         */
		tchar *astrcpy(tchar *str1,const tchar *str2)
		{
#ifdef _WINDOWS
#ifdef _UNICODE
			return wcscpy(str1,str2);
#else
			return strcpy(str1,str2);
#endif
#else
			return strcpy(str1,str2);
#endif
		}

		/**
         * Wrapper around the strncpy-like functions to be compatible with
		 * different character encodings.
         * @param [in] str1 The first string.
		 * @param [in] str2 The second string.
		 * @param [in] n The number of characters to copy.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
	     *         identical to str2 0 is returned, if str1 is greater than
		 *         str2 > 0 is returned.
         */
		tchar *astrncpy(tchar *str1,const tchar *str2,size_t n)
		{
#ifdef _WINDOWS
#ifdef _UNICODE
			return wcsncpy(str1,str2,n);
#else
			return strncpy(str1,str2,n);
#endif
#else
			return strncpy(str1,str2,n);
#endif
		}

        /**
         * Converts an ANSI string into UTF-16 (big endian) format.
         * @param [in] ansi The ANSI string to convert.
         * @param [in] utf Pointer to buffer to which the UTF-16 string should
         *                 be written.
         * @param [in] utf_len The length of the utf buffer counted in UTF-16
         *                     characters.
         * @return The same pointer specified as the utf argument.
         */
        wchar_t *ansi_to_utf16(const char *ansi,wchar_t *utf,int utf_len)
        {
#ifdef _WINDOWS
            int converted = MultiByteToWideChar(AreFileApisANSI() ? CP_ACP : CP_OEMCP,MB_PRECOMPOSED,
                ansi,(int)strlen(ansi) + 1,utf,utf_len);

            // Trunkate UTF-16 string if buffer is too small.
            if (converted == utf_len)
                utf[utf_len - 1] = '\0';
#else
            size_t ansi_len = strlen(ansi);
            size_t out_len = ansi_len >= utf_len ? utf_len - 1 : ansi_len;

            for (size_t i = 0; i < out_len; i++)
                utf[i] = ansi[i];

            utf[out_len] = '\0';
#endif
            return utf;
        }

        /**
         * Converts a UTF-16 (big endian) string into ANSI format.
         * @param [in] utf The UTF-16 string to convert.
         * @param [in] ansi Pointer to buffer to which the ANSI string should
         *                  be written.
         * @param [in] ansi_len The length of the utf buffer counted in ANSI
         *                      characters (bytes).
         * @return The same pointer specified as the ansi argument.
         */
        char *utf16_to_ansi(const wchar_t *utf,char *ansi,int ansi_len)
        {
#ifdef _WINDOWS
            int converted = WideCharToMultiByte(AreFileApisANSI() ? CP_ACP : CP_OEMCP,0,
                utf,(int)lstrlenW(utf) + 1,ansi,ansi_len,NULL,NULL);

            // Trunkate the ANSI string of buffer is too small.
            if (converted == ansi_len)
                ansi[ansi_len - 1] = '\0';
#else
            size_t utf_len = wcslen(utf);
            size_t out_len = utf_len >= ansi_len ? ansi_len - 1 : utf_len;

            for (size_t i = 0; i < out_len; i++)
                ansi[i] = utf[i] & 0xff;

            ansi[out_len] = '\0';
#endif
            return ansi;
        }
    };
};
