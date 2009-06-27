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

#include <cxxtest/TestSuite.h>
#include <stdlib.h>
#include "ckcore/types.hh"
#include "ckcore/file.hh"
#include "ckcore/process.hh"

#ifdef _WINDOWS
#define FILETESTER		ckT("bin/filetester.exe")
#else
#define FILETESTER		ckT("./bin/filetester")
#endif

class SimpleProcess: public ckcore::Process
{
public:
    void event_finished() {}
    void event_output(const std::string &block) {}
};

class FileTestSuite : public CxxTest::TestSuite
{
public:
    void testOpenClose()
    {
        // test non-existing file.
        ckcore::File file1(ckT("data/file/non-existent"));
        TS_ASSERT(!file1.test());

        TS_ASSERT(!file1.open(ckcore::File::ckOPEN_READ));
        TS_ASSERT(!file1.test());
        TS_ASSERT(!file1.close());
        TS_ASSERT(!file1.test());

        TS_ASSERT(file1.open(ckcore::File::ckOPEN_WRITE));
        TS_ASSERT(file1.test());
        TS_ASSERT(file1.close());
        TS_ASSERT(!file1.test());

        TS_ASSERT(file1.remove());
        TS_ASSERT(!file1.test());
        TS_ASSERT(!file1.close());

        // test existing file.
        ckcore::File file2(ckT("data/file/0bytes"));
        TS_ASSERT(!file2.test());

        TS_ASSERT(file2.open(ckcore::File::ckOPEN_READ));
        TS_ASSERT(file2.test());
        TS_ASSERT(file2.close());
        TS_ASSERT(!file2.test());

        TS_ASSERT(file2.open(ckcore::File::ckOPEN_WRITE));
        TS_ASSERT(file2.test());
        TS_ASSERT(file2.close());
        TS_ASSERT(!file2.test());
    }

	void testAppend()
	{
		ckcore::File file1(ckT("data/file/new1"));
		ckcore::File file2(ckT("data/file/new2"));

		TS_ASSERT(file1.open(ckcore::File::ckOPEN_WRITE));
		TS_ASSERT(file2.open(ckcore::File::ckOPEN_WRITE));
		TS_ASSERT(file1.write("1234",4) != -1);
		TS_ASSERT(file2.write("1234",4) != -1);
		TS_ASSERT(file1.close());
		TS_ASSERT(file2.close());

		TS_ASSERT(file1.open(ckcore::File::ckOPEN_WRITE));
		TS_ASSERT(file2.open(ckcore::File::ckOPEN_READWRITE));
		TS_ASSERT(file2.seek(0,ckcore::File::ckFILE_END) != -1);
		TS_ASSERT(file1.write("5678",4) != -1);
		TS_ASSERT(file2.write("5678",4) != -1);
		TS_ASSERT(file1.close());
		TS_ASSERT(file2.close());

		TS_ASSERT_EQUALS(file1.size(),4);
		TS_ASSERT_EQUALS(file2.size(),8);

		TS_ASSERT(file1.remove());
		TS_ASSERT(file2.remove());
	}

    void testReadWrite()
    {
        ckcore::File file(ckT("data/file/new"));
        TS_ASSERT(file.open(ckcore::File::ckOPEN_WRITE));
        const char out_data[] = "abcdefghijklmnopqrstuvwxyz0123456789";

		ckcore::tuint32 tot_write = 0;
        while (tot_write < 37)
        {
			ckcore::tuint32 write = (ckcore::tuint32)file.write(out_data + tot_write,
																37 - tot_write);
            TS_ASSERT(write != -1);

            tot_write += write;
        }

        file.close();
        file.open(ckcore::File::ckOPEN_READ);
        char in_data[37];

		ckcore::tuint32 tot_read = 0;
        while (tot_read < 37)
        {
            ckcore::tuint32 read = (ckcore::tuint32)file.read(in_data,37 - tot_read);
            TS_ASSERT(read != -1);

            tot_read += read;
        }

        file.close();
        file.remove();

        TS_ASSERT_SAME_DATA(in_data,out_data,37);
    }

