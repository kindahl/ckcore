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

#include "ckcore/filestream.hh"

namespace ckcore
{
    /**
     * Constructs a FileInStream object.
     */
    FileInStream::FileInStream(const Path &file_path) : file_(file_path),
        size_(file_.size()),read_(0)
    {
    }

	/**
     * Closes the stream and destructs the object.
     */
	FileInStream::~FileInStream()
	{
		close();
	}

    /**
     * Opens the file for access through the stream.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileInStream::open()
    {
        size_ = file_.size();
        return file_.open(File::ckOPEN_READ);
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileInStream::close()
    {
        if (file_.close())
        {
            read_ = 0;
            return true;
        }

        return false;
    }

    /**
     * Checks if the end of the stream has been reached.
     * @return If positioned at end of the stream true is returned,
     *         otherwise false is returned.
     */
    bool FileInStream::end()
    {
        return read_ >= size_;
    }

	/**
     * Repositions the file pointer to the specified offset accoding to the
     * whence directive in the file.
     * @param [in] distance The number of bytes that the file pointer should
     *                      move.
     * @param [in] whence Specifies what to use as base when calculating the
     *                    final file pointer position.
     * @return If successfull true is returned, otherwise false is returned.
     */
	bool FileInStream::seek(tuint32 distance,StreamWhence whence)
	{
		File::FileWhence file_whence;
		switch (whence)
		{
			case ckSTREAM_CURRENT:
				file_whence = File::ckFILE_CURRENT;
				break;

			default:
				file_whence = File::ckFILE_BEGIN;
				break;
		}

		tint64 result = file_.seek(distance,file_whence);
		if (result != -1)
		{
			read_ = result;
			return true;
		}

		return false;
	}

	/**
     * Checks whether the file stream has been opened or not.
     * @return If a file stream is open true is returned, otherwise false is
	 *		   returned.
     */
	bool FileInStream::test() const
	{
		return file_.test();
	}

    /**
     * Reads raw data from the stream.
     * @param [in] buffer Pointer to beginning of buffer to read to.
     * @param [in] count The number of bytes to read.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of butes read (this may be zero
     *         when the end of the file has been reached).
     */
    tint64 FileInStream::read(void *buffer,tuint32 count)
    {
        tint64 result = file_.read(buffer,count);
        if (result != -1)
            read_ += result;

        return result;
    }

	/**
	 * Returns the size of the file provoding data for the stream.
	 * @return If successfull the size in bytes of the file is returned,
	 *		   if unsuccessfull -1 is returned.
	 */
	tint64 FileInStream::size()
	{
		return size_;
	}

    /**
     * Constructs a FileOutStream object.
     */
    FileOutStream::FileOutStream(const Path &file_path) : file_(file_path)
    {
    }

	/**
     * Closes the stream and destructs the object.
     */
	FileOutStream::~FileOutStream()
	{
		close();
	}

    /**
     * Opens the file for access through the stream.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileOutStream::open()
    {
        return file_.open(File::ckOPEN_WRITE);
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileOutStream::close()
    {
        return file_.close();
    }

    /**
     * Writes raw data to the stream.
     * @param [in] buffer Pointer to the beginning of the bufferi
     *                    containing the data to be written.
     * @param [in] count The number of bytes to write.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of bytes written (this may be
     *         zero).
     */
    tint64 FileOutStream::write(void *buffer,tuint32 count)
    {
        return file_.write(buffer,count);
    }
};

