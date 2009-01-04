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

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "ckcore/convert.hh"
#include "ckcore/directory.hh"

namespace ckcore
{
    /**
     * Constructs an Iterator object.
     */
    Directory::Iterator::Iterator() : dir_handle_(NULL),cur_ent_(NULL)
    {
    }

    /**
     * Constructs an Iterator object.
     * @param [in] dir A reference to the Directory object to iterate over.
     */
    Directory::Iterator::Iterator(const Directory &dir) : dir_handle_(NULL),
        cur_ent_(NULL)
    {
        if (dir.dir_handles_.count(this) > 0)
        {
            dir_handle_ = const_cast<Directory &>(dir).dir_handles_[this];
        }
        else
        {
            dir_handle_ = opendir(dir.dir_path_.name().c_str());
            const_cast<Directory &>(dir).dir_handles_[this] = dir_handle_;
        }

        if (dir_handle_ != NULL)
            next();
    }

    void Directory::Iterator::next()
    {
        cur_ent_ = readdir(dir_handle_);

        // Skip system '.' and '..' directories.
        while ((cur_ent_ != NULL) && (!strcmp(cur_ent_->d_name,".") ||
                                      !strcmp(cur_ent_->d_name,"..")))
        {
            cur_ent_ = readdir(dir_handle_);
        }
    }

    /**
     * Returns the name of the file or directory that the iterator currently
     * points at.
     * @return The name of the file or directory that the iterator points at.
     */
    tstring Directory::Iterator::operator*() const
    {
        if (cur_ent_ == NULL)
            return tstring("NULL");
        else
            return tstring(cur_ent_->d_name);
    }

    /**
     * Moves the iterator to the next file or directory residing in the
     * directory.
     * @return An Iterator object pointing at the next file or directory.
     */
    Directory::Iterator &Directory::Iterator::operator++()
    {
        if (cur_ent_ != NULL)
            next();

        return *this;
    }

    /**
     * Moves the iterator to the next file or directory residing in the
     * directory.
     * @return An Iterator object pointing at the next file or directory.
     */
    Directory::Iterator &Directory::Iterator::operator++(int)
    {
        if (cur_ent_ != NULL)
            next();

        return *this;
    }

    /**
     * Tests the equivalence of this and another iterator.
     * @param [in] it The iterator to use for comparison.
     * @return If the iterators are equal true is returned, otherwise false.
     */
    bool Directory::Iterator::operator==(const Iterator &it) const
    {
        if (cur_ent_ == NULL && it.cur_ent_ == NULL)
            return true;

		if ((cur_ent_ == NULL && it.cur_ent_ != NULL) ||
            (cur_ent_ != NULL && it.cur_ent_ == NULL))
            return false;

        return !strcmp(cur_ent_->d_name,it.cur_ent_->d_name);
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
        std::map<Iterator *,DIR *>::iterator it;
        for (it = dir_handles_.begin(); it != dir_handles_.end(); it++)
            closedir(it->second);

        dir_handles_.clear();
    }

	/**
	 * Returns the full directory path name.
	 * @return The full directory path name.
	 */
	const tstring &Directory::name() const
	{
		return dir_path_.name();
	}

    /**
     * Creates an iterator pointing to the first file or directory in the
     * current directory.
     * @return An Iteator object pointing to the first file or directory.
     */
    Directory::Iterator Directory::begin() const
    {
        return Directory::Iterator(*this);
    }

    /**
     * Creats an iterator poiting beyond the last file or directory in the
     * directory in the current directory.
     * @return An Iteator object pointing beyond the last file or directory.
     */
    Directory::Iterator Directory::end() const
    {
        return Directory::Iterator();
    }

    /**
     * Creates the directory unless it already exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::create() const
    {
        return create(dir_path_);
    }

    /**
     * Removes the directory if it exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::remove() const
    {
        return rmdir(dir_path_.name().c_str()) == 0;
    }

    /**
     * Tests if the current directory exist.
     * @return If the directory exist true is returned, otherwise false.
     */
    bool Directory::exist() const
    {
        struct stat file_stat;
        if (stat(dir_path_.name().c_str(),&file_stat) != 0)
            return false;

        return (file_stat.st_mode & S_IFDIR) > 0;
    }

    /**
     * Obtains time stamps on when the current directory was last accessed,
     * last modified and created.
     * @param [out] access_time Time of last access.
     * @param [out] modify_time Time of last modification.
     * @param [out] create_time Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::time(struct tm &access_time,struct tm &modify_time,
                         struct tm &create_time) const
    {
        return time(dir_path_,access_time,modify_time,create_time);
    }

    /**
     * Creates the specified directory unless it already exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::create(const Path &dir_path)
    {
        tstring cur_path = dir_path.root_name();

        Path::Iterator it;
        for (it = dir_path.begin(); it != dir_path.end(); it++)
        {
            cur_path += *it + "/";
            if (!exist(cur_path.c_str()))
            {
                if (mkdir(cur_path.c_str(),S_IRUSR | S_IWUSR | S_IXUSR) != 0)
                    return false;
            }
        }

        return true;
    }

    /**
     * Removes the specified directory if it exist.
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::remove(const Path &dir_path)
    {
        return rmdir(dir_path.name().c_str()) == 0;
    }

    /**
     * Tests if the specified directory exist.
     * @return If the directory exist true is returned, otherwise false.
     */
    bool Directory::exist(const Path &dir_path)
    {
        struct stat file_stat;
        if (stat(dir_path.name().c_str(),&file_stat) != 0)
            return false;

        return (file_stat.st_mode & S_IFDIR) > 0;
    }

    /**
     * Obtains time stamps on when the specified directory was last accessed,
     * last modified and created.
     * @param [in] dir_path The path to the directory.
     * @param [out] access_time Time of last access.
     * @param [out] modify_time Time of last modification.
     * @param [out] create_time Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::time(const Path &dir_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time)
    {
        struct stat dir_stat;
        if (stat(dir_path.name().c_str(),&dir_stat) == -1)
            return false;

        // Convert to local time.
        if (localtime_r(&dir_stat.st_atime,&access_time) == NULL)
            return false;

        if (localtime_r(&dir_stat.st_mtime,&modify_time) == NULL)
            return false;

        if (localtime_r(&dir_stat.st_ctime,&create_time) == NULL)
            return false;

        return true;
    }

    /**
     * Creates a Directory object describing a temporary directory on the hard
     * drive. The directory path is pointing to an unique directory name in the
     * default temporary directory of the current system. The directory is not
     * automatically created.
     * @return Directory object to a temporary directory.
     */
	Directory Directory::temp()
	{
		tchar *tmp_name = tmpnam(NULL);
        if (tmp_name != NULL)
        {
            return Directory(tmp_name);
        }
        else
        {
            tchar tmp_name2[260];
            strcpy(tmp_name2,ckT("/tmp/file"));
            strcat(tmp_name2,convert::ui32_to_str(rand()));
            strcat(tmp_name2,ckT(".tmp"));

            return Directory(tmp_name2);
        }
	}
};

