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

/**
 * @file src/linux/file.hh
 * @brief Defines the Linux file class.
 */
#pragma once
#include "../file.hh"
#include "../types.hh"

namespace ckCore
{
    /**
     * @brief The class for dealing with files on Linux.
     */
    class CFile : public CFileBase
    {
    private:
        int m_iHandle;
        TString m_FilePath;

    public:
        CFile(const TChar *szFilePath);
        ~CFile();

        // Functions for internal manipulation.
        bool Open(EFileMode FileMode);
        bool Close();
        bool Test();
        TInt64 Seek(TInt64 iDistance,EFileWhence FileWhence);
        TInt64 Tell();
        TInt64 Read(void *pBuffer,unsigned long ulCount);
        TInt64 Write(const void *pBuffer,unsigned long ulCount);

        // Functions for external manipulation (does not require file to be
        // opened).
        bool Exist();
        bool Delete();
        bool Rename(const TChar *szNewFilePath);
        bool Time(struct tm &AccessTime,struct tm &ModifyTime,
            struct tm &CreateTime);
        bool Access(EFileMode FileMode);
        TInt64 Size();

        // Static (conveniance and performance) functions, they are not allowed
        // to be wrappers around the non-static functions for performance
        // reasons.
        static bool Exist(const TChar *szFilePath);
        static bool Delete(const TChar *szFilePath);
        static bool Rename(const TChar *szOldFilePath,
            const TChar *szNewFilePath);
        static bool Time(const TChar *szFilePath,struct tm &AccessTime,
            struct tm &ModifyTime,struct tm &CreateTime);
        static bool Access(const TChar *szFilePath,EFileMode FileMode);
        static TInt64 Size(const TChar *szFilePath);

        //static void Attributes(const TChar *szFilePath) {}
    };
};

