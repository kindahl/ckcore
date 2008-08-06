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

#include "stdafx.h"
#include <iostream>
#include "directory.hh"
#include "util.hh"

namespace ckCore
{
    /**
     * Constructs an Iterator object.
     */
    Directory::Iterator::Iterator() : dir_handle_(INVALID_HANDLE_VALUE),
		at_end_(true)
    {
    }

    /**
     * Constructs an Iterator object.
     * @param [in] dir A reference to the Directory object to iterate over.
     */
    Directory::Iterator::Iterator(const Directory &dir) :
		dir_handle_(INVALID_HANDLE_VALUE),at_end_(false)
    {
        if (dir.dir_handles_.count(this) > 0)
        {
            dir_handle_ = const_cast<Directory &>(dir).dir_handles_[this];
        }
        else
        {
			tstring path = dir.dir_path_.Name();
			tchar last = path[path.size() - 1];

			// Make sure we have a trailing delimiter.
			if (last != '/' && last != '\\')
				path += '/';
			
			path += '*';

			dir_handle_ = FindFirstFile(path.c_str(),&cur_ent_);
            const_cast<Directory &>(dir).dir_handles_[this] = dir_handle_;
        }
		
		if (dir_handle_ == INVALID_HANDLE_VALUE)
		{
			at_end_ = true;
		}
		else
		{
			// Skip system '.' and '..' directories.
			if (!lstrcmp(cur_ent_.cFileName,ckT(".")) ||
				!lstrcmp(cur_ent_.cFileName,ckT("..")))
			{
				Next();
			}
		}
    }

    void Directory::Iterator::Next()
    {
		while (!(at_end_ = !FindNextFile(dir_handle_,&cur_ent_)))
		{
			// Skip system '.' and '..' directories.
			if (lstrcmp(cur_ent_.cFileName,ckT(".")) &&
				lstrcmp(cur_ent_.cFileName,ckT("..")))
			{
				break;
			}
		}
    }

    /**
     * Returns the name of the file or directory that the iterator currently
     * points at.
     * @return The name of the file or directory that the iterator points at.
     */
    tstring Directory::Iterator::operator*() const
    {
        if (at_end_)
            return tstring(ckT("NULL"));
        else
            return tstring(cur_ent_.cFileName);
    }

    /**
     * Moves the iterator to the next file or directory residing in the
     * directory.
     * @return An Iterator object pointing at the next file or directory.
     */
    Directory::Iterator &Directory::Iterator::operator++()
    {
        if (!at_end_)
            Next();

        return *this;
    }

    /**
     * Moves the iterator to the next file or directory residing in the
     * directory.
     * @return An Iterator object pointing at the next file or directory.
     */
    Directory::Iterator &Directory::Iterator::operator++(int)
    {
        if (!at_end_)
            Next();

        return *this;
    }

    /**
     * Tests the equivalence of this and another iterator.
     * @param [in] it The iterator to use for comparison.
     * @return If the iterators are equal true is returned, otherwise false.
     */
    bool Directory::Iterator::operator==(const Iterator &it) const
    {
        if (at_end_ && it.at_end_)
            return true;

        if ((at_end_ && !it.at_end_) ||
            (!at_end_ && it.at_end_))
            return false;

        return !lstrcmp(cur_ent_.cFileName,it.cur_ent_.cFileName);
    }

    /**
     * Tests the non-equivalence of this and another iterator.
     * @param [in] it The iterator to use for comparison.
     * @return If the iterators are equal true is returned, otherwise false.
     */
    bool Directory::Iterator::operator!=(const Iterator &it) const
    {
        return !(*this == it);
    }

    /**
     * Constructs a Directory object.
     * @param [in] dir_path The path to the directory.
     */
    Directory::Directory(const Path &dir_path) : dir_path_(dir_path)
    {
    }

