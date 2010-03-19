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

#include "stdafx.hh"

#include <atldef.h>
#include <atlbase.h>
#include <atlapp.h>
#include <assert.h>

#include <ckcore/file.hh>

#include "util.hh"

namespace ckcore
{
#pragma warning( push )
#pragma warning( disable : 4290 )  // C++ exception specification ignored except to ...

    /**
     * Constructs a File object.
     * @param [in] file_path The path to the file.
     */
    File::File(const Path &file_path) : file_handle_(INVALID_HANDLE_VALUE),
		file_path_(file_path)
    {
    }

    /**
     * Opens the file in the requested mode.
     * @param [in] file_mode Determines how the file should be opened. In write
     *                       mode the file will be created if it does not
     *                       exist.
     * Errors are thrown as exceptions.
     */
	void File::open2(FileMode file_mode) throw(std::exception)
    {
        // Check a file handle has already been opened, in that case try to close
        // it.
        if (file_handle_ != INVALID_HANDLE_VALUE && !close())
			throw Exception2(ckT("Cannot close previously open file handle."));

        // Open the file handle.
        switch (file_mode)
        {
            case ckOPEN_READ:
                file_handle_ = CreateFile(file_path_.name().c_str(),
										  GENERIC_READ,
										  FILE_SHARE_READ,NULL,OPEN_EXISTING,
										  FILE_ATTRIBUTE_ARCHIVE,NULL);
                break;

            case ckOPEN_WRITE:
                file_handle_ = CreateFile(file_path_.name().c_str(),
										  GENERIC_WRITE,
										  FILE_SHARE_READ,NULL,CREATE_ALWAYS,
										  FILE_ATTRIBUTE_ARCHIVE,NULL);
                break;

			case ckOPEN_READWRITE:
				file_handle_ = CreateFile(file_path_.name().c_str(),
										  GENERIC_WRITE,
										  FILE_SHARE_READ,NULL,OPEN_EXISTING,
										  FILE_ATTRIBUTE_ARCHIVE,NULL);
				break;

			default:
				assert( false );
        }

		if ( file_handle_ == INVALID_HANDLE_VALUE )
		{
			throw_from_last_error( ckT("Error opening file \"%s\": "),
			                       file_path_.name().c_str() );
		}
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool File::close()
    {
        if (file_handle_ == INVALID_HANDLE_VALUE)
            return false;

        if (CloseHandle(file_handle_) == TRUE)
        {
            file_handle_ = INVALID_HANDLE_VALUE;
            return true;
        }

        return false;
    }

    /**
     * Checks whether the file has been opened or not.
     * @return If a file is open true is returned, otherwise false is returned.
     */
    bool File::test() const
    {
        return file_handle_ != INVALID_HANDLE_VALUE;
    }

    /**
     * Repositions the file pointer to the specified offset accoding to the
     * whence directive in the file.
     * @param [in] distance The number of bytes that the file pointer should
     *                      move.
     * @param [in] whence Specifies what to use as base when calculating the
     *                    final file pointer position.
     * @return If successfull the resulting file pointer location is returned,
     *         otherwise an exception is thrown.
     */
	tint64 File::seek2(tint64 distance,FileWhence whence) throw(std::exception)
    {
		check_file_is_open();

		LARGE_INTEGER li;
		li.QuadPart = distance;

        switch (whence)
        {
			case ckFILE_CURRENT:
				li.LowPart = SetFilePointer(file_handle_,li.LowPart,&li.HighPart,FILE_CURRENT);
				break;

            case ckFILE_BEGIN:
				li.LowPart = SetFilePointer(file_handle_,li.LowPart,&li.HighPart,FILE_BEGIN);
				break;

            case ckFILE_END:
				li.LowPart = SetFilePointer(file_handle_,li.LowPart,&li.HighPart,FILE_END);
				break;

			default:
				assert( false );
        }

		if (li.LowPart == INVALID_SET_FILE_POINTER)
		{
			const DWORD lastError = GetLastError();
			
			if ( lastError != ERROR_SUCCESS )
			{
				throw_from_given_last_error( lastError, NULL );
			}
		}

        return li.QuadPart;
    }

    /**
     * Calculates the current file pointer position in the file.
     * @return If successfull the current file pointer position, otherwise -1
     *         is returned.
     */
	tint64 File::tell2() const throw(std::exception)
    {
		check_file_is_open();

        // Obtain the current file pointer position by seeking 0 bytes from the
        // current position.
		LARGE_INTEGER li;
		li.QuadPart = 0;
		li.LowPart = SetFilePointer(file_handle_,0,&li.HighPart,FILE_CURRENT);

		if (li.LowPart == INVALID_SET_FILE_POINTER)
		{
			const DWORD lastError = GetLastError();
			if ( lastError != ERROR_SUCCESS )
			{
				throw_from_given_last_error( lastError, NULL );			
			}
		}

        return (tint64)li.QuadPart;
    }

    /**
     * Reads raw data from the current file.
     * @param [out] buffer A pointer to the beginning of a buffer in which to
     *                     put the data.
     * @param [in] count The number of bytes to read from the file.
     * @return If the operation failed -1 is returned, otherwise the function
     *         returns the number of bytes read (this may be zero when the end
     *         of the file has been reached).
     */
    tint64 File::read(void *buffer,tint64 count)
    {
		// ReadFile() takes a DWORD (defined as unsigned long) as the byte count.
		ATLASSERT( count >= 0 || count <= ULONG_MAX );

        if (file_handle_ == INVALID_HANDLE_VALUE)
            return -1;

		unsigned long read = 0;
		if (ReadFile(file_handle_,buffer,DWORD(count),&read,NULL) == FALSE)
			return -1;
		else
			return read;
    }

    /**
     * Writes raw data to the current file.
     * @param [in] buffer A pointer to the beginning of a buffer from which to
     *                    read data to be written to the file.
     * @param [in] count The number of bytes to write to the file.
     * @return If the operation failed -1 is returned, otherwise the function
     *         returns the number of bytes written (this may be zero).
     */
    tint64 File::write(const void *buffer,tint64 count)
    {
		// WriteFile() takes a DWORD (defined as unsigned long) as the byte count.
		ATLASSERT( count >= 0 || count <= ULONG_MAX );

        if (file_handle_ == INVALID_HANDLE_VALUE)
            return -1;

		unsigned long written = 0;
		if (WriteFile(file_handle_,buffer,DWORD(count),&written,NULL) == FALSE)
			return -1;
		else
			return written;
    }

    /**
     * Checks whether the file exist or not.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::exist() const
    {
		return exist(file_path_);
    }

    /**
     * Removes the file from the file system. If other links exists to the file
     * only this link will be deleted. If the file is opened it will be closed.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool File::remove()
    {
		close();
		return remove(file_path_);
    }

    /**
     * Moves the file to use the new file path. If the new full path exist it
     * will not be overwritten. If the file is open it will be closed.
     * @param [in] new_file_path The new file path.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool File::rename(const Path &new_file_path)
    {
        // If a file already exist abort so it will not be overwritten. 
        if (exist(new_file_path))
            return false;

        close();

		if (MoveFile(file_path_.name().c_str(),
					 new_file_path.name().c_str()) != FALSE)
		{
			file_path_ = new_file_path;
			return true;
		}

		return false;
    }

    /**
     * Obtains time stamps on when the file was last accessed, last modified
     * and created.
     * @param [out] access_time Time of last access.
     * @param [out] modify_time Time of last modification.
     * @param [out] create_time Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool File::time(struct tm &access_time,struct tm &modify_time,
                    struct tm &create_time) const
    {
        if (file_handle_ != INVALID_HANDLE_VALUE)
        {
			FILETIME access_ftime,modify_ftime,create_ftime;
			if (GetFileTime(file_handle_,&create_ftime,&access_ftime,
							&modify_ftime) != TRUE)
			{
				return false;
			}

			// Convert to local file time.
			FILETIME laccess_ftime,lmodify_ftime,lcreate_ftime;
			if (FileTimeToLocalFileTime(&access_ftime,&laccess_ftime) == FALSE)
				return false;
			if (FileTimeToLocalFileTime(&modify_ftime,&lmodify_ftime) == FALSE)
				return false;
			if (FileTimeToLocalFileTime(&create_ftime,&lcreate_ftime) == FALSE)
				return false;

			// Convert to system time.
			SYSTEMTIME access_stime,modify_stime,create_stime;

			if (FileTimeToSystemTime(&laccess_ftime,&access_stime) == FALSE)
				return false;
			if (FileTimeToSystemTime(&lmodify_ftime,&modify_stime) == FALSE)
				return false;
			if (FileTimeToSystemTime(&lcreate_ftime,&create_stime) == FALSE)
				return false;

			// Convert to struct tm.
			SysTimeToTm(access_stime,access_time);
			SysTimeToTm(modify_stime,modify_time);
			SysTimeToTm(create_stime,create_time);

            return true;
        }

        return time(file_path_,access_time,modify_time,create_time);
    }

    /**
     * Calcualtes the size of the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
	tint64 File::size2() throw(std::exception)
    {
        // If the file is not open, use the static in this case optimized
        // function.
		if ( !test() )
			return size(file_path_.name().c_str());

		LARGE_INTEGER li;
		li.QuadPart = 0;
		li.LowPart = GetFileSize(file_handle_,(LPDWORD)&li.HighPart);

		if (li.LowPart == INVALID_FILE_SIZE)
		{
			const DWORD lastError = GetLastError();
			
			if ( lastError != ERROR_SUCCESS )
			{
				throw_from_given_last_error( lastError, NULL );			
			}
		}

		return li.QuadPart;
    }

    /**
     * Checks whether the specified file exist or not.
     * @param [in] file_path The path to the file.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::exist(const Path &file_path)
    {
		unsigned long attr = GetFileAttributes(file_path.name().c_str());

		return (attr != -1) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
    }

    /**
     * Removes the specified file from the file system. If other links exists
     * to the file only the specified link will be deleted.
     * @param [in] file_path The path to the file.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool File::remove(const Path &file_path)
    {
        return DeleteFile(file_path.name().c_str()) != FALSE;
    }

    /**
     * Moves the old file to use the new file path. If the new full path exist
     * it will not be overwritten.
     * @param [in] old_file_path The path to the file that should be moved.
     * @param [in] new_file_path The new path of the existing file.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool File::rename(const Path &old_file_path,const Path &new_file_path)
    {
        if (exist(new_file_path))
            return false;

        return MoveFile(old_file_path.name().c_str(),
						new_file_path.name().c_str()) != FALSE;
    }

    /**
     * Obtains time stamps on when the specified file was last accessed, last
     * modified and created.
     * @param [in] file_path The path to the file.
     * @param [out] access_time Time of last access.
     * @param [out] modify_time Time of last modification.
     * @param [out] create_time Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool File::time(const Path &file_path,struct tm &access_time,
                    struct tm &modify_time,struct tm &create_time)
    {
		HANDLE file_handle = CreateFile(file_path.name().c_str(),GENERIC_READ,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,OPEN_EXISTING,
										FILE_ATTRIBUTE_ARCHIVE,NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
			return false;

        FILETIME access_ftime,modify_ftime,create_ftime;
		bool result = GetFileTime(file_handle,&create_ftime,&access_ftime,
								  &modify_ftime) == TRUE;
		CloseHandle(file_handle);

		if (!result)
			return false;

		// Convert to local file time.
		FILETIME laccess_ftime,lmodify_ftime,lcreate_ftime;
		if (FileTimeToLocalFileTime(&access_ftime,&laccess_ftime) == FALSE)
			return false;
		if (FileTimeToLocalFileTime(&modify_ftime,&lmodify_ftime) == FALSE)
			return false;
		if (FileTimeToLocalFileTime(&create_ftime,&lcreate_ftime) == FALSE)
			return false;

		// Convert to system time.
		SYSTEMTIME access_stime,modify_stime,create_stime;

		if (FileTimeToSystemTime(&laccess_ftime,&access_stime) == FALSE)
			return false;

		if (FileTimeToSystemTime(&lmodify_ftime,&modify_stime) == FALSE)
			return false;

		if (FileTimeToSystemTime(&lcreate_ftime,&create_stime) == FALSE)
			return false;

		// Convert to struct tm.
		SysTimeToTm(access_stime,access_time);
		SysTimeToTm(modify_stime,modify_time);
		SysTimeToTm(create_stime,create_time);

        return true;
    }

    /**
     * Checks if the active user has permission to open the specified file in a
     * certain file mode.
     * @param [in] file_path The path to the file.
     * @param [in] file_mode The file mode to check for access permission.
     * @return If the active user have permission to open the file in the
     *         specified file mode true is returned, otherwise false is
     *         returned.
     */
    bool File::access(const Path &file_path,FileMode file_mode)
    {
        switch (file_mode)
        {
            case ckOPEN_READ:
				return exist(file_path);

            case ckOPEN_WRITE:
			case ckOPEN_READWRITE:
				unsigned long attr = GetFileAttributes(file_path.name().c_str());
				return (attr != -1) && !(attr & FILE_ATTRIBUTE_READONLY);
        }

        return false;
    }

	/**
	 * Checks if the specified file is hidden or not.
	 * @return If successfull and if the file is hidden true is returned,
	 *		   otherwise false is returned.
	 */
	bool File::hidden(const Path &file_path)
	{
		unsigned long attr = GetFileAttributes(file_path.name().c_str());
		if (attr == INVALID_FILE_ATTRIBUTES)
			return false;

		return (attr & FILE_ATTRIBUTE_HIDDEN) != 0;
	}

    /**
     * Calcualtes the size of the specified file.
     * @param [in] file_path The path to the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
	tint64 File::size2(const Path &file_path) throw(std::exception)
    {
		try
		{
			HANDLE file_handle = CreateFile(file_path.name().c_str(),GENERIC_READ,
											FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL,OPEN_EXISTING,
											FILE_ATTRIBUTE_ARCHIVE,NULL);
			if (file_handle == INVALID_HANDLE_VALUE)
			{
				throw_from_last_error( NULL );
			}

			LARGE_INTEGER li;
			li.QuadPart = 0;
			li.LowPart = GetFileSize(file_handle,(LPDWORD)&li.HighPart);

			if (li.LowPart == INVALID_FILE_SIZE)
			{
				// Grab the last error before CloseHandle() or something else resets it.
				const DWORD lastError = GetLastError();
				if ( lastError != ERROR_SUCCESS )
				{
					ATLVERIFY( 0 != CloseHandle(file_handle) );
					throw_from_given_last_error( lastError, NULL );
				}
			}
		    
			tint64 result = li.QuadPart;
			ATLVERIFY( 0 != CloseHandle(file_handle) );
		    
			return result;
		}
        catch ( const std::exception & e )
        {
            rethrow_with_pfx( e,
                              ckT("Error querying size of file \"%s\": "),
                              file_path.name().c_str() );
        }
    }

	/**
	 * Creates a File object of a temporary file. The file path is generated
	 * to be placed in the systems default temporary directory.
	 * @param [in] prefix Prefix to use on temporary file name.
	 * @return File object of temp file.
	 */
	File File::temp(const tchar *prefix)
	{
		if (prefix == NULL)
			prefix = ckT("tmp");

		tchar dir_name[246];
		GetTempPath(sizeof(dir_name) / sizeof(tchar),dir_name);

		tchar tmp_name[260];
		GetTempFileName(dir_name,prefix,0,tmp_name);

		return File(tmp_name);
	}

	/**
	 * Creates a File object of a temporary file. The file path is generated
	 * to be placed in the specified path.
	 * @param [in] file_path The path to where the temporary file should be
	 *                       stored.
	 * @param [in] prefix Prefix to use on temporary file name.
	 * @return File object of temp file.
	 */
	File File::temp(const Path &file_path,const tchar *prefix)
	{
		if (prefix == NULL)
			prefix = ckT("tmp");

		tchar tmp_name[260];
		GetTempFileName(file_path.name().c_str(),prefix,0,tmp_name);

		return File(tmp_name);
	}

#pragma warning( pop )

};