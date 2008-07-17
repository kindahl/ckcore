/*
 * Copyright (C) 2006-2008 Christian Kindahl, christian dot kindahl at gmail dot com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

        // Functions for external manipulation (does not require file to be
        // opened).
        bool Exist();
        bool Delete();
        bool Rename(const TChar *szNewFilePath);
        TInt64 Size();

        // Static (conveniance and performance) functions, they are not allowed
        // to be wrappers around the non-static functions for performance
        // reasons.
        static bool Exist(const TChar *szFilePath);
        static bool Delete(const TChar *szFilePath);
        static bool Rename(const TChar *szOldFilePath,const TChar *szNewFilePath);
        static TInt64 Size(const TChar *szFilePath);

        //static void Attributes(const TChar *szFilePath) {}
    };
};

