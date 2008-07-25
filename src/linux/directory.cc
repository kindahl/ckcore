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
#include "directory.hh"

namespace ckCore
{
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
        return rmdir(dir_path_.Name().c_str()) == 0;
    }

    /**
     * Tests if the current directory exist.
     * @return If the directory exist true is returned, otherwise false.
     */
    bool Directory::Exist() const
    {
        struct stat file_stat;
        return stat(dir_path_.Name().c_str(),&file_stat) == 0;
    }

    /**
     * Obtains time stamps on when the current directory was last accessed,
     * last modified and created.
     * @param [out] access_time Time of last access.
     * @param [out] modify_time Time of last modification.
     * @param [out] create_time Time of creation (last status change on Linux).
     * @return If successfull true is returned, otherwise false.
     */
    bool Directory::Time(struct tm &access_time,struct tm &modify_time,
                         struct tm &create_time) const
    {
        return Time(dir_path_,access_time,modify_time,create_time);
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
            cur_path += *it + "/";
            if (!Exist(cur_path.c_str()))
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
    bool Directory::Remove(const Path &dir_path)
    {
        return rmdir(dir_path.Name().c_str()) == 0;
    }

    /**
     * Tests if the specified directory exist.
     * @return If the directory exist true is returned, otherwise false.
     */
    bool Directory::Exist(const Path &dir_path)
    {
        struct stat file_stat;
        return stat(dir_path.Name().c_str(),&file_stat) == 0;
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
    bool Directory::Time(const Path &dir_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time)
    {
        struct stat dir_stat;
        if (stat(dir_path.Name().c_str(),&dir_stat) == -1)
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
};

