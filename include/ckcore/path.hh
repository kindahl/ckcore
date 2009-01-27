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
 * @file include/ckcore/path.hh
 * @brief Defines the path name class.
 */
#pragma once
#include <vector>
#include "ckcore/types.hh"

namespace ckcore
{
    /**
     * @brief Class for dealing with path names.
     */
    class Path
    {
    public:
        /**
         * @brief Class for iterating the file/directory structure of a path
         *        name.
         */
        class Iterator
        {
        private:
            bool at_end_;
            size_t pos_start_;
            size_t pos_end_;
            const Path *path_;

            void next();

        public:
            Iterator();
            Iterator(const Path &path);

            tstring operator*() const;
            Iterator &operator++();
            Iterator &operator++(int);
            bool operator==(const Iterator &it) const;
            bool operator!=(const Iterator &it) const;
        };

    private:
        tstring path_name_;

    public:
		Path();
        Path(const tchar *path_name);
        ~Path();

        Iterator begin() const;
        Iterator end() const;

        bool valid() const;
        const tstring &name() const;
        tstring root_name() const;
        tstring dir_name() const;
        tstring base_name() const;
        tstring ext_name() const;

        bool operator==(const Path &p) const;
        bool operator!=(const Path &p) const;
        Path &operator=(const Path &p);
        Path operator+(const Path &p);
        Path &operator+=(const Path &p);
    };
};

