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
    Path::Iterator::Iterator() : at_end_(true),pos_start_(-1),pos_end_(0)
    {
    }

    Path::Iterator::Iterator(const Path &path) : at_end_(false),pos_start_(-1),
        pos_end_(0),path_(&path)
    {
        Next();
    }

    void Path::Iterator::Next()
    {
        if (pos_end_ == path_->path_name_.size())
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

    tstring Path::Iterator::operator*() const
    {
        if (at_end_)
            return tstring("NULL");
        else
            return path_->path_name_.substr(pos_start_ + 1,pos_end_ - pos_start_ - 1);
    }

    Path::Iterator &Path::Iterator::operator++()
    {
        pos_start_ = pos_end_;
        Next();

        return *this;
    }

    Path::Iterator &Path::Iterator::operator++(int)
    {
        pos_start_ = pos_end_;
        Next();         

        return *this;
    }

    bool Path::Iterator::operator==(const Iterator &it)
    {
        if (at_end_ && it.at_end_)
            return true;
        else
            return (**this) == *it;
    }

    bool Path::Iterator::operator!=(const Iterator &it)
    {
        if (at_end_ && it.at_end_)
            return false;
        else
            return (**this) != *it;
    }

    Path::Path(const tchar *path_name) : path_name_(path_name)
    {
    }

    Path::~Path()
    {
    }

    Path::Iterator Path::Begin()
    {
        return Path::Iterator(*this);
    }

    Path::Iterator Path::End()
    {
        return Path::Iterator();
    }
};

