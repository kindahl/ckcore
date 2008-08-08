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
#include "../src/crcstream.hh"

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

    void testCrcStream()
    {
        ckCore::FileInStream is1("data/file/8253bytes");
        TS_ASSERT(is1.Open());
        ckCore::FileInStream is2("data/file/123bytes");
        TS_ASSERT(is2.Open());
        ckCore::FileInStream is3("data/file/53bytes");
        TS_ASSERT(is3.Open());
        ckCore::FileInStream is4("data/file/0bytes");
        TS_ASSERT(is4.Open());

        // CRC-32.
        ckCore::CrcStream crc32(ckCore::CrcStream::ckCRC_32);

        ckCore::Stream::Copy(is1,crc32);
        TS_ASSERT_EQUALS(crc32.Checksum(),0x33d5a2ec);
        crc32.Reset();

        ckCore::Stream::Copy(is2,crc32);
        TS_ASSERT_EQUALS(crc32.Checksum(),0xfa2e73f4);
        crc32.Reset();

        ckCore::Stream::Copy(is3,crc32);
        TS_ASSERT_EQUALS(crc32.Checksum(),0x30e06b16);
        crc32.Reset();

        ckCore::Stream::Copy(is4,crc32);
        TS_ASSERT_EQUALS(crc32.Checksum(),0x00000000);
        crc32.Reset();

        is1.Close();
        is1.Open();
        is2.Close();
        is2.Open();
        is3.Close();
        is3.Open();
        is4.Close();
        is4.Open();

        // CRC-16 (CCITT polynomial).
        ckCore::CrcStream crc16udf(ckCore::CrcStream::ckCRC_CCITT);

        ckCore::Stream::Copy(is1,crc16udf);
        TS_ASSERT_EQUALS(crc16udf.Checksum(),0x8430);
        crc16udf.Reset();

        ckCore::Stream::Copy(is2,crc16udf);
        TS_ASSERT_EQUALS(crc16udf.Checksum(),0x8bfe);
        crc16udf.Reset();

        ckCore::Stream::Copy(is3,crc16udf);
        TS_ASSERT_EQUALS(crc16udf.Checksum(),0xef2a);
        crc16udf.Reset();

        ckCore::Stream::Copy(is4,crc16udf);
        TS_ASSERT_EQUALS(crc16udf.Checksum(),0x0000);
        crc16udf.Reset();

        // Test from the UDF 1.50 reference.
        unsigned char bytes[] = { 0x70, 0x6A, 0x77 };
        crc16udf.Write(bytes,3);
        TS_ASSERT_EQUALS(crc16udf.Checksum(),0x3299);

        is1.Close();
        is1.Open();
        is2.Close();
        is2.Open();
        is3.Close();
        is3.Open();
        is4.Close();
        is4.Open();

        // CRC-16 (IBM polynomial).
        ckCore::CrcStream crc16ibm(ckCore::CrcStream::ckCRC_16);

        ckCore::Stream::Copy(is1,crc16ibm);
        TS_ASSERT_EQUALS(crc16ibm.Checksum(),0x398e);
        crc16ibm.Reset();

        ckCore::Stream::Copy(is2,crc16ibm);
        TS_ASSERT_EQUALS(crc16ibm.Checksum(),0xd3bb);
        crc16ibm.Reset();

        ckCore::Stream::Copy(is3,crc16ibm);
        TS_ASSERT_EQUALS(crc16ibm.Checksum(),0xb7d4);
        crc16ibm.Reset();

        ckCore::Stream::Copy(is4,crc16ibm);
        TS_ASSERT_EQUALS(crc16ibm.Checksum(),0x0000);
        crc16ibm.Reset();
    }
};

