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

/**
 * @file include/ckcore/unix/file.hh
 * @brief Defines the Unix file class.
 */
#pragma once
#include "ckcore/file.hh"
#include "ckcore/types.hh"
#include "ckcore/path.hh"

namespace ckcore
{
    /**
     * @brief The class for dealing with files on Unix.
     */
    class File
    {
    public:
        /**
         * Defines modes which describes how to open files.
         */
        enum FileMode
        {
            ckOPEN_READ,
            ckOPEN_WRITE,
			ckOPEN_READWRITE
        };

        /**
         * Defines directives what to use as base offset when performing seek
         * operations.
         */
        enum FileWhence
        {
            ckFILE_CURRENT,
            ckFILE_BEGIN,
			ckFILE_END
        };

    private:
        int file_handle_;
        Path file_path_;

    public:
        File(const Path &file_path);
        ~File();

		const tstring &name() const;

        // Functions for internal manipulation.
        bool open(FileMode file_mode);
        bool close();
        bool test() const;
        tint64 seek(tint64 distance,FileWhence whence);
        tint64 tell() const;
        tint64 read(void *buffer,unsigned long count);
        tint64 write(const void *buffer,unsigned long count);

        // Functions for external manipulation (does not require file to be
        // opened).
        bool exist() const;
        bool remove();
        bool rename(const Path &new_file_path);
        bool time(struct tm &access_time,struct tm &modify_time,
                  struct tm &create_time) const;
        bool access(FileMode file_mode) const;
		bool hidden() const;
        tint64 size();

        // Static (conveniance and performance) functions, they are not allowed
        // to be wrappers around the non-static functions for performance
        // reasons.
        static bool exist(const Path &file_path);
        static bool remove(const Path &file_path);
        static bool rename(const Path &old_file_path,
                           const Path &new_file_path);
        static bool time(const Path &file_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time);
        static bool access(const Path &file_path,FileMode file_mode);
		static bool hidden(const Path &file_path);
        static tint64 size(const Path &file_path);
		static File temp(const tchar *prefix);
		static File temp(const Path &file_path,const tchar *prefix);
    };
};

