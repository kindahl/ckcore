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
     */
    CFile::CFile(const TChar *szFilePath) : m_iHandle(-1),m_FilePath(szFilePath)
    {
    }

    /**
     * Destrucs the ckFile object. The file will be automatically closed if the
     * close function has not been called.
     */
    CFile::~CFile()
    {
        Close();
    }

    /**
     * Opens the file in the requested mode.
     * @param [in] FileMode Determines how the file should be opened. In write mode
     *                      the file will be created if it does not exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool CFile::Open(EFileMode FileMode)
    {
        // Check a file handle has already been opened, in that case try to close
        // it.
        if (m_iHandle != -1 && !Close())
            return false;

        // Open the file handle.
        switch (FileMode)
        {
            case OPEN_READ:
                m_iHandle = open(m_FilePath.c_str(),O_RDONLY);
                break;

            case OPEN_WRITE:
                m_iHandle = open(m_FilePath.c_str(),O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);
                break;
        }

        return m_iHandle != -1;
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool CFile::Close()
    {
        if (m_iHandle == -1)
            return false;

        if (close(m_iHandle) == 0)
        {
            m_iHandle = -1;
            return true;
        }

        return false;
    }

    /**
     * Checks whether the file has been opened or not.
     * @return If a file is open true is returned, otherwise false.
     */
    bool CFile::Test()
    {
        return m_iHandle != -1;
    }

    /**
     * Repositions the file pointer to the specified offset accoding to the
     * whence directive in the file.
     * @param [in] iDistance The number of bytes that the file pointer should
     *                       move.
     * @param [in] FileWhence Specifies what to use as base when calculating
     *                        the final file pointer position.
     * @return If successfull the resulting file pointer location is returned,
     *         otherwise -1 is returned.
     */
    TInt64 CFile::Seek(TInt64 iDistance,EFileWhence FileWhence)
    {
        if (m_iHandle == -1)
            return -1;

        int iWhence;
        switch (FileWhence)
        {
            case FILE_CURRENT:
                return lseek(m_iHandle,iDistance,SEEK_CUR);

            case FILE_BEGIN:
                return lseek(m_iHandle,iDistance,SEEK_SET);

            case FILE_END:
                return lseek(m_iHandle,iDistance,SEEK_END);
        }

        return -1;
    }

    /**
     * Calculates the current file pointer position in the file.
     * @return If successfull the current file pointer position, otherwise -1
     *         is returned.
     */
    TInt64 CFile::Tell()
    {
        if (m_iHandle == -1)
            return -1;

        // Obtain the current file pointer position by seeking 0 bytes from the
        // current position.
        return lseek(m_iHandle,0,SEEK_CUR);
    }

    /**
     * Reads raw data from the current file.
     * @param [out] pBuffer A pointer to the beginning of a buffer in which to
     *                      put the data.
     * @param [in] ulCount The number of bytes to read from the file.
     * @return If the operation failed -1 is returned, otherwise the function
     *         returns the number of bytes read (this may be zero when the end
     *         of the file has been reached).
     */
    TInt64 CFile::Read(void *pBuffer,unsigned long ulCount)
    {
        if (m_iHandle == -1)
            return -1;

        return read(m_iHandle,pBuffer,ulCount);
    }

    /**
     * Writes raw data to the current file.
     * @param [in] pBuffer A pointer to the beginning of a buffer from which to
     *                     read data to be written to the file.
     * @param [in] ulCount The number of bytes to write to the file.
     * @return If the operation failed -1 is returned, otherwise the function
     *         returns the number of bytes written (this may be zero).
     */
    TInt64 CFile::Write(const void *pBuffer,unsigned long ulCount)
    {
        if (m_iHandle == -1)
            return -1;

        return write(m_iHandle,pBuffer,ulCount);
    }

    /**
     * Checks whether the file exist or not.
     * @return If the file exist true is returned, otherwise false.
     */
    bool CFile::Exist()
    {
        if (m_iHandle != -1)
        {
            struct stat Stat;
            return fstat(m_iHandle,&Stat) == 0;
        }

        return Exist(m_FilePath.c_str());
    }

    /**
     * Deletes the file from the file system. If other links exists to the file
     * only this link will be deleted. If the file is opened it will be closed.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool CFile::Delete()
    {
        Close();

        return unlink(m_FilePath.c_str()) == 0;
    }

    /**
     * Moves the file to use the new file path. If the new full path exist it
     * will not be overwritten. If the file is open it will be closed.
     * @param [in] szNewFilePath The new file path.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool CFile::Rename(const TChar *szNewFilePath)
    {
        // If a file already exist abort so it will not be overwritten. 
        if (Exist(szNewFilePath))
            return false;

        Close();

        if (rename(m_FilePath.c_str(),szNewFilePath) == 0)
        {
            m_FilePath = szNewFilePath;
            return true;
        }

        return false;
    }

    /**
     * Obtains time stamps on when the file was last accessed, last modified
     * and created.
     * @param [out] AccessTime Time of last access.
     * @param [out] ModifyTime Time of last modification.
     * @param [out] CreateTime Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool CFile::Time(struct tm &AccessTime,struct tm &ModifyTime,
        struct tm &CreateTime)
    {
        if (m_iHandle != -1)
        {
            struct stat Stat;
            if (fstat(m_iHandle,&Stat) == -1)
                return false;

            // Convert to local time.
            if (localtime_r(&Stat.st_atime,&AccessTime) == NULL)
                return false;

            if (localtime_r(&Stat.st_mtime,&ModifyTime) == NULL)
                return false;

            if (localtime_r(&Stat.st_ctime,&CreateTime) == NULL)
                return false;

            return true;
        }

        return Time(m_FilePath.c_str(),AccessTime,ModifyTime,CreateTime);
    }

    /**
     * Checks if the active user has permission to open the file in a certain
     * file mode.
     * @param [in] FileMode The file mode to check for access permission.
     * @return If the active user have permission to open the file in the
     *         specified file mode true is returned, otherwise false is
     *         returned.
     */
    bool CFile::Access(EFileMode FileMode)
    {
        return Access(m_FilePath.c_str(),FileMode);
    }

    /**
     * Calcualtes the size of the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    TInt64 CFile::Size()
    {
        // If the file is not open, use the static in this case optimized
        // function.
        if (m_iHandle == -1)
            return Size(m_FilePath.c_str());

        TInt64 iCurPos = Tell();
        TInt64 iFileSize = Seek(0,FILE_END);
        Seek(iCurPos,FILE_BEGIN);

        return iFileSize;
    }

    /**
     * Checks whether the specified file exist or not.
     * @param [in] szFilePath The path to the file.
     * @return If the file exist true is returned, otherwise false.
     */
    bool CFile::Exist(const TChar *szFilePath)
    {
        struct stat Stat;
        return stat(szFilePath,&Stat) == 0;
    }

    /**
     * Deletes the specified file from the file system. If other links exists
     * to the file only the specified link will be deleted.
     * @param [in] szFilePath The path to the file.
     * @return If the file was successfully deleted true is returned, otherwise
     *         false is returned.
     */
    bool CFile::Delete(const TChar *szFilePath)
    {
        return unlink(szFilePath) == 0;
    }

    /**
     * Moves the old file to use the new file path. If the new full path exist
     * it will not be overwritten.
     * @param [in] szOldFilePath The path to the file that should be moved.
     * @param [in] szNewFilePath The new path of the existing file.
     * @return If the file was sucessfully renamed true is returned, otherwise
     *         false is returned.
     */
    bool CFile::Rename(const TChar *szOldFilePath,const TChar *szNewFilePath)
    {
        if (Exist(szNewFilePath))
            return false;

        return rename(szOldFilePath,szNewFilePath) == 0;
    }

    /**
     * Obtains time stamps on when the specified file was last accessed, last
     * modified and created.
     * @param [in] szFilePath The path to the file.
     * @param [out] AccessTime Time of last access.
     * @param [out] ModifyTime Time of last modification.
     * @param [out] CreateTime Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool CFile::Time(const TChar *szFilePath,struct tm &AccessTime,
        struct tm &ModifyTime,struct tm &CreateTime)
    {
        struct stat Stat;
        if (stat(szFilePath,&Stat) == -1)
            return false;

        // Convert to local time.
        if (localtime_r(&Stat.st_atime,&AccessTime) == NULL)
            return false;

        if (localtime_r(&Stat.st_mtime,&ModifyTime) == NULL)
            return false;

        if (localtime_r(&Stat.st_ctime,&CreateTime) == NULL)
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
     * @param szFilePath The path to the file.
     * @param [in] FileMode The file mode to check for access permission.
     * @return If the active user have permission to open the file in the
     *         specified file mode true is returned, otherwise false is
     *         returned.
     */
    bool CFile::Access(const TChar *szFilePath,EFileMode FileMode)
    {
        switch (FileMode)
        {
            case OPEN_READ:
                return access(szFilePath,R_OK) == 0;

            case OPEN_WRITE:
                return access(szFilePath,W_OK) == 0;
        }

        return false;
    }

    /**
     * Calcualtes the size of the specified file.
     * @param [in] szFilePath The path to the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    TInt64 CFile::Size(const TChar *szFilePath)
    {
        struct stat Stat;
        if (stat(szFilePath,&Stat) == -1)
            return -1;

        return Stat.st_size;
    }
};

