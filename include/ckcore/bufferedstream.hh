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
 * @file include/ckcore/bufferedstream.hh
 * @brief Buffered implementation of stream interfaces.
 */

#pragma once
#include "ckcore/types.hh"
#include "ckcore/stream.hh"
#include "ckcore/path.hh"

namespace ckcore
{
    /**
     * @brief Buffered stream class for reading streams.
     */
    class BufferedInStream : public InStream
    {
    private:
        InStream &stream_;

        unsigned char *buffer_;
        tuint32 buffer_size_;
        tuint32 buffer_pos_;

        // The number of valid bytes of data the buffer contains.
        unsigned long buffer_data_;

    public:
        BufferedInStream(InStream &stream);
        BufferedInStream(InStream &stream,tuint32 buffer_size);
        virtual ~BufferedInStream();

        bool end();
		bool seek(tuint32 distance,StreamWhence whence);

        tint64 read(void *buffer,tuint32 count);
		tint64 size();
    };

    /**
     * @brief Buffered stream class for reading streams.
     */
    class BufferedOutStream : public OutStream
    {
    private:
        OutStream &stream_;

        unsigned char *buffer_;
        tuint32 buffer_size_;
        tuint32 buffer_pos_;

    public:
        BufferedOutStream(OutStream &stream);
        BufferedOutStream(OutStream &stream,tuint32 buffer_size);
        ~BufferedOutStream();

        tint64 write(void *buffer,tuint32 count);
        tint64 flush();
    };
};

