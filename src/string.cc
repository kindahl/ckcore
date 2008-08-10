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
#include "string.hh"

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
		int compare(const tchar *str1,const tchar *str2)
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
    };
};
