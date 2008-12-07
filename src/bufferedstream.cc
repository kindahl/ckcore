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
#include "ckcore/system.hh"
#include "ckcore/bufferedstream.hh"

namespace ckcore
{
    /**
     * Constructs an BufferedInStream object. The default internal buffer size
     * is the size of the host processor level 1 cache.
     * @param [in] stream Input stream to read from.
     */
    BufferedInStream::BufferedInStream(InStream &stream) : stream_(stream),
        buffer_(NULL),buffer_size_(0),buffer_pos_(0),buffer_data_(0)
    {
		// UPDATE: Hangs the application on some systems.
        /*buffer_size_ = System::Cache(System::ckLEVEL_1);
        if (buffer_size_ == 0)*/
            buffer_size_ = 8192;

        buffer_ = new unsigned char[buffer_size_];

        // Make sure that the memory allocation succeeded.
        if (buffer_ == NULL)
            buffer_size_ = 0;
    }

    /**
     * Constructs an BufferedInStream object.
     * @param [in] stream Input stream to read from.
     * @param [in] buffer_size The size of the internal buffer.
     */
    BufferedInStream::BufferedInStream(InStream &stream,
                                       tuint32 buffer_size) :
        stream_(stream),buffer_(NULL),buffer_size_(buffer_size),buffer_pos_(0),
        buffer_data_(0)
    {
        if (buffer_size_ == 0)
            buffer_size_ = 8192;

        buffer_ = new unsigned char[buffer_size_];

        // Make sure that the memory allocation succeeded.
        if (buffer_ == NULL)
            buffer_size_ = 0;
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
    bool BufferedInStream::End()
    {
        return stream_.End() && buffer_data_ == 0;
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
	bool BufferedInStream::Seek(tuint32 distance,StreamWhence whence)
	{
		// Reset the internal state if necessary.
		if (whence == ckSTREAM_BEGIN)
		{
			if (!stream_.Seek(0,ckSTREAM_BEGIN))
				return false;

			buffer_pos_ = 0;
			buffer_data_ = 0;
		}

		// Optimization.
		if (distance == 0)
			return true;

		// Perform the seek operation.
		tuint32 buffer_size = buffer_size_ == 0 ? 8192 : buffer_size_;
		unsigned char *temp_buffer = new unsigned char[buffer_size];
		while (distance > 0)
		{
			tuint32 read = distance > buffer_size ? buffer_size : distance;

			tint64 res = Read(temp_buffer,read);
			if (res == -1)
			{
				delete [] temp_buffer;
				return false;
			}

			distance -= (tuint32)res;
		}

		delete [] temp_buffer;
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
    tint64 BufferedInStream::Read(void *buffer,tuint32 count)
    {
        // If we have failed to allocate the internal buffer, just redirect the
        // read call.
        if (buffer_size_ == 0)
            return stream_.Read(buffer,count);

        tuint32 pos = 0;

        while (count > buffer_data_)
        {
            memcpy((unsigned char *)buffer + pos,buffer_ + buffer_pos_,buffer_data_);
            count -= buffer_data_;

            pos += buffer_data_;

            buffer_pos_ = 0;
            buffer_data_ = 0;

            // Fetch more data from the input stream.
            if (stream_.End())
                return pos;

            tint64 result = stream_.Read(buffer_,buffer_size_);
            if (result == -1)
                return pos == 0 ? -1 : pos;

            buffer_data_ = (tuint32)result;
        }

        memcpy((unsigned char *)buffer + pos,buffer_ + buffer_pos_,count);
        buffer_pos_ += count;
        buffer_data_ -= count;

        return pos + count;
    }

	/**
	 * Calculates the size of the data provided by the stream.
	 * @return If successfull the size in bytes of the stream data is returned,
	 *		   if unsuccessfull -1 is returned.
	 */
	tint64 BufferedInStream::Size()
	{
		return stream_.Size();
	}

    /**
     * Constructs an BufferedOutStream object. The default internal buffer size
     * is the size of the host processor level 1 cache.
     * @param [in] stream Output stream to write to.
     */
    BufferedOutStream::BufferedOutStream(OutStream &stream) : stream_(stream),
        buffer_(NULL),buffer_size_(0),buffer_pos_(0)
    {
		// UPDATE: Hangs the application on some systems.
        /*buffer_size_ = System::Cache(System::ckLEVEL_1);
        if (buffer_size_ == 0)*/
            buffer_size_ = 8192;

        buffer_ = new unsigned char[buffer_size_];

        // Make sure that the memory allocation succeeded.
        if (buffer_ == NULL)
            buffer_size_ = 0;
    }

    /**
     * Constructs an BufferedOutStream object.
     * @param [in] stream Output stream to write to.
     * @param [in] buffer_size The size of the internal buffer.
     */
    BufferedOutStream::BufferedOutStream(OutStream &stream,
                                         tuint32 buffer_size) :
        stream_(stream),buffer_(NULL),buffer_size_(buffer_size),buffer_pos_(0)
    {
        if (buffer_size_ == 0)
            buffer_size_ = 8192;

        buffer_ = new unsigned char[buffer_size_];

        // Make sure that the memory allocation succeeded.
        if (buffer_ == NULL)
            buffer_size_ = 0;
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
    tint64 BufferedOutStream::Write(void *buffer,tuint32 count)
    {
        // If we failed to allocate the internal buffer, just redirect the
        // write call.
        if (buffer_size_ == 0)
            return stream_.Write(buffer,count);

        tuint32 pos = 0;

        while (buffer_pos_ + count > buffer_size_)
        {
            tuint32 remain = buffer_size_ - buffer_pos_;
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
        // If we don't have a buffer we can't flush.
        if (buffer_size_ == 0)
            return 0;

        tint64 result = stream_.Write(buffer_,buffer_pos_);
        if (result != -1)
            buffer_pos_ = 0;

        return result;
    }
};

