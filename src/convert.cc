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
#include "convert.hh"

namespace ckcore
{
    namespace convert
    {
		tchar global_buffer[64];

		/**
		 * A safe platform intependant version of the sprintf function.
		 * @param [in] buffer The target string buffer.
		 * @param [in] size The size of the buffer in bytes.
		 * @param [in] format Format string.
		 */
		void sprintf(tchar *buffer,size_t size,const tchar *format,...)
		{
			va_list ap;
			va_start(ap,format);

#ifdef _WINDOWS
#ifdef _UNICODE
			_vsnwprintf_s(buffer,size,size/sizeof(tchar) - 1,format,ap);
#else
			_vsnprintf_s(buffer,size,size - 1,format,ap);
#endif
#else
			vsnprintf(buffer,size - 1,format,ap);
#endif
			va_end(ap);
		}

		/**
         * Converts the specified boolean value into a string. This function
         * uses a global buffer so the result pointer must not be freed.
         * @param [in] value The integer value to convert.
         * @return A pointer to the string containing the converter value.
         */
		const tchar *b_to_str(bool value)
		{
			global_buffer[0] = '\0';
			sprintf(global_buffer,sizeof(global_buffer),ckT("%d"),value);
			return global_buffer;
		}

        /**
         * Converts the specified 32-bit integer value into a string. This
         * function uses a global buffer so the result pointer must not be 
         * freed.
         * @param [in] value The integer value to convert.
         * @return A pointer to the string containing the converter value.
         */
		const tchar *i32_to_str(tint32 value)
		{
			global_buffer[0] = '\0';
			sprintf(global_buffer,sizeof(global_buffer),ckT("%d"),value);
			return global_buffer;
		}

		/**
         * Converts the specified unsigned 32-bit integer value into a string.
         * This function uses a global buffer so the result pointer must not be
         * freed.
         * @param [in] value The integer value to convert.
         * @return A pointer to the string containing the converter value.
         */
		const tchar *ui32_to_str(tuint32 value)
		{
			global_buffer[0] = '\0';
			sprintf(global_buffer,sizeof(global_buffer),ckT("%u"),value);
			return global_buffer;
		}

		/**
         * Converts the specified 64-bit integer value into a string. This
         * function uses a global buffer so the result pointer must not be
         * freed.
         * @param [in] value The integer value to convert.
         * @return A pointer to the string containing the converter value.
         */
		const tchar *i64_to_str(tint64 value)
		{
			global_buffer[0] = '\0';
#ifdef _WINDOWS
			sprintf(global_buffer,sizeof(global_buffer),ckT("%I64d"),value);
#else
			sprintf(global_buffer,sizeof(global_buffer),ckT("%ld"),value);
#endif
			return global_buffer;
		}

		/**
         * Converts the specified unsigned 64-bit integer value into a string.
         * This function uses a global buffer so the result pointer must not be
         * freed.
         * @param [in] value The integer value to convert.
         * @return A pointer to the string containing the converter value.
         */
		const tchar *ui64_to_str(tuint64 value)
		{
			global_buffer[0] = '\0';
#ifdef _WINDOWS
			sprintf(global_buffer,sizeof(global_buffer),ckT("%I64u"),value);
#else
			sprintf(global_buffer,sizeof(global_buffer),ckT("%lu"),value);
#endif
			return global_buffer;
		}
    };
};

