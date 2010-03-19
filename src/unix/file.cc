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

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include "ckcore/convert.hh"
#include "ckcore/file.hh"

namespace ckcore
{
    /**
     * Constructs a File object.
     * @param [in] file_path The path to the file.
     */
    File::File(const Path &file_path) : file_handle_(-1),file_path_(file_path)
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
		try
		{
			// Check a file handle has already been opened, in that case try to close
			// it.
			if (file_handle_ != -1 && !close())
				throw Exception2(ckT("Cannot close previously open file handle."));

			// Open the file handle.
			switch (file_mode)
			{
			case ckOPEN_READ:
				file_handle_ = ::open(file_path_.name().c_str(),O_RDONLY);
				break;

			case ckOPEN_WRITE:
				file_handle_ = ::open(file_path_.name().c_str(),O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);
				break;

			case ckOPEN_READWRITE:
				file_handle_ = ::open(file_path_.name().c_str(),O_RDWR,S_IRUSR | S_IWUSR);
				break;

			default:
				assert( false );
			}

			if (file_handle_ == -1)
				throw_from_errno( errno, NULL );

			// Set lock.
			struct flock file_lock;
			file_lock.l_start = 0;
			file_lock.l_len = 0;
			file_lock.l_type = file_mode == ckOPEN_READ ? F_RDLCK : F_WRLCK;
			file_lock.l_whence = SEEK_SET;

			if (fcntl(file_handle_,F_SETLK,&file_lock) == -1)
			{
				const int saved_errno = errno; // close() can overwrite errno.
				if (saved_errno == EACCES || saved_errno == EAGAIN)
				{
					close();
					throw_from_errno( saved_errno, ckT("Error setting the file lock: ") );
				}
			}
		}
		catch ( const std::exception & e )
		{
			rethrow_with_pfx( e,
				ckT("Error opening file \"%s\": "),
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
        if (file_handle_ == -1)
            return false;

        if (::close(file_handle_) == 0)
        {
            file_handle_ = -1;
            return true;
        }

        return false;
    }

    /**
     * Checks whether the file has been opened or not.
     * @return If a file is open true is returned, otherwise false.
     */
    bool File::test() const
    {
        return file_handle_ != -1;
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

        int ret = -1;

        switch (whence)
        {
            case ckFILE_CURRENT:
                ret = lseek(file_handle_,distance,SEEK_CUR);
                break;

            case ckFILE_BEGIN:
                ret = lseek(file_handle_,distance,SEEK_SET);
                break;

            case ckFILE_END:
                ret = lseek(file_handle_,distance,SEEK_END);
                break;

            default:
                assert( false );
        }

        if ( ret == -1 )
          throw_from_errno( errno, ckT("Cannot seek in file: ") );

        return ret;
    }

    /**
     * Calculates the current file pointer position in the file.
     * @return If successfull the current file pointer position, otherwise
     *         an exception is thrown.
     */
    tint64 File::tell2() const throw(std::exception)
    {
		check_file_is_open();

        // Obtain the current file pointer position by seeking 0 bytes from the
        // current position.
        const int ret = lseek(file_handle_,0,SEEK_CUR);

        if ( ret == 1 )
          throw_from_errno( errno, ckT("Cannot get the current file pointer: ") );

        return ret;
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
        if (file_handle_ == -1)
            return -1;

        return ::read(file_handle_,buffer,count);
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
        if (file_handle_ == -1)
            return -1;

        return ::write(file_handle_,buffer,count);
    }

    /**
     * Checks whether the file exist or not.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::exist() const
    {
        if (file_handle_ != -1)
        {
            struct stat file_stat;
            if (fstat(file_handle_,&file_stat) != 0)
                return false;

            return (file_stat.st_mode & S_IFDIR) == 0;
        }

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

        // Test if the file can be opened for writing, if not we should not
        // removal of the file.
        int file_handle = ::open(file_path_.name().c_str(),O_WRONLY,S_IRUSR | S_IWUSR);
        if (file_handle != -1)
        {
            struct flock file_lock;
            file_lock.l_start = 0;
            file_lock.l_len = 0;
            file_lock.l_type = F_WRLCK;
            file_lock.l_whence = SEEK_SET;

            int res = ::fcntl(file_handle,F_SETLK,&file_lock);
            int err = errno;
            ::close(file_handle);

            if (res == -1 && (err == EACCES || err == EAGAIN))
                return false;
        }

        return ::unlink(file_path_.name().c_str()) == 0;
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

        if (::rename(file_path_.name().c_str(),new_file_path.name().c_str()) == 0)
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
        if (file_handle_ != -1)
        {
            struct stat file_stat;
            if (fstat(file_handle_,&file_stat) == -1)
                return false;

            // Convert to local time.
            if (localtime_r(&file_stat.st_atime,&access_time) == NULL)
                return false;

            if (localtime_r(&file_stat.st_mtime,&modify_time) == NULL)
                return false;

            if (localtime_r(&file_stat.st_ctime,&create_time) == NULL)
                return false;

            return true;
        }

        return time(file_path_.name().c_str(),access_time,modify_time,create_time);
    }

    /**
     * Calculates the size of the file.
     * @return If successfull the size of the file, otherwise an exception is thrown.
     */
    tint64 File::size2() throw(std::exception)
    {
        // If the file is not open, use the static in this case optimized
        // function.
        if ( !test() )
            return size2(file_path_);

        try
        {
            tint64 cur_pos = tell2();
            
            tint64 size = seek2(0,ckFILE_END);

            seek2(cur_pos,ckFILE_BEGIN);

			return size;
		}
        catch ( const std::exception & e )
        {
            rethrow_with_pfx( e,
                              ckT("Error querying size of file \"%s\": "),
                              file_path_.name().c_str() );
        }
    }

    /**
     * Checks whether the specified file exist or not.
     * @param [in] file_path The path to the file.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::exist(const Path &file_path)
    {
        struct stat file_stat;
        if (stat(file_path.name().c_str(),&file_stat) != 0)
            return false;

        return (file_stat.st_mode & S_IFDIR) == 0;
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
        return unlink(file_path.name().c_str()) == 0;
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

        return ::rename(old_file_path.name().c_str(),
                        new_file_path.name().c_str()) == 0;
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
        struct stat file_stat;
        if (stat(file_path.name().c_str(),&file_stat) == -1)
            return false;

        // Convert to local time.
        if (localtime_r(&file_stat.st_atime,&access_time) == NULL)
            return false;

        if (localtime_r(&file_stat.st_mtime,&modify_time) == NULL)
            return false;

        if (localtime_r(&file_stat.st_ctime,&create_time) == NULL)
            return false;

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
                return ::access(file_path.name().c_str(),R_OK) == 0;

            case ckOPEN_WRITE:
                return ::access(file_path.name().c_str(),W_OK) == 0;

			case ckOPEN_READWRITE:
                return ::access(file_path.name().c_str(),W_OK | R_OK) == 0;
        }

        return false;
    }

	/**
	 * Checks if the file is hidden or not.
	 * @return If successful and if the file is hidden true is returned,
	 * 		   otherwise false is returned.
	 */
	bool File::hidden(const Path &file_path)
	{
		tstring base_name = file_path.base_name();
		if (base_name.size() < 1)
			return false;

		return base_name[0] == '.';
	}

    /**
     * Calculates the size of the specified file.
     * @param [in] file_path The path to the file.
     * @return If successfull the size of the file, otherwise an exception is thrown.
     */
    tint64 File::size2(const Path &file_path) throw(std::exception)
    {
        struct stat file_stat;
        if (stat(file_path.name().c_str(),&file_stat) == -1)
            throw_from_errno( errno, "Error querying size of file \"%s\": ", file_path.name().c_str() );

        return file_stat.st_size;
    }

    /**
     * Creates a File object describing a temporary file on the hard drive. The
     * file path is pointing to an unique file name in the default temporary
     * directory of the current system. The file is not automatically created.
     * @param [in] prefix Prefix to use on temporary file name.
     * @return File object to a temporary file.
     */
	File File::temp(const tchar *prefix)
	{
        if (prefix == NULL)
            prefix = ckT("file");

		tchar *tmp_name = tmpnam(NULL);
		if (tmp_name != NULL)
		{
            Path tmp_path(tmp_name);

            tstring file_name = prefix;
            file_name += tmp_path.base_name();

			Path full_path = tmp_path.dir_name().c_str();
            full_path += file_name.c_str();
			return File(full_path);
            
			//return File(tmp_name);
		}
		else
		{
			tchar tmp_name2[PATH_MAX+1];
			strcpy(tmp_name2,ckT("/tmp/"));
			strcat(tmp_name2,prefix);

            tchar convBuffer[convert::INT_TO_STR_BUFLEN];
            convert::ui32_to_str2(rand(), convBuffer);
			strcat(tmp_name2, convBuffer);
            
			strcat(tmp_name2,ckT(".tmp"));

			return File(tmp_name2);
		}
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
            prefix = ckT("file");

		tchar *tmp_name = tmpnam(NULL);
		if (tmp_name != NULL)
		{
            tstring file_name = prefix;
            file_name += Path(tmp_name).base_name();

			Path full_path = file_path;
            full_path += file_name.c_str();
			return File(full_path);
		}
		else
		{
			tchar tmp_name2[PATH_MAX+1];
			strcpy(tmp_name2,prefix);
            
            tchar convBuffer[convert::INT_TO_STR_BUFLEN];
            convert::ui32_to_str2(rand(), convBuffer);
			strcat(tmp_name2, convBuffer);
            
			strcat(tmp_name2,ckT(".tmp"));

			Path full_path = file_path;
			full_path += tmp_name2;

			return File(full_path);
		}
	}
}