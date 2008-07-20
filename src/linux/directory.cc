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
    Directory::Directory(const tchar *dir_path) : dir_path_(dir_path)
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
     * @return If successfully true is returned, otherwise false.
     */
    bool Directory::Create()
    {
        return mkdir(dir_path_.c_str(),S_IRUSR | S_IWUSR | S_IXUSR) == 0;
    }

    /**
     * Removes the directory if it exist.
     * @return If successfully true is returned, otherwise false.
     */
    bool Directory::Remove()
    {
        return rmdir(dir_path_.c_str()) == 0;
    }
};

