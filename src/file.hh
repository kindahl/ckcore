/*
 * Copyright (C) 2006-2008 Christian Kindahl, christian dot kindahl at gmail dot com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file src/file.hh
 * @brief Defines the virtual file base class.
 */

#pragma once

namespace ckCore
{
    /**
     * @brief The virtual base class for using files.
     */
    class CFileBase
    {
    public:
        /**
         * Defines modes which describes how to open files.
         */
        enum EFileMode
        {
            OPEN_READ,
            OPEN_WRITE
        };

        /**
         * Defines directives what to use as base offset when performing seek
         * operations.
         */
        enum EFileWhence
        {
            FILE_CURRENT,
            FILE_BEGIN,
            FILE_END
        };
    };
};

#ifdef _WINDOWS
#include "windows/file.hh"
#endif

#ifdef _LINUX
#include "linux/file.hh"
#endif

