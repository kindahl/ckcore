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
 * @file include/ckcore/linereader.hh
 * @brief Defines the line reader class.
 */

#pragma once
#include "ckcore/types.hh"
#include "ckcore/stream.hh"

namespace ckcore
{
    /**
     * @brief Class for parsing a stream into lines.
     */
    template<typename T>
    class LineReader
    {
    public:
        /**
         * Defines different file encodings.
         */
        enum Encoding
        {
            ckENCODING_ANSI,
            ckENCODING_UTF1,
            ckENCODING_UTF7,
            ckENCODING_UTF8,
            ckENCODING_UTF16BE,
            ckENCODING_UTF16LE,
            ckENCODING_UTF32BE,
            ckENCODING_UTF32LE,
            ckENCODING_UTFEBCDIC,
            ckENCODING_SCSU,
            ckENCODING_BOCU1
        };

    private:
        Encoding encoding_;
        std::basic_string<T> next_str_;
        InStream &stream_;

    public:
        /**
         * Constructs a LineReader object.
         * @param [in] stream The input stream to use for reading and parsing
         *                    into lines.
         */
        LineReader(InStream &stream) : stream_(stream),encoding_(ckENCODING_ANSI)
        {
            unsigned char bom[4];
            tint64 read = stream.read(bom,4);
            stream_.seek(0,InStream::ckSTREAM_BEGIN);

            if (read >= 2)
            {
                // Check if UTF-16 (big endian).
                if (bom[0] == 0xfe && bom[1] == 0xff)
                {
                    encoding_ = ckENCODING_UTF16BE;
                    stream_.seek(2,InStream::ckSTREAM_CURRENT);
                }

                // Check if UTF-16 (little endian).
                if (bom[0] == 0xff && bom[1] == 0xfe)
                {
                    encoding_ = ckENCODING_UTF16LE;
                    stream_.seek(2,InStream::ckSTREAM_CURRENT);
                }

                if (read >= 3)
                {
                    // Check if UTF-1.
                    if (bom[0] == 0xf7 && bom[1] == 0x64 && bom[2] == 0x4c)
                    {
                        encoding_ = ckENCODING_UTF1;
                        stream_.seek(3,InStream::ckSTREAM_CURRENT);
                    }

                    // Check if UTF-8.
                    if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf)
                    {
                        encoding_ = ckENCODING_UTF8;
                        stream_.seek(3,InStream::ckSTREAM_CURRENT);
                    }

                    // Check if SCSU.
                    if (bom[0] == 0x0e && bom[1] == 0xfe && bom[2] == 0xff)
                    {
                        encoding_ = ckENCODING_SCSU;
                        stream_.seek(3,InStream::ckSTREAM_CURRENT);
                    }

                    // Check if BOCU-1.
                    if (bom[0] == 0xfb && bom[1] == 0xee && bom[2] == 0x28)
                    {
                        encoding_ = ckENCODING_BOCU1;
                        stream_.seek(3,InStream::ckSTREAM_CURRENT);
                    }

                    if (read == 4)
                    {
                        // Check if UTF-32 (big endian).
                        if (bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xfe && bom[3] == 0xff)
                        {
                            encoding_ = ckENCODING_UTF32BE;
                            stream_.seek(4,InStream::ckSTREAM_CURRENT);
                        }

                        // Check if UTF-32 (little endian).
                        if (bom[0] == 0xff && bom[1] == 0xfe && bom[2] == 0x00 && bom[3] == 0x00)
                        {
                            encoding_ = ckENCODING_UTF32LE;
                            stream_.seek(2,InStream::ckSTREAM_CURRENT); // We have already skipped two bytes for UTF-16.
                        }

                        // Check if UTF-EBCDIC.
                        if (bom[0] == 0xdd && bom[1] == 0x73 && bom[2] == 0x66 && bom[3] == 0x73)
                        {
                            encoding_ = ckENCODING_UTFEBCDIC;
                            stream_.seek(4,InStream::ckSTREAM_CURRENT);
                        }

                        // Check if BOCU-1 (extended).
                        if (bom[0] == 0xfb && bom[1] == 0xee && bom[2] == 0x28 && bom[3] == 0xff)
                            stream_.seek(1,InStream::ckSTREAM_CURRENT); // We have already skipped three bytes for BOCU-1.
                    }
                }
            }
        }

        /**
         * Returns the type of encoding used in the input stream.
         * @return The type of encoding used in the input stream.
         */
        Encoding encoding()
        {
            return encoding_;
        }

        /**
         * Checks if the end of the input stream has been reached.
         * @return If positioned at end of the stream true is returned,
         *         otherwise false is returned.
         */
        bool end()
        {
            return stream_.end() && next_str_.size() == 0;
        }

        /**
         * reads a new line from the input stream.
         * @return The new line read from the input stream. If the end has been
         *         reached an empty string is returned.
         */
        std::basic_string<T> read_line()
        {
            std::basic_string<T> line;
            bool has_cr = false;

            // See if we have any char from previous parsings to add.
            if (next_str_.size() > 0)
            {
                line = next_str_;
                next_str_.clear();
            }

            // Loop until we find line breaks or the end of stream.
            while (!stream_.end())
            {
                T c;
                tint64 read = stream_.read(&c,sizeof(T));
                if (read != sizeof(T))
                    return line;

                if (c == '\n')
                {
                    return line;
                }
                else if (c == '\r')
                {
                    has_cr = true;

                    // Check if the carriage return is followed by a linefeed.
                    T next;
                    read = stream_.read(&next,sizeof(T));
                    if (read == sizeof(T) && next != '\n')
                        next_str_.push_back(next);

                    return line;
                }
                else
                {
                    line.push_back(c);
                }
            }

            return line;
        }
    };
};

