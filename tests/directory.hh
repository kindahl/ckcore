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
#include "../src/directory.hh"

class DirectoryTestSuite : public CxxTest::TestSuite
{
public:
    void testCreateRemove()
    {
        ckCore::Directory dir1(ckT("data/new"));
        TS_ASSERT(dir1.Create());
        TS_ASSERT(dir1.Remove());

        ckCore::Directory dir2(ckT("data/new/"));
        TS_ASSERT(dir2.Create());
        TS_ASSERT(dir2.Remove());

        ckCore::Directory dir3(ckT("data/new/new"));
        TS_ASSERT(dir3.Create());
        TS_ASSERT(dir3.Remove());

        ckCore::Directory dir4(ckT("data/new/new/"));
        TS_ASSERT(dir4.Create());
        TS_ASSERT(dir4.Remove());

        // This is just for clean up since the above calls to Remove does not
        // remove more than one directory entry.
        ckCore::Directory dir5(ckT("data/new"));
        TS_ASSERT(dir5.Remove());
    }

    void testIterator()
    {
        ckCore::Directory::Iterator it;
        ckCore::Directory dir1(ckT("data"));
        ckCore::Directory dir2(ckT("data/file"));

        std::list<ckCore::tstring> files1,files2;
        std::list<ckCore::tstring>::iterator it_file;

        files1.push_back(ckT(".svn"));
        files1.push_back(ckT("file"));

        files2.push_back(ckT(".svn"));
        files2.push_back(ckT("0bytes"));
        files2.push_back(ckT("53bytes"));
        files2.push_back(ckT("123bytes"));
        files2.push_back(ckT("8253bytes"));

        for (it = dir1.Begin(); it != dir1.End(); it++)
        {
            it_file = std::find(files1.begin(),files1.end(),*it);
            if (it_file != files1.end())
                files1.erase(it_file);
        }

        for (it = dir2.Begin(); it != dir2.End(); it++)
        {
            it_file = std::find(files2.begin(),files2.end(),*it);
            if (it_file != files2.end())
                files2.erase(it_file);
        }

        TS_ASSERT(files1.size() == 0);
        TS_ASSERT(files2.size() == 0);
    }
};

