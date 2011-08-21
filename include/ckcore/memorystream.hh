/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2011 Christian Kindahl
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
 * @file include/ckcore/memorystream.hh
 * @brief In-memory implementation of stream interfaces.
 */

#pragma once
#include "ckcore/types.hh"
#include "ckcore/stream.hh"

namespace ckcore
{
    /**
     * @brief In-memory stream class for reading streams.
     */
    class MemoryInStream : public InStream
    {
    private:
        unsigned char *data_;
        tuint32 count_;
        tuint32 pos_;

    public:
        MemoryInStream(unsigned  char *data,tuint32 count);
        virtual ~MemoryInStream();

        bool end();
        bool seek(tuint32 distance,StreamWhence whence);

        tint64 read(void *buffer,tuint32 count);
        tint64 size();
    };

    /**
     * @brief In-memory stream class for reading streams.
     */
    class MemoryOutStream : public OutStream
    {
    private:
        unsigned char *buffer_;
        tuint32 buffer_size_;
        tuint32 buffer_pos_;

    public:
        MemoryOutStream();
        MemoryOutStream(tuint32 buffer_size);
        ~MemoryOutStream();

        tint64 write(const void *buffer,tuint32 count);

        unsigned char *data() const;
        tuint32 count() const;
    };
}
