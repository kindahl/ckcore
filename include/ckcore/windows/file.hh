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
 * @file src/windows/file.hh
 * @brief Defines the Windows file class.
 */
#pragma once
#include <windows.h>
#include "ckcore/file.hh"
#include "ckcore/types.hh"
#include "ckcore/path.hh"

namespace ckcore
{
    /**
     * @brief The class for dealing with files on Windows.
     */
    class File : public FileBase
    {
    private:
        HANDLE file_handle_;
        Path file_path_;

    public:
        File(const Path &file_path);
        ~File();

		const tstring &Name() const;

        // Functions for internal manipulation.
        bool Open(FileMode file_mode);
        bool Close();
        bool Test() const;
        tint64 Seek(tint64 distance,FileWhence whence);
        tint64 Tell() const;
        tint64 Read(void *buffer,tuint32 count);
        tint64 Write(const void *buffer,tuint32 count);

        // Functions for external manipulation (does not require file to be
        // opened).
        bool Exist() const;
        bool Remove();
        bool Rename(const Path &new_file_path);
        bool Time(struct tm &access_time,struct tm &modify_time,
                  struct tm &create_time) const;
        bool Access(FileMode file_mode) const;
		bool Hidden() const;
        tint64 Size();

        // Static (conveniance and performance) functions, they are not allowed
        // to be wrappers around the non-static functions for performance
        // reasons.
        static bool Exist(const Path &file_path);
        static bool Remove(const Path &file_path);
        static bool Rename(const Path &old_file_path,
                           const Path &new_file_path);
        static bool Time(const Path &file_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time);
        static bool Access(const Path &file_path,FileMode file_mode);
		static bool Hidden(const Path &file_path);
        static tint64 Size(const Path &file_path);
		static File Temp();
    };
};
