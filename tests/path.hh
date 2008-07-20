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
#include "../src/types.hh"
#include "../src/path.hh"

class PathTestSuite : public CxxTest::TestSuite
{
public:
    void testPath()
    {
        // Full Linux paths.
        ckCore::Path path1("/this/is/a/test");
        ckCore::Path::Iterator it = path1.Begin();

        TS_ASSERT_EQUALS(*it,"this");
        ++it;
        TS_ASSERT_EQUALS(*it,"is");
        it++;
        TS_ASSERT_EQUALS(*it,"a");
        ++it;
        TS_ASSERT_EQUALS(*it,"test");

        ckCore::Path path2("/this/is/a/test/");
        it = path2.Begin();

        TS_ASSERT_EQUALS(*it,"this");
        ++it;
        TS_ASSERT_EQUALS(*it,"is");
        it++;
        TS_ASSERT_EQUALS(*it,"a");
        ++it;
        TS_ASSERT_EQUALS(*it,"test");

        // Relative path.
        ckCore::Path path3("this/is/a/test");
        it = path3.Begin();

        TS_ASSERT_EQUALS(*it,"this");
        ++it;
        TS_ASSERT_EQUALS(*it,"is");
        it++;
        TS_ASSERT_EQUALS(*it,"a");
        ++it;
        TS_ASSERT_EQUALS(*it,"test");

        // Windows paths.
        ckCore::Path path4("c:/this/is/a/test");
        it = path4.Begin();

        TS_ASSERT_EQUALS(*it,"c:");
        it++;
        TS_ASSERT_EQUALS(*it,"this");
        ++it;
        TS_ASSERT_EQUALS(*it,"is");
        it++;
        TS_ASSERT_EQUALS(*it,"a");
        ++it;
        TS_ASSERT_EQUALS(*it,"test");

#ifdef _WINDOWS
        ckCore::Path path5("c:\\this/is\\a/test");
        it = path5.Begin();

        TS_ASSERT_EQUALS(*it,"c:");
        it++;
        TS_ASSERT_EQUALS(*it,"this");
        ++it;
        TS_ASSERT_EQUALS(*it,"is");
        it++;
        TS_ASSERT_EQUALS(*it,"a");
        ++it;
        TS_ASSERT_EQUALS(*it,"test");
#else
        ckCore::Path path5("c:\\this/is\\a/test");
        it = path5.Begin();

        TS_ASSERT_EQUALS(*it,"c:\\this");
        it++;
        TS_ASSERT_EQUALS(*it,"is\\a");
        it++;
        TS_ASSERT_EQUALS(*it,"test");
#endif

        // Iterator end limit.
        it = path1.Begin();
        ckCore::Path::Iterator end = path1.End();
        TS_ASSERT_EQUALS(it,it);
        it++;
        TS_ASSERT_EQUALS(it,it);
        it++;
        TS_ASSERT_EQUALS(it,it);
        it++;
        TS_ASSERT_EQUALS(it,it);
        it++;
        TS_ASSERT_EQUALS(it,end);
        ++it;
        TS_ASSERT_EQUALS(it,end);
    }
};

