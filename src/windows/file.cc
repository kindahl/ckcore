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

#include "stdafx.hh"
#include "file.hh"
#include "util.hh"

namespace ckcore
{
    /**
     * Constructs a File object.
     * @param [in] file_path The path to the file.
     */
    File::File(const Path &file_path) : file_handle_(INVALID_HANDLE_VALUE),
		file_path_(file_path)
    {
    }

    /**
     * Destructs the File object. The file will be automatically closed if the
     * close function has not been called.
     */
    File::~File()
    {
        Close();
    }

    /**
     * Opens the file in the requested mode.
     * @param [in] file_mode Determines how the file should be opened. In write
     *                       mode the file will be created if it does not
     *                       exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool File::Open(FileMode file_mode)
    {
        // Check a file handle has already been opened, in that case try to close
        // it.
        if (file_handle_ != INVALID_HANDLE_VALUE && !Close())
            return false;

        // Open the file handle.
        switch (file_mode)
        {
            case ckOPEN_READ:
                file_handle_ = CreateFile(file_path_.Name().c_str(),
										  GENERIC_READ,
										  FILE_SHARE_READ | FILE_SHARE_WRITE,
										  NULL,OPEN_EXISTING,
										  FILE_ATTRIBUTE_ARCHIVE,NULL);
                break;

            case ckOPEN_WRITE:
                file_handle_ = CreateFile(file_path_.Name().c_str(),
										  GENERIC_WRITE,
										  FILE_SHARE_READ,NULL,CREATE_ALWAYS,
										  FILE_ATTRIBUTE_ARCHIVE,NULL);
                break;

			case ckOPEN_READWRITE:
				file_handle_ = CreateFile(file_path_.Name().c_str(),
										  GENERIC_WRITE,
										  FILE_SHARE_READ,NULL,OPEN_EXISTING,
										  FILE_ATTRIBUTE_ARCHIVE,NULL);
				break;
        }

        return file_handle_ != INVALID_HANDLE_VALUE;
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool File::Close()
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
     * @return If a file is open true is returned, otherwise false.
     */
    bool File::Test() const
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
     *         otherwise -1 is returned.
     */
    tint64 File::Seek(tint64 distance,FileWhence whence)
    {
        if (file_handle_ == INVALID_HANDLE_VALUE)
            return -1;

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
        }

