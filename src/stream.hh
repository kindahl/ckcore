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
 * @file src/stream.hh
 * @brief Defines the stream interfaces.
 */

#pragma once
#include "types.hh"

namespace ckcore
{
    /**
     * @brief Interface for input streams.
     */
    class InStream
    {
    public:
        /**
         * Readsi raw data from the stream.
         * @param [in] buffer Pointer to beginning of buffer to read to.
         * @param [in] count The number of bytes to read.
         * @return If the operation failed -1 is returned, otherwise the
         *         function returns the number of butes read (this may be zero
         *         when the end of the file has been reached).
         */
        virtual tint64 Read(void *buffer,tuint32 count) = 0;

        /**
         * Checks if the end of the stream has been reached.
         * @return If positioned at end of the stream true is returned,
         *         otherwise false is returned.
         */
        virtual bool Eos() = 0;
    };

    /**
     * @brief Interface for output streams.
     */
    class OutStream
    {
    public:
        /**
         * Writes raw data to the stream.
         * @param [in] buffer Pointer to the beginning of the bufferi
         *                    containing the data to be written.
         * @param [in] count The number of bytes to write.
         * @return If the operation failed -1 is returned, otherwise the
         *         function returns the number of bytes written (this may be
         *         zero).
         */
        virtual tint64 Write(void *buffer,tuint32 count) = 0;
    };

    namespace Stream
    {
        bool copy(InStream &from,OutStream &to);
    };
};

