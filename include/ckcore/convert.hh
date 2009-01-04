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
 * @file include/ckcore/convert.hh
 * @brief Type conversion functions.
 */

#pragma once
#include "ckcore/types.hh"

namespace ckcore
{
    namespace convert
    {
		void sprintf(tchar *buffer,size_t size,const tchar *format,...);

		const tchar *b_to_str(bool value);
		const tchar *i32_to_str(tint32 value);
		const tchar *ui32_to_str(tuint32 value);
		const tchar *i64_to_str(tint64 value);
		const tchar *ui64_to_str(tuint64 value);

		tuint32 be_to_le32(tuint32 value);
		tuint16 be_to_le16(tuint16 value);

		void tm_to_dostime(struct tm &time,unsigned short &dos_date,
						   unsigned short &dos_time);
    };
};
