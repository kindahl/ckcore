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

#include <ckcore/exception.hh>  // The include file for this module comes first.
#include <assert.h>
#include <stdarg.h>
#include <string.h>  // strerror_r

#ifdef _WINDOWS
#include <comdef.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlapp.h>
#endif  // #ifdef _WINDOWS

#include <ckcore/string.hh>

namespace ckcore
{
    /**
     * Construct an Exception object.
     * @param [in] err_msg The error message.
     */
    Exception2::Exception2(const ckcore::tchar * const err_msg)
    {
#if defined(_WINDOWS) && defined(_UNICODE)
        int utf16_size = lstrlen(err_msg) + 1;
        int utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg,utf16_size,NULL,
                                            0,NULL,NULL);

        assert(utf8_size != 0);

        err_msg_.resize(utf8_size);

        utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg,utf16_size,
                                        const_cast<char *>(err_msg_.c_str()),
                                        utf8_size,NULL,NULL);
#else
		err_msg_ = err_msg;
#endif
    }

    /**
     * Construct an Exception object.
     * @param [in] err_msg The error message.
     */
    Exception2::Exception2(const ckcore::tstring &err_msg)
    {
#if defined(_WINDOWS) && defined(_UNICODE)
		int utf16_size = static_cast<int>(err_msg.size()) + 1;
		int utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg.c_str(),
											utf16_size,NULL,
                                            0,NULL,NULL);

        assert(utf8_size != 0);

		err_msg_.resize(utf8_size);

		utf8_size = WideCharToMultiByte(CP_UTF8,0,err_msg.c_str(),utf16_size,
                                        const_cast<char *>(err_msg_.c_str()),
                                        utf8_size,NULL,NULL);
#else
		err_msg_ = err_msg;
#endif
    }

    /**
     * Returns the error message in UTF-8 format.
     * @return The error message.
     */
    const char *Exception2::what(void) const throw()
    {
       return err_msg_.c_str();
    }

	/**
     * Returns the error message in tstring format.
     * @return The error message.
     */
	tstring Exception2::message() const
	{
#if defined(_WINDOWS) && defined(_UNICODE)
		const int utf8_size = static_cast<int>(err_msg_.size()) + 1;

		int utf16_size = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,
											 err_msg_.c_str(),utf8_size,NULL,0);

		assert(utf16_size != 0);

		tstring result;
		result.resize(utf16_size + 1);

		utf16_size = MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,
										 err_msg_.c_str(),utf8_size,
										 const_cast<wchar_t *>(result.c_str()),
										 utf16_size);

		return result;
#else
		return err_msg_;
#endif
	}

    /**
     * Returns the exception message of the given exception object. The
     * function tests if the exception is an Exception2 and in that case
     * returns its message.
     * @param [in] e The exception object.
     * @return The error message of the exception.
     */
    ckcore::tstring get_except_msg(const std::exception &e)
    {
        const Exception2 *const ptr = dynamic_cast<const Exception2 *>(&e);
		if (ptr != NULL)
			return ptr->message();

		return ckcore::string::ansi_to_auto<1024>(e.what());
    }

    /**
     * Rethrows the exception with a prefix message.
     * @param [in] e The exception to rethrow.
     * @param [in] szFormatStr The message format string.
     */
    void rethrow_with_pfx(const std::exception &e,const ckcore::tchar * const fmt,...)
    {
        ckcore::tstring msg;

        va_list args;
        va_start(args,fmt);

        ckcore::string::vformatstr(msg,fmt,args);

        va_end(args);

        const ckcore::tstring err_msg = get_except_msg(e);
        msg.append(err_msg);

        throw Exception2(msg);
    }

#ifdef _WINDOWS
    void throw_from_hresult(const HRESULT res,const tchar * const pfx_fmt,...)
    {
        tstring msg;

        if (pfx_fmt != NULL)
        {
            va_list args;
            va_start(args,pfx_fmt);

			string::vformatstr(msg,pfx_fmt,args);

            va_end(args);
        }

        const tstring err_msg = _com_error(res).ErrorMessage();
        msg.append(err_msg);

        throw Exception2(msg);
    }

	// Parameter pfx_fmt can be NULL if there is no message prefix.
    static tstring build_last_error_msg(const DWORD lastErrorCode, const tchar * const pfx_fmt, va_list args )
	{
        // The caller should have checked whether there was a last error to collect.
        assert( lastErrorCode != ERROR_SUCCESS );

        tstring msg;

		if ( pfx_fmt != NULL )
		{
			ckcore::string::vformatstr(msg,pfx_fmt,args);
		}

        tstring errMsgFromLastError;

        // FormatMessage() does not return the size of the buffer needed.
        // We could try with a small buffer, and if that's not enough, double it
        // and so on. However, performance is not important at this point,
        // so we ask FormatMessage() to allocate a temporary buffer itself.

        LPTSTR buffer;
		DWORD dwLen = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
				                     NULL, lastErrorCode, 0, (LPTSTR)&buffer, 1, NULL );

        if( dwLen )
        {
          try
          {
            // This can throw an exception if out of memory.
            errMsgFromLastError = buffer;
          }
          catch ( ... )
          {
			ATLVERIFY( NULL == LocalFree( buffer ) );
            throw;
          }

			ATLVERIFY( NULL == LocalFree( buffer ) );
        }
        else
        {
          errMsgFromLastError = ckT("<no error description available>");
        }

        msg.append(errMsgFromLastError);

		return msg;
	}

    void throw_from_given_last_error(const DWORD lastErrorCode, const tchar * const pfx_fmt,...)
	{
        va_list args;
        va_start(args,pfx_fmt);
        
        tstring msg = build_last_error_msg(lastErrorCode, pfx_fmt, args);
        
        va_end(args);

		throw Exception2( msg );
	}

	void throw_from_last_error(const tchar * const pfx_fmt,...)
	{
		// Grab the last error as the very first thing,
		// in case something else overwrites it.
		const DWORD lastErrorCode = GetLastError();

        va_list args;
        va_start(args,pfx_fmt);
        
        tstring msg = build_last_error_msg(lastErrorCode,pfx_fmt,args);
        
        va_end(args);

		throw Exception2( msg );
	}


#endif  // #ifdef _WINDOWS

    static tstring get_errno_msg(const int errno_code)
    {
#ifdef WIN32
        const tchar * const errno_msg = _tcserror(errno_code);
#else
        
        // We could write a loop here, but it's unlikely
        // that any errno error messages are so long.
        tchar strerror_buffer[ 2048 ];
    
        const tchar * const errno_msg = strerror_r(errno_code,
                                                   strerror_buffer,
                                                   sizeof(strerror_buffer));
#endif
        return tstring(errno_msg);
    }

    
    void throw_from_errno(const int errno_code,
                          const tchar * const pfx_fmt,...)
    {
        tstring msg;

        if (pfx_fmt != NULL)
        {
            va_list args;
            va_start(args,pfx_fmt);

			string::vformatstr(msg,pfx_fmt,args);

            va_end(args);
        }

        const tstring errno_msg = get_errno_msg(errno_code);
        msg.append(errno_msg);

        throw Exception2(msg);
    }

    void throw_internal_error(const tchar *file,int line)
    {
      throw Exception2(string::formatstr(ckT("Internal error in %s at line %d."),file,line));
    }
 
}  // namespace ckcore