    void testSeekTell()
    {
        ckcore::File file(ckT("data/file/8253bytes"));
        TS_ASSERT(file.open(ckcore::File::ckOPEN_READ));
        TS_ASSERT(file.test());

        // test exteme values.
        TS_ASSERT(file.seek(0,ckcore::File::ckFILE_BEGIN) == 0);
        TS_ASSERT(file.tell() == 0);
        TS_ASSERT(file.seek(0,ckcore::File::ckFILE_CURRENT) == 0);
        TS_ASSERT(file.tell() == 0);

        TS_ASSERT(file.seek(0,ckcore::File::ckFILE_END) == file.size());
        TS_ASSERT(file.tell() == file.size());

        int file_size = 8253;

        // test random seeking.
        for (unsigned int i = 0; i < 100; i++)
        {
            TS_ASSERT(file.seek(0,ckcore::File::ckFILE_BEGIN) == 0);

            int seek1 = rand() % (file_size >> 1);
            int seek2 = rand() % (file_size >> 1);
            int seekckTot = seek1 + seek2;

            // seek forward from the current position.
            TS_ASSERT(file.seek(seek1,ckcore::File::ckFILE_CURRENT) == seek1);
            TS_ASSERT(file.tell() == seek1);

            TS_ASSERT(file.seek(seek2,ckcore::File::ckFILE_CURRENT) == seekckTot);
            TS_ASSERT(file.tell() == seekckTot);

            // seek backward from the current position.
            TS_ASSERT(file.seek(-seek1,ckcore::File::ckFILE_CURRENT) == seek2);
            TS_ASSERT(file.tell() == seek2);

            TS_ASSERT(file.seek(-seek2,ckcore::File::ckFILE_CURRENT) == 0);
            TS_ASSERT(file.tell() == 0);
        }
    }

    void testExistRemove()
    {
        ckcore::File file1(ckT("data/file/non-existent"));
        TS_ASSERT(!file1.exist());
        TS_ASSERT(!file1.remove());
        TS_ASSERT(file1.open(ckcore::File::ckOPEN_WRITE));
        TS_ASSERT(file1.exist());
        TS_ASSERT(file1.remove());
        TS_ASSERT(!file1.exist());

        ckcore::File file2(ckT("data/file/0bytes"));
        TS_ASSERT(file2.exist());

        // test static functions.
        TS_ASSERT(ckcore::File::exist(ckT("data/file/0bytes")));
        TS_ASSERT(!ckcore::File::exist(ckT("data/file/non-existent")));

        TS_ASSERT(!ckcore::File::remove(ckT("data/file/non-existent")));
        ckcore::File file3(ckT("data/file/new"));
        TS_ASSERT(file3.open(ckcore::File::ckOPEN_WRITE));
        TS_ASSERT(file3.close());
        TS_ASSERT(ckcore::File::remove(ckT("data/file/new")));
        TS_ASSERT(!ckcore::File::remove(ckT("data/file/non-existent")));
        TS_ASSERT(!ckcore::File::remove(ckT("")));
    }

