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

#include <unistd.h>
#include <fcntl.h>
#include "file.hh"

namespace ckCore
{
    /**
     * Constructs a ckFile object.
     * @param [in] file_path The path of the file.
     */
    File::File(const TChar *file_path) : file_handle_(-1),file_path_(file_path)
    {
    }

    /**
     * Destrucs the ckFile object. The file will be automatically closed if the
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
        if (file_handle_ != -1 && !Close())
            return false;

        // Open the file handle.
        switch (file_mode)
        {
            case OPEN_READ:
                file_handle_ = open(file_path_.c_str(),O_RDONLY);
                break;

            case OPEN_WRITE:
                file_handle_ = open(file_path_.c_str(),O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);
                break;
        }

        return file_handle_ != -1;
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool File::Close()
    {
        if (file_handle_ == -1)
            return false;

        if (close(file_handle_) == 0)
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
    bool File::Test()
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
     *         otherwise -1 is returned.
     */
    TInt64 File::Seek(TInt64 distance,FileWhence whence)
    {
        if (file_handle_ == -1)
            return -1;

        switch (whence)
        {
            case FILE_CURRENT:
                return lseek(file_handle_,distance,SEEK_CUR);

            case FILE_BEGIN:
                return lseek(file_handle_,distance,SEEK_SET);

            case FILE_END:
                return lseek(file_handle_,distance,SEEK_END);
        }

        return -1;
    }

    /**
     * Calculates the current file pointer position in the file.
     * @return If successfull the current file pointer position, otherwise -1
     *         is returned.
     */
    TInt64 File::Tell()
    {
        if (file_handle_ == -1)
            return -1;

        // Obtain the current file pointer position by seeking 0 bytes from the
        // current position.
        return lseek(file_handle_,0,SEEK_CUR);
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
    TInt64 File::Read(void *buffer,unsigned long count)
    {
        if (file_handle_ == -1)
            return -1;

        return read(file_handle_,buffer,count);
    }

    /**
     * Writes raw data to the current file.
     * @param [in] buffer A pointer to the beginning of a buffer from which to
     *                    read data to be written to the file.
     * @param [in] count The number of bytes to write to the file.
     * @return If the operation failed -1 is returned, otherwise the function
     *         returns the number of bytes written (this may be zero).
     */
    TInt64 File::Write(const void *buffer,unsigned long count)
    {
        if (file_handle_ == -1)
            return -1;

        return write(file_handle_,buffer,count);
    }

    /**
     * Checks whether the file exist or not.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::Exist()
    {
        if (file_handle_ != -1)
        {
            struct stat file_stat;
            return fstat(file_handle_,&file_stat) == 0;
        }

        return Exist(file_path_.c_str());
    }

    /**
     * Deletes the file from the file system. If other links exists to the file
     * only this link will be deleted. If the file is opened it will be closed.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool File::Delete()
    {
        Close();

        return unlink(file_path_.c_str()) == 0;
    }

    /**
     * Moves the file to use the new file path. If the new full path exist it
     * will not be overwritten. If the file is open it will be closed.
     * @param [in] new_file_path The new file path.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool File::Rename(const TChar *new_file_path)
    {
        // If a file already exist abort so it will not be overwritten. 
        if (Exist(new_file_path))
            return false;

        Close();

        if (rename(file_path_.c_str(),new_file_path) == 0)
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
                    struct tm &create_time)
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

        return Time(file_path_.c_str(),access_time,modify_time,create_time);
    }

    /**
     * Checks if the active user has permission to open the file in a certain
     * file mode.
     * @param [in] file_mode The file mode to check for access permission.
     * @return If the active user have permission to open the file in the
     *         specified file mode true is returned, otherwise false is
     *         returned.
     */
    bool File::Access(FileMode file_mode)
    {
        return Access(file_path_.c_str(),file_mode);
    }

    /**
     * Calcualtes the size of the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    TInt64 File::Size()
    {
        // If the file is not open, use the static in this case optimized
        // function.
        if (file_handle_ == -1)
            return Size(file_path_.c_str());

        TInt64 cur_pos = Tell();
        TInt64 size = Seek(0,FILE_END);
        Seek(cur_pos,FILE_BEGIN);

        return size;
    }

    /**
     * Checks whether the specified file exist or not.
     * @param [in] file_path The path to the file.
     * @return If the file exist true is returned, otherwise false.
     */
    bool File::Exist(const TChar *file_path)
    {
        struct stat file_stat;
        return stat(file_path,&file_stat) == 0;
    }

    /**
     * Deletes the specified file from the file system. If other links exists
     * to the file only the specified link will be deleted.
     * @param [in] file_path The path to the file.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool File::Delete(const TChar *file_path)
    {
        return unlink(file_path) == 0;
    }

    /**
     * Moves the old file to use the new file path. If the new full path exist
     * it will not be overwritten.
     * @param [in] old_file_path The path to the file that should be moved.
     * @param [in] new_file_path The new path of the existing file.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool File::Rename(const TChar *old_file_path,const TChar *new_file_path)
    {
        if (Exist(new_file_path))
            return false;

        return rename(old_file_path,new_file_path) == 0;
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
    bool File::Time(const TChar *file_path,struct tm &access_time,
                    struct tm &modify_time,struct tm &create_time)
    {
        struct stat file_stat;
        if (stat(file_path,&file_stat) == -1)
            return false;

        // Convert to local time.
        if (localtime_r(&file_stat.st_atime,&access_time) == NULL)
            return false;

        if (localtime_r(&file_stat.st_mtime,&modify_time) == NULL)
            return false;

        if (localtime_r(&file_stat.st_ctime,&create_time) == NULL)
            return false;

        //struct tm *pLocalAccess = localtime(&Stat.st_atime);

        // Access time.
        /*Access.m_ucSecond = pLocalAccess->tm_sec;
        Access.m_ucMinute = pLocalAccess->tm_min;
        Access.m_ucHour = pLocalAccess->tm_hour;
        Access.m_ucDayMonth = pLocalAccess->tm_mday;
        Access.m_ucMonth = pLocalAccess->tm_mon;
        Access.m_ucYear = pLocalAccess->tm_year;
        Access.m_ucDayWeek = pLocalAccess->tm_wday;
        Access.m_usDayYear = pLocalAccess->tm_yday;

        switch (pLocalAccess->tm_isdst)
        {
            case -1:
                Access.m_DaySaveTime = CTime::DST_UNKNOWN;
                break;

            case 0:
                Access.m_DaySaveTime = CTime::DST_DISABLED;
                break;

            case 1:
                Access.m_DaySaveTime = CTime::DST_ENABLED;
                break;
        }*/

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
    bool File::Access(const TChar *file_path,FileMode file_mode)
    {
        switch (file_mode)
        {
            case OPEN_READ:
                return access(file_path,R_OK) == 0;

            case OPEN_WRITE:
                return access(file_path,W_OK) == 0;
        }

        return false;
    }

    /**
     * Calcualtes the size of the specified file.
     * @param [in] file_path The path to the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    TInt64 File::Size(const TChar *file_path)
    {
        struct stat file_stat;
        if (stat(file_path,&file_stat) == -1)
            return -1;

        return file_stat.st_size;
    }
};

