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
 * @file src/linux/directory.hh
 * @brief Defines the Linux file class.
 */
#pragma once
#include "../file.hh"
#include "../types.hh"

namespace ckCore
{
    /**
     * @brief The class for dealing with directories on Linux.
     */
    class Directory
    {
    private:
        tstring dir_path_;

    public:
        Directory(const tchar *dir_path);
        ~Directory();

        bool Create();
        bool Remove();
    };
};

