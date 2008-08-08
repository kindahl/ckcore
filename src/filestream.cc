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

#include "filestream.hh"

namespace ckCore
{
    /**
     * Constructs an FileInStream object.
     */
    FileInStream::FileInStream(const Path &file_path) : file_(file_path),
        size_(file_.Size()),read_(0)
    {
    }

    /**
     * Opens the file for access through the stream.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileInStream::Open()
    {
        size_ = file_.Size();
        return file_.Open(FileBase::ckOPEN_READ);
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileInStream::Close()
    {
        if (file_.Close())
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
    bool FileInStream::Eos()
    {
        return read_ >= size_;
    }

    /**
     * Reads raw data from the stream.
     * @param [in] buffer Pointer to beginning of buffer to read to.
     * @param [in] count The number of bytes to read.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of butes read (this may be zero
     *         when the end of the file has been reached).
     */
    tint64 FileInStream::Read(void *buffer,unsigned long count)
    {
        tint64 result = file_.Read(buffer,count);
        if (result != -1)
            read_ += result;

        return result;
    }

    /**
     * Constructs an FileOutStream object.
     */
    FileOutStream::FileOutStream(const Path &file_path) : file_(file_path)
    {
    }

    /**
     * Opens the file for access through the stream.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileOutStream::Open()
    {
        return file_.Open(FileBase::ckOPEN_WRITE);
    }

    /**
     * Closes the currently opened file handle. If the file has not been opened
     * a call this call will fail.
     * @return If successfull true is returned, otherwise false.
     */
    bool FileOutStream::Close()
    {
        return file_.Close();
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
    tint64 FileOutStream::Write(void *buffer,unsigned long count)
    {
        return file_.Write(buffer,count);
    }
};

