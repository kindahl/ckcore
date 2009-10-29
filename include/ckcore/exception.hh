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
 * @file include/ckcore/exception.hh
 * @brief Defines the exception base class.
 */

#pragma once

#ifdef _WINDOWS
#include <windows.h>
#endif

#include "ckcore/types.hh"
#include "ckcore/string.hh"

namespace ckcore
{
	/**
     * @brief Exception class.
     */
    class Exception2 : public std::exception
    {
    private:
		std::string err_msg_;

    public:
        Exception2(const tchar * const err_msg);
        Exception2(const tstring &err_msg);
        virtual ~Exception2() throw() {};

        virtual const char *what(void) const throw();
		tstring message() const;
    };

    tstring get_except_msg(const std::exception &e);
    void rethrow_with_pfx(const std::exception &e,const tchar *fmt,...) __attribute__ ((format (printf, 2, 3)));

#ifdef _WINDOWS
    void throw_from_hresult(HRESULT res,const tchar * pfx_fmt,...);
#endif
    void throw_from_errno(int errno_code,const tchar * pfx_fmt,... ) __attribute__ ((format (printf, 2, 3)));;
};  // namespace ckcore