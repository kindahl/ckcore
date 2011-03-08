/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2011 Christian Kindahl
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
#include <atlbase.h>
#include <atlapp.h>
#endif

#include <assert.h>

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
#if defined(_WINDOWS) && defined(_UNICODE)
            return wcscmp(str1,str2);
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
#if defined(_WINDOWS) && defined(_UNICODE)
            return wcsncmp(str1,str2,n);
#else
            return strncmp(str1,str2,n);
#endif
        }

        /**
         * Compares two strings in a case insensitive way.
         * @param [in] str1 The first string.
         * @param [in] str2 The second string.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
         *         identical to str2 0 is returned, if str1 is greater than
         *         str2 > 0 is returned.
         */
        int astrcmpi(const tchar *str1,const tchar *str2)
        {
#ifdef _WINDOWS
#ifdef _UNICODE
            return _wcsicmp(str1,str2);
#else
            return _stricmp(str1,str2);
#endif
#else
            return strcasecmp(str1,str2);
#endif
        }

        /**
         * Compares two strings in a case insensitive way.
         * @param [in] str1 The first string.
         * @param [in] str2 The second string.
         * @param [in] n The number of characters to compare.
         * @return If str1 is less than str2 < 0 is returned, if str1 is
         *         identical to str2 0 is returned, if str1 is greater than
         *         str2 > 0 is returned.
         */
        int astrncmpi(const tchar *str1,const tchar *str2,size_t n)
        {
#ifdef _WINDOWS
#ifdef _UNICODE
            return _wcsnicmp(str1,str2,n);
#else
            return _strnicmp(str1,str2,n);
#endif
#else
            return strncasecmp(str1,str2,n);
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
#if defined(_WINDOWS) && defined(_UNICODE)
            return wcslen(str);
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
#if defined(_WINDOWS) && defined(_UNICODE)
            return wcscpy(str1,str2);
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
#if defined(_WINDOWS) && defined(_UNICODE)
            return wcsncpy(str1,str2,n);
#else
            return strncpy(str1,str2,n);
#endif
        }

        /**
         * Creates a formatted string from a format description similar to that
         * of sprintf. This routine accept positional arguments like "%1$s".
         * @param [out] res Output formatted string.
         * @param [in] fmt The string format.
         * @param [in] args Variable argument list.
         */
        void vformatstr(tstring &res,const tchar * const fmt,
                        va_list args)
        {
#ifdef _WINDOWS
            // There are here several ways to write this routine:
            // 1) Call vasprintf(), insert the result into the string, free()
            //    the result.
            // 2) [the one implemented] Call Microsoft's _vscprintf() to
            //    calculate the string length upfront, make enough room for
            //    that length, then call sprintf() to print the string.
            // 3) Try once with snprintf(), and, if not enough room was
            //    available, make enough room and call snprintf() again.
            // 4) Try once with Microsoft's _snprintf(), and, if not enough
            //    room, increase room (perhaps exponentially) and try again.
            const int char_cnt = _vsctprintf_p(fmt,args);
            if (char_cnt == 0)
            {
                res.clear();
                return;
            }

            res.resize(char_cnt + 1);
            ATLVERIFY(char_cnt == _vstprintf_p(&res[0],char_cnt + 1,fmt,args));

            // Remove the null terminator, std::string will add its own if
            // necessary.
            res.resize(char_cnt);

#else  // #ifdef _WINDOWS
            const int char_cnt = vsnprintf(NULL,0,fmt,args);
            if (char_cnt == 0)
            {
                res.clear();
                return;
            }

            res.resize(char_cnt + 1);
            if (char_cnt != vsnprintf(&res[0],char_cnt + 1,fmt,args))
                 assert(false);

            // Remove the null terminator, std::string will add its own if
            // necessary.
            res.resize(char_cnt);
#endif  // #ifdef _WINDOWS
        }

        /**
         * Creates a formatted string from a format description similar to that
         * of sprintf. This routine accept positional arguments like "%1$s".
         * @param [in] fmt The string format.
         * @return The resulting formatted string.
         */
        tstring formatstr(const tchar * const fmt,...)
        {
            tstring res;
            
            va_list args;
            va_start(args,fmt);
            
            vformatstr(res,fmt,args);
            
            va_end(args);
            return res;
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
          assert( utf_len >= 0 ); // See size_t typecast below.
#ifdef _WINDOWS
            int converted = MultiByteToWideChar(AreFileApisANSI() ? CP_ACP : CP_OEMCP,
                                                MB_PRECOMPOSED,ansi,(int)strlen(ansi) + 1,
                                                utf,utf_len);

            // Trunkate UTF-16 string if buffer is too small.
            if (converted == utf_len)
                utf[utf_len - 1] = '\0';
#else
            size_t ansi_len = strlen(ansi);
            size_t out_len = ansi_len >= size_t(utf_len) ? utf_len - 1 : ansi_len;

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
          assert( ansi_len >= 0 ); // See size_t typecast below.
#ifdef _WINDOWS
            int converted = WideCharToMultiByte(AreFileApisANSI() ? CP_ACP : CP_OEMCP,0,
                utf,(int)lstrlenW(utf) + 1,ansi,ansi_len,NULL,NULL);

            // Trunkate the ANSI string of buffer is too small.
            if (converted == ansi_len)
                ansi[ansi_len - 1] = '\0';
#else
            size_t utf_len = wcslen(utf);
            size_t out_len = utf_len >= size_t(ansi_len) ? ansi_len - 1 : utf_len;

            for (size_t i = 0; i < out_len; i++)
                ansi[i] = utf[i] & 0xff;

            ansi[out_len] = '\0';
#endif
            return ansi;
        }

        /**
         * Converts an ANSI string if necessary into UTF-16 format. If UTF-16 is not
         * used the very same ANSI string is copied.
         * @param [in] ansi The ANSI string to convert.
         * @param [in] out Pointer to buffer to which the ANSI or UTF-16 string
         *                 should be written.
         * @param [in] out_len The length of the out buffer counted in UTF-16
         *                     or ANSI characters.
         * @return The same pointer specified as the out argument.
         */
        tchar *ansi_to_auto(const char *ansi,tchar *out,int out_len)
        {
#if defined(_WINDOWS) && defined(_UNICODE)
            return ansi_to_utf16(ansi,out,out_len);
#else
            return strncpy(out,ansi,out_len);
#endif
        }
    }
}
