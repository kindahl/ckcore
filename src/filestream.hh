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
 * @file src/filestream.hh
 * @brief Implementation of stream interfaces for dealing with files.
 */

#pragma once
#include "types.hh"
#include "stream.hh"
#include "file.hh"
#include "path.hh"

namespace ckCore
{
    /**
     * @brief Stream class for reading files.
     */
    class FileInStream : public InStream
    {
    private:
        File file_;
        tint64 size_;
        tint64 read_;

    public:
        FileInStream(const Path &file_path);

        bool Open();
        bool Close();
        bool Eos();

        tint64 Read(void *buffer,unsigned long count);
    };

    /**
     * @brief Stream class for writing files.
     */
    class FileOutStream : public OutStream
    {
    private:
        File file_;

    public:
        FileOutStream(const Path &file_path);

        bool Open();
        bool Close();

        tint64 Write(void *buffer,unsigned long count);
    };
};