		if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != ERROR_SUCCESS)
			return -1;

        return (tint64)li.QuadPart;
    }

    /**
     * Calculates the current file pointer position in the file.
     * @return If successfull the current file pointer position, otherwise -1
     *         is returned.
     */
    tint64 File::Tell() const
    {
        if (file_handle_ == INVALID_HANDLE_VALUE)
            return -1;

        // Obtain the current file pointer position by seeking 0 bytes from the
        // current position.
		LARGE_INTEGER li;
		li.QuadPart = 0;
		li.LowPart = SetFilePointer(file_handle_,0,&li.HighPart,FILE_CURRENT);

		if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != ERROR_SUCCESS)
			return -1;

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
    tint64 File::Read(void *buffer,tuint32 count)
    {
        if (file_handle_ == INVALID_HANDLE_VALUE)
            return -1;

		unsigned long read = 0;
		if (ReadFile(file_handle_,buffer,count,&read,NULL) == FALSE)
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
    tint64 File::Write(const void *buffer,tuint32 count)
    {
        if (file_handle_ == INVALID_HANDLE_VALUE)
            return -1;

		unsigned long written = 0;
		if (WriteFile(file_handle_,buffer,count,&written,NULL) == FALSE)
			return -1;
		else
			return written;
    }

    /**
     * Checks whether the file exist or not.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::Exist() const
    {
		return Exist(file_path_);
    }

    /**
     * Removes the file from the file system. If other links exists to the file
     * only this link will be deleted. If the file is opened it will be closed.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool File::Remove()
    {
		Close();

		return Remove(file_path_);
    }

    /**
     * Moves the file to use the new file path. If the new full path exist it
     * will not be overwritten. If the file is open it will be closed.
     * @param [in] new_file_path The new file path.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool File::Rename(const Path &new_file_path)
    {
        // If a file already exist abort so it will not be overwritten. 
        if (Exist(new_file_path))
            return false;

        Close();

		if (MoveFile(file_path_.Name().c_str(),
					 new_file_path.Name().c_str()) != FALSE)
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
    bool File::Time(struct tm &access_time,struct tm &modify_time,
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

        return Time(file_path_,access_time,modify_time,create_time);
    }

    /**
     * Checks if the active user has permission to open the file in a certain
     * file mode.
     * @param [in] file_mode The file mode to check for access permission.
     * @return If the active user have permission to open the file in the
     *         specified file mode true is returned, otherwise false is
     *         returned.
     */
    bool File::Access(FileMode file_mode) const
    {
        return Access(file_path_,file_mode);
    }

    /**
     * Calcualtes the size of the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    tint64 File::Size()
    {
        // If the file is not open, use the static in this case optimized
        // function.
		if (file_handle_ == INVALID_HANDLE_VALUE)
			return Size(file_path_.Name().c_str());

		LARGE_INTEGER li;
		li.QuadPart = 0;
		li.LowPart = GetFileSize(file_handle_,(LPDWORD)&li.HighPart);

		if (li.LowPart == 0xFFFFFFFF && GetLastError() != ERROR_SUCCESS)
			return -1;

		return li.QuadPart;
    }

    /**
     * Checks whether the specified file exist or not.
     * @param [in] file_path The path to the file.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::Exist(const Path &file_path)
    {
		unsigned long attr = GetFileAttributes(file_path.Name().c_str());

		return (attr != -1) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
    }

    /**
     * Removes the specified file from the file system. If other links exists
     * to the file only the specified link will be deleted.
     * @param [in] file_path The path to the file.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool File::Remove(const Path &file_path)
    {
        return DeleteFile(file_path.Name().c_str()) != FALSE;
    }

    /**
     * Moves the old file to use the new file path. If the new full path exist
     * it will not be overwritten.
     * @param [in] old_file_path The path to the file that should be moved.
     * @param [in] new_file_path The new path of the existing file.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool File::Rename(const Path &old_file_path,const Path &new_file_path)
    {
        if (Exist(new_file_path))
            return false;

        return MoveFile(old_file_path.Name().c_str(),
						new_file_path.Name().c_str()) != FALSE;
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
    bool File::Time(const Path &file_path,struct tm &access_time,
                    struct tm &modify_time,struct tm &create_time)
    {
		HANDLE file_handle = CreateFile(file_path.Name().c_str(),GENERIC_READ,
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
    bool File::Access(const Path &file_path,FileMode file_mode)
    {
        switch (file_mode)
        {
            case ckOPEN_READ:
				return Exist(file_path);

            case ckOPEN_WRITE:
			case ckOPEN_READWRITE:
				unsigned long attr = GetFileAttributes(file_path.Name().c_str());
				return (attr != -1) && !(attr & FILE_ATTRIBUTE_READONLY);
        }

        return false;
    }

    /**
     * Calcualtes the size of the specified file.
     * @param [in] file_path The path to the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    tint64 File::Size(const Path &file_path)
    {
		HANDLE file_handle = CreateFile(file_path.Name().c_str(),GENERIC_READ,
										FILE_SHARE_READ | FILE_SHARE_WRITE,
										NULL,OPEN_EXISTING,
										FILE_ATTRIBUTE_ARCHIVE,NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
			return false;

		LARGE_INTEGER li;
		li.QuadPart = 0;
		li.LowPart = GetFileSize(file_handle,(LPDWORD)&li.HighPart);

		if (li.LowPart == 0xFFFFFFFF && GetLastError() != ERROR_SUCCESS)
			return -1;
	    
		tint64 result = li.QuadPart;
		CloseHandle(file_handle);
	    
		return result;
    }
};
