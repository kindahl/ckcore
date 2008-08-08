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

#include <cxxtest/TestSuite.h>
#include <stdlib.h>
#include <algorithm>
#include "../src/types.hh"
#include "../src/filestream.hh"
#include "../src/bufferedstream.hh"

class StreamTestSuite : public CxxTest::TestSuite
{
public:
    void testInStream()
    {
        ckCore::FileInStream is1("data/file/8253bytes");
        ckCore::FileInStream fs("data/file/8253bytes");
        ckCore::BufferedInStream is2(fs);

        for (int i = 0; i < 100; i++)
        {
            TS_ASSERT(is1.Open());
            TS_ASSERT(fs.Open());

            size_t buffer_size = (rand() % 2100) + 50;
            unsigned char *buffer1 = new unsigned char[buffer_size];
            unsigned char *buffer2 = new unsigned char[buffer_size];

            ckCore::tint64 read1 = 0,read2 = 0;
            while (!is1.Eos() && !is2.Eos())
            {
                ckCore::tint64 res1 = is1.Read(buffer1,buffer_size);
                ckCore::tint64 res2 = is2.Read(buffer2,buffer_size);

                TS_ASSERT(res1 != -1);
                TS_ASSERT(res2 != -1);

                TS_ASSERT_SAME_DATA(buffer1,buffer2,buffer_size);

                read1 += res1;
                read2 += res2;
            }

            TS_ASSERT_EQUALS(is1.Eos(),is2.Eos());
            TS_ASSERT_EQUALS(read1,8253);
            TS_ASSERT_EQUALS(read2,8253);

            TS_ASSERT(is1.Close());
            TS_ASSERT(fs.Close());

            delete [] buffer1;
            delete [] buffer2;
        }
    }

    void testOutStream()
    {
        ckCore::FileInStream is1("data/file/8253bytes");
        ckCore::FileInStream is2("data/file/new");
        ckCore::FileOutStream fs("data/file/new");
        ckCore::BufferedOutStream os(fs);

        // Run 100 tests with different buffer sizes to capture buffer edge errors.
        for (int i = 0; i < 100; i++)
        {
            TS_ASSERT(is1.Open());
            TS_ASSERT(fs.Open());

            size_t buffer_size = (rand() % 2100) + 50;
            unsigned char *buffer1 = new unsigned char[buffer_size];
            unsigned char *buffer2 = new unsigned char[buffer_size];

            ckCore::tint64 written = 0;
            while (!is1.Eos())
            {
                ckCore::tint64 res1 = is1.Read(buffer1,buffer_size);
                TS_ASSERT(res1 != -1);

                ckCore::tint64 res2 = os.Write(buffer1,res1);
                TS_ASSERT(res2 != -1);

                TS_ASSERT_EQUALS(res1,res2);

                written += res2;
            }

            os.Flush();

            TS_ASSERT_EQUALS(written,8253);

            TS_ASSERT(is1.Close());
            TS_ASSERT(fs.Close());

            // Perform the same operation as in the input stream test.
            TS_ASSERT(is1.Open());
            TS_ASSERT(is2.Open());

            ckCore::tint64 read1 = 0,read2 = 0;
            while (!is1.Eos() && !is2.Eos())
            {
                ckCore::tint64 res1 = is1.Read(buffer1,buffer_size);
                ckCore::tint64 res2 = is2.Read(buffer2,buffer_size);

                TS_ASSERT(res1 != -1);
                TS_ASSERT(res2 != -1);

                TS_ASSERT_SAME_DATA(buffer1,buffer2,buffer_size);

                read1 += res1;
                read2 += res2;
            }

            TS_ASSERT_EQUALS(is1.Eos(),is2.Eos());
            TS_ASSERT_EQUALS(read1,8253);
            TS_ASSERT_EQUALS(read2,8253);

            TS_ASSERT(is1.Close());
            TS_ASSERT(is2.Close());

            TS_ASSERT(ckCore::File::Remove("data/file/new"));

            delete [] buffer1;
            delete [] buffer2;
        }
    }
};

