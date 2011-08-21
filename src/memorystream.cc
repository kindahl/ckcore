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

#include <string.h>
#include "ckcore/assert.hh"
#include "ckcore/memorystream.hh"

namespace ckcore
{
    /**
     * Constructs an MemoryInStream object. The stream doesn't take ownership of the specified
     * input data, nor does it copy it. It assumes the data is available through its life time.
     * @param [in] data Pointer to data.
     * @param [in] count Number of elements available at data pointer.
     */
    MemoryInStream::MemoryInStream(unsigned char * data,tuint32 count) :
        data_(data),count_(count),pos_(0)
    {
        ckASSERT(data);
    }

    /**
     * Destructs the MemoryInStream object.
     */
    MemoryInStream::~MemoryInStream()
    {
    }

    /**
     * Checks if the end of the stream has been reached.
     * @return If positioned at end of the stream true is returned,
     *         otherwise false is returned.
     */
    bool MemoryInStream::end()
    {
        return pos_ >= count_;
    }

    /**
     * Repositions the file pointer to the specified offset accoding to the
     * whence directive in the stream. Please note that the seeking performance
     * is very poor since it calls the read function and throws the data.
     * @param [in] distance The number of bytes that the stream pointer should
     *                      move.
     * @param [in] whence Specifies what to use as base when calculating the
     *                    final stream pointer position.
     * @return If successfull true is returned, otherwise false is returned.
     */
    bool MemoryInStream::seek(tuint32 distance,StreamWhence whence)
    {
        // Reset the internal state if necessary.
        if (whence == ckSTREAM_BEGIN)
            pos_ = 0;

        pos_ += distance;
        return true;
    }

    /**
     * Reads raw data from the stream.
     * @param [in] buffer Pointer to beginning of buffer to read to.
     * @param [in] count The number of bytes to read.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of butes read (this may be zero
     *         when the end of the file has been reached).
     */
    tint64 MemoryInStream::read(void *buffer,tuint32 count)
    {
        tuint32 to_read = pos_ + count > count_ ? count_ - pos_ : count;
        memcpy(buffer,data_ + pos_,to_read);
        pos_ += to_read;

        return to_read;
    }

    /**
     * Calculates the size of the data provided by the stream.
     * @return If successfull the size in bytes of the stream data is returned,
     *         if unsuccessfull -1 is returned.
     */
    tint64 MemoryInStream::size()
    {
        return count_;
    }

    /**
     * Constructs an MemoryOutStream object.
     */
    MemoryOutStream::MemoryOutStream() : 
        buffer_(NULL),buffer_size_(1024),buffer_pos_(0)
    {
        buffer_ = new unsigned char[buffer_size_];

        // Make sure that the memory allocation succeeded.
        if (buffer_ == NULL)
            buffer_size_ = 0;
    }

    /**
     * Constructs an MemoryOutStream object.
     * @param [in] buffer_size The size of the internal buffer.
     */
    MemoryOutStream::MemoryOutStream(tuint32 buffer_size) :
        buffer_(NULL),buffer_size_(buffer_size),buffer_pos_(0)
    {
        if (buffer_size_ == 0)
            buffer_size_ = 1024;

        buffer_ = new unsigned char[buffer_size_];

        // Make sure that the memory allocation succeeded.
        if (buffer_ == NULL)
            buffer_size_ = 0;
    }

    /**
     * Destructs the MemoryOutStream object and flushes any remaining data in
     * the buffer.
     */
    MemoryOutStream::~MemoryOutStream()
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
    tint64 MemoryOutStream::write(const void *buffer,tuint32 count)
    {
        // Make sure we have a buffer to write to.
        if (buffer_ == NULL)
            return -1;

        while (buffer_pos_ + count > buffer_size_)
        {
            tuint32 new_buffer_size = buffer_size_ * 2;
            unsigned char *new_buffer = new unsigned char[new_buffer_size];
            if (new_buffer == NULL)
                return -1;

            memcpy(new_buffer,buffer_,buffer_size_);
            delete [] buffer_;

            buffer_ = new_buffer;
            buffer_size_ = new_buffer_size;
        }

        memcpy(buffer_ + buffer_pos_,buffer,count);
        buffer_pos_ += count;
        return count;
    }

    /**
     * Returns a pointer to the data.
     * @return Pointer to the data.
     */
    unsigned char *MemoryOutStream::data() const
    {
        return buffer_;
    }

    /**
     * Returns the number of elements current stored in the stream buffer.
     * @return The number of elements current stored in the stream buffer.
     */
    tuint32 MemoryOutStream::count() const
    {
        return buffer_pos_;
    }
}
