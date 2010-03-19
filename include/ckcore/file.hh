/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2009 Christian Kindahl
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file include/ckcore/file.hh
 * @brief Includes the platform specific file class.
 */

#pragma once

#ifdef _WINDOWS

#include <windows.h>

#elif defined(_UNIX)

// Nothing special for Unix.

#else
#error "Unknown platform"
#endif

#include <ckcore/types.hh>
#include <ckcore/path.hh>
#include <ckcore/exception.hh>

namespace ckcore
{

#ifdef _WINDOWS
#pragma warning( push )
#pragma warning( disable : 4290 )  // C++ exception specification ignored except to ...
#endif

    /**
     * @brief The class for dealing with files on Windows.
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

#ifdef _WINDOWS
        HANDLE file_handle_;
#else
        int file_handle_;
#endif

        Path file_path_;

    public:
        File(const Path &file_path);
       ~File() { close(); }

        const tstring &name() const { return file_path_.name(); }

        // Functions for internal manipulation.
        bool open(FileMode file_mode) throw();
        void open2(FileMode file_mode) throw(std::exception);
        bool close();
        bool test() const;
        tint64 seek(tint64 distance,FileWhence whence) throw();
        tint64 seek2(tint64 distance,FileWhence whence) throw(std::exception);
        tint64 tell() const throw();
        tint64 tell2() const throw(std::exception);
        tint64 read(void *buffer,tint64 count);
        tint64 write(const void *buffer,tint64 count);

        // Functions for external manipulation (does not require file to be
        // opened).
        bool exist() const;
        bool remove();
        bool rename(const Path &new_file_path);
        bool time(struct tm &access_time,struct tm &modify_time,
                  struct tm &create_time) const;
        bool access(FileMode file_mode) const { return access(file_path_,file_mode); }
        bool hidden() const { return hidden(file_path_); }
        tint64 size() throw();
        tint64 size2() throw(std::exception);

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
        static tint64 size(const Path &file_path) throw();
        static tint64 size2(const Path &file_path) throw(std::exception);
		static File temp(const tchar *prefix);
		static File temp(const Path &file_path,const tchar *prefix);

	private:
		void check_file_is_open ( void ) const throw(std::exception);
    };


    /**
     * Opens the file in the requested mode.
     * @param [in] file_mode Determines how the file should be opened. In write
     *                       mode the file will be created if it does not
     *                       exist.
     * @return If successfull true is returned, otherwise false.
     */

    inline bool File::open(FileMode file_mode) throw()
    {
        try
        {
            open2(file_mode);
        }
        catch ( ... )
        {
            return false;
        }

        return true;
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
    inline tint64 File::seek(tint64 distance,FileWhence whence) throw()
    {
      try
      {
        return seek2(distance, whence);
      }
      catch ( ... )
      {
        return -1;
      }
    }

    /**
     * Calculates the current file pointer position in the file.
     * @return If successfull the current file pointer position, otherwise -1
     *         is returned.
     */
    inline tint64 File::tell() const throw()
    {
      try
      {
        return tell2();
      }
      catch ( ... )
      {
        return -1;
      }
    }

    /**
     * Calculates the size of the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    inline tint64 File::size() throw()
    {
        try
        {
            return size2();
        }
        catch ( ... )
        {
            return -1;
        }
    }

    /**
     * Calculates the size of the specified file.
     * @param [in] file_path The path to the file.
     * @return If successfull the size of the file, otherwise -1 is returned.
     */
    inline tint64 File::size(const Path &file_path) throw()
    {
      try
      {
        return size2(file_path);
      }
      catch ( ... )
      {
        return -1;
      }
    }

	inline void File::check_file_is_open ( void ) const throw(std::exception)
	{
        if (!test())
		{
          throw Exception2( ckT("File not yet opened.") );
		}
	}

#ifdef _WINDOWS
#pragma warning( pop )
#endif

};