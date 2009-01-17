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
 * @brief Defines the virtual file base class.
 */

#pragma once

namespace ckcore
{
    /**
     * @brief The virtual base class for using files.
     */
    class FileBase
    {
    public:
        virtual ~FileBase() {};

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
    };
};

#ifdef _WINDOWS
#include "ckcore/windows/file.hh"
#endif

#ifdef _UNIX
#include "ckcore/unix/file.hh"
#endif

