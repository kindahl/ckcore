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
 * @file include/ckcore/dynlib.hh
 * @brief Defines a class for managing dynamic libraries.
 */

#pragma once
#ifdef _WINDOWS
#include <windows.h>
#endif
#include "ckcore/types.hh"
#include "ckcore/path.hh"

namespace ckcore
{
    /**
     * @brief Class for managing dynamic libraries.
     */
    class DynamicLibrary
    {
    private:
        Path path_;

#ifdef _WINDOWS
        HINSTANCE handle_;
#else
        void *handle_;
#endif

    public:
        DynamicLibrary(const Path &path);
        ~DynamicLibrary();

        bool open();
        bool close();
        bool test();
        void *symbol(const char *symbol_name) const;

        const Path &path() const;
    };
};

