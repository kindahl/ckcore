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
    class File : public FileBase
    {
    private:
        int file_handle_;
        TString file_path_;

    public:
        File(const TChar *file_path);
        ~File();

        // Functions for internal manipulation.
        bool Open(FileMode file_mode);
        bool Close();
        bool Test();
        TInt64 Seek(TInt64 distance,FileWhence whence);
        TInt64 Tell();
        TInt64 Read(void *buffer,unsigned long count);
        TInt64 Write(const void *buffer,unsigned long count);

        // Functions for external manipulation (does not require file to be
        // opened).
        bool Exist();
        bool Delete();
        bool Rename(const TChar *new_file_path);
        bool Time(struct tm &access_time,struct tm &modify_time,
                  struct tm &create_time);
        bool Access(FileMode file_mode);
        TInt64 Size();

        // Static (conveniance and performance) functions, they are not allowed
        // to be wrappers around the non-static functions for performance
        // reasons.
        static bool Exist(const TChar *file_path);
        static bool Delete(const TChar *file_path);
        static bool Rename(const TChar *old_file_path,
                           const TChar *new_file_path);
        static bool Time(const TChar *file_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time);
        static bool Access(const TChar *file_path,FileMode file_mode);
        static TInt64 Size(const TChar *file_path);
    };
};

