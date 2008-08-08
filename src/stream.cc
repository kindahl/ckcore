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

#include "system.hh"
#include "stream.hh"

namespace ckCore
{
    namespace Stream
    {
        /**
         * Copies the contents of the input stream to the output stream. An
         * internal buffer is used to optimize the process.
         * @param [in] from The source stream.
         * @param [in] to The target stream.
         * @return If successfull true is returned, otherwise false is
         *         returned.
         */
        bool Copy(InStream &from,OutStream &to)
        {
            unsigned long buffer_size = System::Cache(System::ckLEVEL_1);
            if (buffer_size == 0)
                buffer_size = 4096;

            unsigned char *buffer = new unsigned char[buffer_size];
            if (buffer == NULL)
                return false;

            tint64 res = 0;
            while (!from.Eos())
            {
                res = from.Read(buffer,buffer_size);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }

                res = to.Write(buffer,res);
                if (res == -1)
                {
                    delete [] buffer;
                    return false;
                }
            }

            delete [] buffer;
            return true;
        }
    };
};

