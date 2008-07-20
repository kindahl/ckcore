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
 * @file src/types.hh
 * @brief Defines custom types used by the library.
 */
#pragma once
#include "types.hh"

namespace ckCore
{
    class Path
    {
    public:
        class Iterator
        {
        private:
            bool at_end_;
            size_t pos_start_;
            size_t pos_end_;
            const Path *path_;

            void Next();

        public:
            Iterator();
            Iterator(const Path &path);

            tstring operator*() const;
            Iterator &operator++();
            Iterator &operator++(int);
            bool operator==(const Iterator &it);
            bool operator!=(const Iterator &it);
        };

    private:
        tstring path_name_;

    public:
        Path(const tchar *path_name);
        ~Path();

        Iterator Begin();
        Iterator End();

        //bool Valid();
    };
};

