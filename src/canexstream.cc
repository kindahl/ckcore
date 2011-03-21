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
#include "ckcore/exception.hh"
#include "ckcore/canexstream.hh"

namespace ckcore
{
    /**
     * Constructs a CanexInStream object.
     * @param [in] stream The original non-canonical input stream.
     * @param [in] ident Name for identifying the stream.
     */
    CanexInStream::CanexInStream(InStream &stream,const tchar *ident) :
        stream_(stream),ident_(ident)
    {
    }

    /**
     * Constructs a CanexInStream object.
     * @param [in] stream The original non-canonical input stream.
     * @param [in] ident Name for identifying the stream.
     */
    CanexInStream::CanexInStream(InStream &stream,const tstring &ident) :
        stream_(stream),ident_(ident)
    {
    }

    /**
     * Returns the stream identifier name.
     * @return The stream identifier name.
     */
    const tstring &CanexInStream::identifier() const
    {
        return ident_;
    }

    /**
     * Repositions the file pointer to the specified offset accoding to the
     * whence directive in the file.
     * @param [in] distance The number of bytes that the file pointer should
     *                      move.
     * @param [in] whence Specifies what to use as base when calculating the
     *                    final file pointer position.
     * @throw Exception If seek error occurred.
     */
    void CanexInStream::seek(tuint32 distance,InStream::StreamWhence whence)
    {
        if (!stream_.seek(distance,whence))
        {
            throw Exception2(string::formatstr(ckT("stream seek error in %s."),ident_.c_str()));
        }
    }

    /**
     * Reads raw data from the stream.
     * @param [in] buffer Pointer to beginning of buffer to read to.
     * @param [in] count The number of bytes to read.
     * @return The number of bytes read (this may be zero
     *         when the end of the file has been reached).
     * @throw Exception If a read error occurred.
     */
    tint64 CanexInStream::read(void *buffer,tuint32 count)
    {
        ckcore::tint64 res = stream_.read(buffer,count);
        if (res == -1)
        {
            throw Exception2(string::formatstr(ckT("stream read error in %s."),ident_.c_str()));
        }

        return res;
    }

    /**
     * Checks if the end of the stream has been reached.
     * @return If positioned at end of the stream true is returned,
     *         otherwise false is returned.
     */
    bool CanexInStream::end()
    {
        return stream_.end();
    }

    /**
     * Constructs a CanexOutStream object.
     * @param [in] stream The original non-canonical output stream.
     * @param [in] ident Name for identifying the stream.
     */
    CanexOutStream::CanexOutStream(OutStream &stream,const tchar *ident) :
        stream_(stream),ident_(ident)
    {
    }

    /**
     * Constructs a CanexOutStream object.
     * @param [in] stream The original non-canonical output stream.
     * @param [in] ident Name for identifying the stream.
     */
    CanexOutStream::CanexOutStream(OutStream &stream,const tstring &ident) :
        stream_(stream),ident_(ident)
    {
    }

    /**
     * Returns the stream identifier name.
     * @return The stream identifier name.
     */
    const tstring &CanexOutStream::identifier() const
    {
        return ident_;
    }

    /**
     * Writes raw data to the stream.
     * @param [in] buffer Pointer to the beginning of the bufferi
     *                    containing the data to be written.
     * @param [in] count The number of bytes to write.
     * @throw Exception If write error occurred or if not all bytes were
     *                  written.
     */
    void CanexOutStream::write(void *buffer,tuint32 count)
    {
        ckcore::tint64 res = stream_.write(buffer,count);
        if (res == -1 || res != count)
        {
            throw Exception2(string::formatstr(ckT("stream write error in %s."),ident_.c_str()));
        }
    }

    namespace canexstream
    {
        /**
         * Copies the contents of the input stream to the output stream. An
         * internal buffer is used to optimize the process. Progress is
         * reported through a Progresser object.
         * @param [in] from The source stream.
         * @param [in] to The target stream.
         * @param [in] progresser A reference to the progresser object to use
         *                        for reporting progress.
         * @throw Exception On read or write errors.
         */
        void copy(CanexInStream &from,CanexOutStream &to,Progresser &progresser)
        {
            unsigned char buffer[8192];

            tint64 res = 0;
            while (!from.end())
            {
                // Check if we should cancel.
                if (progresser.cancelled())
                    return;

                res = from.read(buffer,sizeof(buffer));
                to.write(buffer,(tuint32)res);

                // Update progress.
                progresser.update(res);
            }
        }

        /**
         * Copies the contents of the input stream to the output stream. An
         * internal buffer is used to optimize the process. Progress is
         * reported through a Progresser object. If the available data in the
         * input stream is less than requested the output stream will be padded
         * to match the requested ammount. If more data is available in the
         * input stream than what is requested the additional data will be ignored.
         * @param [in] from The source stream.
         * @param [in] to The target stream.
         * @param [in] progresser A reference to the progresser object to use
         *                        for reporting progress.
         * @param [in] size The exact number of bytes to write to the output
         *                  stream.
         * @throw Exception On read or write errors.
         */
        void copy(CanexInStream &from,CanexOutStream &to,Progresser &progresser,
                  tuint64 size)
        {
            unsigned char buffer[8192];

            tint64 res = 0;
            while (!from.end() && size > 0)
            {
                // Check if we should cancel.
                if (progresser.cancelled())
                    return;

                tuint32 to_read = size < sizeof(buffer) ?
                                  static_cast<tuint32>(size) : sizeof(buffer);
                res = from.read(buffer,to_read);
                to.write(buffer,static_cast<tuint32>(res));

                size -= res;

                // Update progress.
                progresser.update(res);
            }

            // Pad if necessary. This is not very efficient but it should also not
            // happen.
            while (size > 0)
            {
                tuint32 to_write = size < sizeof(buffer) ?
                                   static_cast<tuint32>(size) : sizeof(buffer);
                memset(buffer,0,sizeof(buffer));

                to.write(buffer,to_write);
                size -= to_write;

                // Update progress.
                progresser.update(res);
            }
        }
    }
}
