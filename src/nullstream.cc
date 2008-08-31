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

#include "ckcore/nullstream.hh"

namespace ckcore
{
    /**
     * Constructs a NullStream object.
     */
    NullStream::NullStream() : written_(0)
    {
    }

    /**
     * Returns the number of bytes written to the stream.
     * @return The number of bytes written.
     */
    tuint64 NullStream::Written()
    {
        return written_;
    }

    /**
     * Counts the number of bytes to be written.
     * @param [in] buffer Pointer to the beginning of the bufferi
     *                    containing the data to be written (may be NULL).
     * @param [in] count The number of bytes to write.
     * @return The function returns count.
     */
    tint64 NullStream::Write(void *buffer,tuint32 count)
    {
        written_ += count;
        return count;
    }
};