    void testRename()
    {
        // rename file in existing folder (should succeed).
        ckcore::File file1(ckT("data/file/new"));
        TS_ASSERT(file1.open(ckcore::File::ckOPEN_WRITE));

        TS_ASSERT(file1.rename(ckT("data/file/new2")));
        TS_ASSERT(ckcore::File::exist(ckT("data/file/new2")));
        TS_ASSERT(!file1.close());

        // rename file in new non-existing folder (should fail).
        ckcore::File file2(ckT("data/file/new"));
        TS_ASSERT(file2.open(ckcore::File::ckOPEN_WRITE));

        TS_ASSERT(!file2.rename(ckT("data/file/new/new2")));
        TS_ASSERT(!ckcore::File::exist(ckT("data/file/new/new2")));
        TS_ASSERT(!file2.close());

        // Try to rename file to existing file (should fail).
        TS_ASSERT(!file2.rename(ckT("data/file/new2")));

        // Clean up.
        TS_ASSERT(file1.remove());
        TS_ASSERT(file2.remove());

        // Try to rename non-existent file.
        ckcore::File file3(ckT("data/file/non-existent"));
        TS_ASSERT(!file3.rename(ckT("data/file/new")));
        TS_ASSERT(!ckcore::File::exist(ckT("data/file/new")));
        TS_ASSERT(!file3.rename(ckT("data/file/non-existent")));
        TS_ASSERT(!ckcore::File::exist(ckT("data/file/non-existent")));

        // Try to rename without name.
        TS_ASSERT(!file3.rename(ckT("")));

        // test static function.
        ckcore::File file4(ckT("data/file/new"));
        TS_ASSERT(file4.open(ckcore::File::ckOPEN_WRITE));
        TS_ASSERT(file4.close());
        TS_ASSERT(ckcore::File::rename(ckT("data/file/new"),ckT("data/file/new2")));
        TS_ASSERT(!ckcore::File::rename(ckT("data/file/new2"),ckT("data/file/new2")));
        TS_ASSERT(!ckcore::File::rename(ckT("data/file/new2"),ckT("")));
        TS_ASSERT(!ckcore::File::rename(ckT("data/file/new2"),ckT("data/file/new/new2")));
        TS_ASSERT(ckcore::File::remove(ckT("data/file/new2")));
    }

    void testSize()
    {
        const ckcore::tchar *file_paths[] =
        {
            ckT("data/file/0bytes"),
            ckT("data/file/53bytes"),
            ckT("data/file/123bytes"),
            ckT("data/file/8253bytes")
        };

        const ckcore::tint64 file_sizes[] =
        {
            0,
            53,
            123,
            8253
        };

        for (unsigned int i = 0; i < 4; i++)
        {
            ckcore::File file(file_paths[i]);
            TS_ASSERT_EQUALS(file.size(),file_sizes[i]);
            file.open(ckcore::File::ckOPEN_READ);
            TS_ASSERT_EQUALS(file.size(),file_sizes[i]);
            file.close();
            /*file.open(ckcore::File::ckOPEN_WRITE);
            TS_ASSERT_EQUALS(file.size(),file_sizes[i]);
            file.close();*/

            // test static function.
            TS_ASSERT_EQUALS(ckcore::File::size(file_paths[i]),file_sizes[i]);
        }
    }

	void testExclusiveAccess()
	{
		// Create a new file.
		ckcore::File file(ckT("data/file/new"));
        TS_ASSERT(file.open(ckcore::File::ckOPEN_WRITE));
        TS_ASSERT(file.close());

        TS_ASSERT(file.open(ckcore::File::ckOPEN_READ));
        TS_ASSERT(file.test());

		// Launch an external process that tries to read from the test file
		// (should succeed).
		SimpleProcess process;
		ckcore::tstring cmd_line = FILETESTER;
		cmd_line += ckT(" -r data/file/new");

		TS_ASSERT(process.create(cmd_line.c_str()));
        process.wait();

		ckcore::tuint32 exit_code = -1;
		TS_ASSERT(process.exit_code(exit_code));
		TS_ASSERT_EQUALS(exit_code,0);

		// Launch an external process that tries to write to the test file
		// (should fail).
		cmd_line = FILETESTER;
		cmd_line += ckT(" -w data/file/new");

		TS_ASSERT(process.create(cmd_line.c_str()));
        process.wait();

		exit_code = -1;
		TS_ASSERT(process.exit_code(exit_code));
		TS_ASSERT_EQUALS(exit_code,1);

		// Launch an external process that tries to remove the test file
		// (should fail).
		cmd_line = FILETESTER;
		cmd_line += ckT(" -d data/file/new");

		TS_ASSERT(process.create(cmd_line.c_str()));
        process.wait();

		exit_code = -1;
		TS_ASSERT(process.exit_code(exit_code));
		TS_ASSERT_EQUALS(exit_code,1);

		// Finally, close  and remove the file.
        TS_ASSERT(file.close());
		TS_ASSERT(file.remove());
	}
};
