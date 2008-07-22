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

#include "path.hh"

namespace ckCore
{
    /**
     * Constructs an Interator object.
     */
    Path::Iterator::Iterator() : at_end_(true),pos_start_(-1),pos_end_(0)
    {
    }

    /**
     * Constructs an Iterator object.
     * @param [in] path A reference to the Path object to iterate over.
     */
    Path::Iterator::Iterator(const Path &path) : at_end_(false),pos_start_(-1),
        pos_end_(0),path_(&path)
    {
        Next();
    }

    void Path::Iterator::Next()
    {
        if (pos_end_ == path_->path_name_.size() ||
            pos_end_ == path_->path_name_.size() - 1)
        {
            at_end_ = true;
            return;
        }
#ifdef _WINDOWS
        size_t delim = path_->path_name_.find_first_of("/\\",pos_start_ + 1);

        if (delim == 0)
        {
            pos_start_++;
            delim = path_->path_name_.find_first_of("/\\",pos_start_ + 1);
        }
#else
        size_t delim = path_->path_name_.find('/',pos_start_ + 1);

        if (delim == 0)
        {
            pos_start_++;
            delim = path_->path_name_.find('/',pos_start_ + 1);
        }
#endif

        if (delim == -1)
            pos_end_ = path_->path_name_.size();
        else
            pos_end_ = delim;
    }

    /**
     * Returns the name of the file or directory that the iterator currently
     * points at.
     * @return The name of the file or directory that the iterator points at.
     */
    tstring Path::Iterator::operator*() const
    {
        if (at_end_)
            return tstring("NULL");
        else
            return path_->path_name_.substr(pos_start_ + 1,pos_end_ - pos_start_ - 1);
    }

    /**
     * Moves the iterator to the next file or directory in the path name.
     * @return An Iterator object pointing at the next file or directory name.
     */
    Path::Iterator &Path::Iterator::operator++()
    {
        pos_start_ = pos_end_;
        Next();

        return *this;
    }

    /**
     * Moves the iterator to the next file or directory in the path name.
     * @return An Iterator object pointing at the next file or directory name.
     */
    Path::Iterator &Path::Iterator::operator++(int)
    {
        pos_start_ = pos_end_;
        Next();         

        return *this;
    }

    /**
     * Compares to iterators.
     * @return If the iterators are equal true is returned, otherwise false.
     */
    bool Path::Iterator::operator==(const Iterator &it)
    {
        if (at_end_ && it.at_end_)
            return true;
        else
            return (**this) == *it;
    }

    /**
     * Compares two iterators.
     * @return If the iterators are not equal true is returned, otherwise
     *         false.
     */
    bool Path::Iterator::operator!=(const Iterator &it)
    {
        if (at_end_ && it.at_end_)
            return false;
        else
            return (**this) != *it;
    }

    /**
     * Constructs a Path object.
     * @param [in] path_name The path name.
     */
    Path::Path(const tchar *path_name) : path_name_(path_name)
    {
    }

    /**
     * Destructs the Path object.
     */
    Path::~Path()
    {
    }

    /**
     * Creates an iterator pointing at the beginning of the path name.
     * @return An Iterator object pointing at the beginning of the path name.
     */
    Path::Iterator Path::Begin() const
    {
        return Path::Iterator(*this);
    }

    /**
     * Creates and iterator pointing at the end of the path name.
     * @return An Iterator object pointing at the end of the path name.
     */
    Path::Iterator Path::End() const
    {
        return Path::Iterator();
    }

    /**
     * Checks if the path name is valid, that it does not contain any invalid
     * characters.
     * @return If the path name is valid true is returned, otherwise false.
     */
    bool Path::Valid()
    {
#ifdef _WINDOWS
        for (size_t i = 0; i < path_name_.size(); i++)
        {
            switch (path_name_[i])
            {
                case ':':
                    if (i != 1)
                        return false;
                    break;

                case '*':
                case '?':
                case '<':
                case '>':
                case '|':
                case '"':
                    return false;
            }
        }
#endif
        return true;
    }

    /**
     * Calculates the dir name of the path name. The dir name will contain a
     * trailing path delimiter.
     * @return A string containing the dir name of the path name.
     */
    tstring Path::DirName()
    {
        size_t end = path_name_.size() - 1;
#ifdef _WINDOWS
        size_t delim = path_name_.find_last_of("/\\");
        if (delim == end)
            delim = path_name_.find_last_of("/\\",end - 1);
#else
        size_t delim = path_name_.rfind('/');
        if (delim == end)
            delim = path_name_.rfind('/',end - 1);
#endif
        if (delim == -1)
            return tstring("");
        else
            return path_name_.substr(0,delim + 1);
    }

    /**
     * Calculates the base name of the path name.
     * @return A string containing the base name of the path name.
     */
    tstring Path::BaseName()
    {
        size_t end = path_name_.size() - 1;
#ifdef _WINDOWS
        size_t delim = path_name_.find_last_of("/\\");
        if (delim == end)
            delim = path_name_.find_last_of("/\\",--end);
#else
        size_t delim = path_name_.rfind('/');
        if (delim == end)
            delim = path_name_.rfind('/',--end);
#endif

        if (delim == -1)
            return path_name_.substr(0,end + 1);
        else
            return path_name_.substr(delim + 1,end - delim);
    }

    /**
     * Compares two paths.
     * @return If the two Path objects are equal true is returned, otherwise
     *         false.
     */
    bool Path::operator==(const Path &p)
    {
        Iterator it1 = Begin();
        Iterator it2 = p.Begin();

        while (it1 != End() && it2 != p.End())
        {
            if (it1 != it2)
                return false;

            it1++,it2++;
        }

        return it1 == End() && it2 == p.End();
    }

    /**
     * Compares two paths.
     * @return If the two Path objects are not equal true is returned,
     *         otherwise false.
     */
    bool Path::operator!=(const Path &p)
    {
        return !(*this == p);
    }
};

