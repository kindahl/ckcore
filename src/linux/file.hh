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
        tstring file_path_;

    public:
        File(const tchar *file_path);
        ~File();

        // Functions for internal manipulation.
        bool Open(FileMode file_mode);
        bool Close();
        bool Test();
        tint64 Seek(tint64 distance,FileWhence whence);
        tint64 Tell();
        tint64 Read(void *buffer,unsigned long count);
        tint64 Write(const void *buffer,unsigned long count);

        // Functions for external manipulation (does not require file to be
        // opened).
        bool Exist();
        bool Remove();
        bool Rename(const tchar *new_file_path);
        bool Time(struct tm &access_time,struct tm &modify_time,
                  struct tm &create_time);
        bool Access(FileMode file_mode);
        tint64 Size();

        // Static (conveniance and performance) functions, they are not allowed
        // to be wrappers around the non-static functions for performance
        // reasons.
        static bool Exist(const tchar *file_path);
        static bool Remove(const tchar *file_path);
        static bool Rename(const tchar *old_file_path,
                           const tchar *new_file_path);
        static bool Time(const tchar *file_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time);
        static bool Access(const tchar *file_path,FileMode file_mode);
        static tint64 Size(const tchar *file_path);
    };
};

