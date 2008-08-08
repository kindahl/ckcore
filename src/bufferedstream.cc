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

#include <string.h>
#include "system.hh"
#include "bufferedstream.hh"

namespace ckCore
{
    /**
     * Constructs an BufferedInStream object. The default internal buffer size
     * is the size of the host processor level 1 cache.
     * @param [in] stream Input stream to read from.
     */
    BufferedInStream::BufferedInStream(InStream &stream) : stream_(stream),
        buffer_(NULL),buffer_size_(0),buffer_pos_(0),buffer_data_(0)
    {
        buffer_size_ = System::Cache(System::ckLEVEL_1);
        buffer_ = new unsigned char[buffer_size_];
    }

    /**
     * Constructs an BufferedInStream object.
     * @param [in] stream Input stream to read from.
     * @param [in] buffer_size The size of the internal buffer.
     */
    BufferedInStream::BufferedInStream(InStream &stream,
                                       unsigned long buffer_size) :
        stream_(stream),buffer_(NULL),buffer_size_(buffer_size),buffer_pos_(0),
        buffer_data_(0)
    {
        buffer_ = new unsigned char[buffer_size_];
    }

    /**
     * Destructs the BufferedInStream object.
     */
    BufferedInStream::~BufferedInStream()
    {
        // Free the memory allocated for the internal buffer.
        if (buffer_ != NULL)
        {
            delete [] buffer_;
            buffer_ = NULL;
        }
    }

    /**
     * Checks if the end of the stream has been reached.
     * @return If positioned at end of the stream true is returned,
     *         otherwise false is returned.
     */
    bool BufferedInStream::Eos()
    {
        return stream_.Eos() && buffer_data_ == 0;
    }

    /**
     * Reads raw data from the stream.
     * @param [in] buffer Pointer to beginning of buffer to read to.
     * @param [in] count The number of bytes to read.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of butes read (this may be zero
     *         when the end of the file has been reached).
     */
    tint64 BufferedInStream::Read(void *buffer,unsigned long count)
    {
        unsigned long pos = 0;

        while (count > buffer_data_)
        {
            memcpy((unsigned char *)buffer + pos,buffer_ + buffer_pos_,buffer_data_);
            count -= buffer_data_;

            pos += buffer_data_;

            buffer_pos_ = 0;
            buffer_data_ = 0;

            // Fetch more data from the input stream.
            if (stream_.Eos())
                return pos;

            tint64 result = stream_.Read(buffer_,buffer_size_);
            if (result == -1)
                return pos == 0 ? -1 : pos;

            buffer_data_ = result;
        }

        memcpy((unsigned char *)buffer + pos,buffer_ + buffer_pos_,count);
        buffer_pos_ += count;
        buffer_data_ -= count;

        return pos + count;
    }

    /**
     * Constructs an BufferedOutStream object. The default internal buffer size
     * is the size of the host processor level 1 cache.
     * @param [in] stream Output stream to write to.
     */
    BufferedOutStream::BufferedOutStream(OutStream &stream) : stream_(stream),
        buffer_(NULL),buffer_size_(0),buffer_pos_(0)
    {
        buffer_size_ = System::Cache(System::ckLEVEL_1);
        buffer_ = new unsigned char[buffer_size_];
    }

    /**
     * Constructs an BufferedOutStream object.
     * @param [in] stream Output stream to write to.
     * @param [in] buffer_size The size of the internal buffer.
     */
    BufferedOutStream::BufferedOutStream(OutStream &stream,
                                         unsigned long buffer_size) :
        stream_(stream),buffer_(NULL),buffer_size_(buffer_size),buffer_pos_(0)
    {
        buffer_ = new unsigned char[buffer_size_];
    }

    /**
     * Destructs the BufferedOutStream object.
     */
    BufferedOutStream::~BufferedOutStream()
    {
        // Free the memory allocated for the internal buffer.
        if (buffer_ != NULL)
        {
            delete [] buffer_;
            buffer_ = NULL;
        }
    }

    /**
     * Writes raw data to the stream.
     * @param [in] buffer Pointer to the beginning of the bufferi
     *                    containing the data to be written.
     * @param [in] count The number of bytes to write.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of bytes written.
     */
    tint64 BufferedOutStream::Write(void *buffer,unsigned long count)
    {
        unsigned long pos = 0;

        while (buffer_pos_ + count > buffer_size_)
        {
            unsigned long remain = buffer_size_ - buffer_pos_;
            memcpy(buffer_ + buffer_pos_,(unsigned char *)buffer + pos,remain);

            pos += remain;

            // Flush.
            if (stream_.Write(buffer_,buffer_size_) == -1)
                return pos == 0 ? -1 : pos;

            buffer_pos_ = 0;

            count -= remain;
        }

        memcpy(buffer_ + buffer_pos_,(unsigned char *)buffer + pos,count);
        buffer_pos_ += count;

        return pos + count;
    }

    /**
     * Flushes the internal buffer, writing all buffered data to the output
     * stream.
     * @return If the operation failed -1 is returned, otherwise the number of
     *         bytes that where flushed is returned.
     */
    tint64 BufferedOutStream::Flush()
    {
        tint64 result = stream_.Write(buffer_,buffer_pos_);
        if (result != -1)
            buffer_pos_ = 0;

        return result;
    }
};

