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
 * @file src/bufferedstream.hh
 * @brief Buffered implementation of stream interfaces.
 */

#pragma once
#include "types.hh"
#include "stream.hh"
#include "path.hh"

namespace ckCore
{
    /**
     * @brief Buffered stream class for reading streams.
     */
    class BufferedInStream : public InStream
    {
    private:
        InStream &stream_;

        unsigned char *buffer_;
        unsigned long buffer_size_;
        unsigned long buffer_pos_;

        // The number of valid bytes of data the buffer contains.
        unsigned long buffer_data_;

    public:
        BufferedInStream(InStream &stream);
        BufferedInStream(InStream &stream,unsigned long buffer_size);
        ~BufferedInStream();

        bool Eos();

        tint64 Read(void *buffer,unsigned long count);
    };

    /**
     * @brief Buffered stream class for reading streams.
     */
    class BufferedOutStream : public OutStream
    {
    private:
        OutStream &stream_;

        unsigned char *buffer_;
        unsigned long buffer_size_;
        unsigned long buffer_pos_;

    public:
        BufferedOutStream(OutStream &stream);
        BufferedOutStream(OutStream &stream,unsigned long buffer_size);
        ~BufferedOutStream();

        tint64 Write(void *buffer,unsigned long count);
        tint64 Flush();
    };
};

