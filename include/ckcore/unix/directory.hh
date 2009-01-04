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
 * @file include/ckcore/unix/directory.hh
 * @brief Defines the Unix directory class.
 */
#pragma once
#include <map>
#include <dirent.h>
#include "ckcore/file.hh"
#include "ckcore/path.hh"

namespace ckcore
{
    /**
     * @brief The class for dealing with directories on Unix.
     */
    class Directory
    {
    public:
        /**
         * @brief Class for iterating directory contents.
         *
         * Please note that each iterator (except for end iterators) allocates
         * an instance to the directory which will not be released until the
         * Directory (not Iterator) object is destroyed. In other words, a good
         * way of abusing this library is to create lots of iteators while
         * keeping the directory object alive.
         */
        class Iterator
        {
        private:
            DIR *dir_handle_;
            struct dirent *cur_ent_;

            void next();

        public:
            Iterator();
            Iterator(const Directory &dir);

            tstring operator*() const;
            Iterator &operator++();
            Iterator &operator++(int);
            bool operator==(const Iterator &it) const;
            bool operator!=(const Iterator &it) const;
        };

    private:
        Path dir_path_;

        std::map<Iterator *,DIR *> dir_handles_;

    public:
        Directory(const Path &dir_path);
        ~Directory();

		const tstring &name() const;

        Iterator begin() const;
        Iterator end() const;

        bool create() const;
        bool remove() const;
        bool exist() const;
        bool time(struct tm &access_time,struct tm &modify_time,
                  struct tm &create_time) const;

        static bool create(const Path &dir_path);
        static bool remove(const Path &dir_path);
        static bool exist(const Path &dir_path);
        static bool time(const Path &dir_path,struct tm &access_time,
                         struct tm &modify_time,struct tm &create_time);
		static Directory temp();
    };
};