    /**
     * Destructs the Directory object.
     */
    Directory::~Directory()
    {
        // Since the Directory object owns the iterator handles, we need to
        // free them.
        std::map<Iterator *,HANDLE>::iterator it;
        for (it = dir_handles_.begin(); it != dir_handles_.end(); it++)
            FindClose(it->second);

        dir_handles_.clear();
    }

    /**
     * Creates an iterator pointing to the first file or directory in the
     * current directory.
     * @return An Iteator object pointing to the first file or directory.
     */
    Directory::Iterator Directory::Begin() const
    {
        return Directory::Iterator(*this);
    }

    /**
     * Creats an iterator poiting beyond the last file or directory in the
     * directory in the current directory.
     * @return An Iteator object pointing beyond the last file or directory.
     */
    Directory::Iterator Directory::End() const
    {
        return Directory::Iterator();
    }

    /**
     * Creates the directory unless it already exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Create() const
    {
        return Create(dir_path_);
    }

    /**
     * Removes the directory if it exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Remove() const
    {
		return RemoveDirectory(dir_path_.Name().c_str()) != FALSE;
    }

    /**
     * Tests if the current directory exist.
     * @return If the directory exist true is returned, otherwise false.
     */
    bool Directory::Exist() const
    {
		unsigned long attr = GetFileAttributes(dir_path_.Name().c_str());

		return (attr != -1) && (attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    /**
     * Obtains time stamps on when the current directory was last accessed,
     * last modified and created.
     * @param [out] accessckTime Time of last access.
     * @param [out] modifyckTime Time of last modification.
     * @param [out] createckTime Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Time(struct tm &accessckTime,struct tm &modifyckTime,
                         struct tm &createckTime) const
    {
		return Time(dir_path_,accessckTime,modifyckTime,createckTime);
    }

    /**
     * Creates the specified directory unless it already exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Create(const Path &dir_path)
    {
        tstring cur_path = dir_path.RootName();

        Path::Iterator it;
        for (it = dir_path.Begin(); it != dir_path.End(); it++)
        {
            cur_path += *it + ckT("/");
            if (!Exist(cur_path.c_str()))
            {
				if (CreateDirectory(cur_path.c_str(),NULL) == FALSE)
                    return false;
            }
        }

        return true;
    }

    /**
     * Removes the specified directory if it exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Remove(const Path &dir_path)
    {
		return RemoveDirectory(dir_path.Name().c_str()) != FALSE;
    }

    /**
     * Tests if the specified directory exist.
     * @return If the directory exist true is returned, otherwise false.
     */
    bool Directory::Exist(const Path &dir_path)
    {
		unsigned long attr = GetFileAttributes(dir_path.Name().c_str());

		return (attr != -1) && (attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    /**
     * Obtains time stamps on when the specified directory was last accessed,
     * last modified and created.
     * @param [in] dir_path The path to the directory.
     * @param [out] accessckTime Time of last access.
     * @param [out] modifyckTime Time of last modification.
     * @param [out] createckTime Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Time(const Path &dir_path,struct tm &accessckTime,
                         struct tm &modifyckTime,struct tm &createckTime)
    {
		WIN32_FILE_ATTRIBUTE_DATA file_info;
		if (GetFileAttributesEx(dir_path.Name().c_str(),GetFileExInfoStandard,
								&file_info) == FALSE)
		{
			return false;
		}

		// Convert to system time.
		SYSTEMTIME access_stime,modify_stime,create_stime;

		if (FileTimeToSystemTime(&file_info.ftLastAccessTime,&access_stime) == FALSE)
			return false;

		if (FileTimeToSystemTime(&file_info.ftLastWriteTime,&modify_stime) == FALSE)
			return false;

		if (FileTimeToSystemTime(&file_info.ftCreationTime,&create_stime) == FALSE)
			return false;

		// Convert to struct tm.
		SysTimeToTm(access_stime,accessckTime);
		SysTimeToTm(modify_stime,modifyckTime);
		SysTimeToTm(create_stime,createckTime);

		return true;
    }
};
