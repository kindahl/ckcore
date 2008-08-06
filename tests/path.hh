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
        ckCore::Path path1(ckT("/this/is/a/test"));
        ckCore::Path::Iterator it = path1.Begin();

        TS_ASSERT_EQUALS(*it,ckT("this"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("is"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("a"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("test"));

        ckCore::Path path2(ckT("/this/is/a/test/"));
        it = path2.Begin();

        TS_ASSERT_EQUALS(*it,ckT("this"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("is"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("a"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("test"));

        // Relative path.
        ckCore::Path path3(ckT("this/is/a/test"));
        it = path3.Begin();

        TS_ASSERT_EQUALS(*it,ckT("this"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("is"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("a"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("test"));

        // Windows paths.
        ckCore::Path path4(ckT("c:/this/is/a/test"));
        it = path4.Begin();

        TS_ASSERT_EQUALS(*it,ckT("c:"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("this"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("is"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("a"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("test"));

#ifdef _WINDOWS
        ckCore::Path path5(ckT("c:\\this/is\\a/test"));
        it = path5.Begin();

        TS_ASSERT_EQUALS(*it,ckT("c:"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("this"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("is"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("a"));
        ++it;
        TS_ASSERT_EQUALS(*it,ckT("test"));
#else
        ckCore::Path path5(ckT("c:\\this/is\\a/test"));
        it = path5.Begin();

        TS_ASSERT_EQUALS(*it,ckT("c:\\this"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("is\\a"));
        it++;
        TS_ASSERT_EQUALS(*it,ckT("test"));
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

        it = path2.Begin();
        end = path2.End();
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

    void testValid()
    {
        ckCore::Path path1(ckT("c:/this/is/a/test"));
        ckCore::Path path2(ckT("c::/this/is/a/test"));
        ckCore::Path path3(ckT("c:/this?/is/a/test"));
        ckCore::Path path4(ckT("c:/this</is/a/test"));
        ckCore::Path path5(ckT("c:/this>/is/a/test"));
        ckCore::Path path6(ckT("c:/this|/is/a/test"));
        ckCore::Path path7(ckT("c:/this\"/is/a/test"));
#ifdef _WINDOWS
        TS_ASSERT(path1.Valid());
        TS_ASSERT(!path2.Valid());
        TS_ASSERT(!path3.Valid());
        TS_ASSERT(!path4.Valid());
        TS_ASSERT(!path5.Valid());
        TS_ASSERT(!path6.Valid());
        TS_ASSERT(!path7.Valid());
#else
        TS_ASSERT(path1.Valid());
        TS_ASSERT(path2.Valid());
        TS_ASSERT(path3.Valid());
        TS_ASSERT(path4.Valid());
        TS_ASSERT(path5.Valid());
        TS_ASSERT(path6.Valid());
        TS_ASSERT(path7.Valid());
#endif
    }

    void testRootName()
    {
        ckCore::Path path1(ckT("/this/is/"));
#ifdef _WINDOWS
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("/this/is");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("/this");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("/");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("this/is/");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("c:/");
        TS_ASSERT_EQUALS(path1.RootName(),ckT("c:/"));

        path1 = ckT("c:/foo");
        TS_ASSERT_EQUALS(path1.RootName(),ckT("c:/"));

        path1 = ckT("c:\\");
        TS_ASSERT_EQUALS(path1.RootName(),ckT("c:\\"));
#else
        TS_ASSERT_EQUALS(path1.RootName(),ckT("/"));

        path1 = ckT("/this/is");
        TS_ASSERT_EQUALS(path1.RootName(),ckT("/"));

        path1 = ckT("/this");
        TS_ASSERT_EQUALS(path1.RootName(),ckT("/"));

        path1 = ckT("/");
        TS_ASSERT_EQUALS(path1.RootName(),ckT("/"));

        path1 = ckT("this/is/");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("c:\\");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));
#endif

        // Special cases.
        path1 = ckT("");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));

        path1 = ckT("foo");
        TS_ASSERT_EQUALS(path1.RootName(),ckT(""));
    }

    void testDirName()
    {
        ckCore::Path path1(ckT("/this/is/a/test"));
        TS_ASSERT_EQUALS(path1.DirName(),ckT("/this/is/a/"));

        ckCore::Path path2(ckT("/this/is/a/test/"));
        TS_ASSERT_EQUALS(path2.DirName(),ckT("/this/is/a/"));

        ckCore::Path path3(ckT("/this"));
        TS_ASSERT_EQUALS(path3.DirName(),ckT("/"));

        ckCore::Path path4(ckT("/this/is/a/test/"));
        TS_ASSERT_EQUALS(path4.DirName(),ckT("/this/is/a/"));

        ckCore::Path path5(ckT("this"));
        TS_ASSERT_EQUALS(path5.DirName(),ckT(""));

        ckCore::Path path6(ckT("c:/this"));
        TS_ASSERT_EQUALS(path6.DirName(),ckT("c:/"));

        ckCore::Path path7(ckT("c:\\this"));
        ckCore::Path path8(ckT("c:\\this\\"));
#ifdef _WINDOWS
        TS_ASSERT_EQUALS(path7.DirName(),ckT("c:\\"));
        TS_ASSERT_EQUALS(path8.DirName(),ckT("c:\\"));
#else
        TS_ASSERT_EQUALS(path7.DirName(),ckT(""));
        TS_ASSERT_EQUALS(path8.DirName(),ckT(""));
#endif
    }

    void testBaseName()
    {
        ckCore::Path path1(ckT("c:/this/is/a/test"));
        TS_ASSERT_EQUALS(path1.BaseName(),ckT("test"));

        ckCore::Path path2(ckT("c:/this/is/a/test/"));
        TS_ASSERT_EQUALS(path2.BaseName(),ckT("test"));

        ckCore::Path path3(ckT("c:/test"));
        TS_ASSERT_EQUALS(path3.BaseName(),ckT("test"));

        ckCore::Path path4(ckT("c:/"));
        TS_ASSERT_EQUALS(path4.BaseName(),ckT("c:"));

        ckCore::Path path5(ckT("/test"));
        TS_ASSERT_EQUALS(path5.BaseName(),ckT("test"));

        ckCore::Path path6(ckT("test"));
        TS_ASSERT_EQUALS(path6.BaseName(),ckT("test"));

        ckCore::Path path7(ckT("test/"));
        TS_ASSERT_EQUALS(path7.BaseName(),ckT("test"));

        ckCore::Path path8(ckT("c:\\this\\is\\a\\test"));
        ckCore::Path path9(ckT("c:\\this\\is\\a\\test/"));
        ckCore::Path path10(ckT("c:\\test"));
        ckCore::Path path11(ckT("c:\\"));
#ifdef _WINDOWS
        TS_ASSERT_EQUALS(path8.BaseName(),ckT("test"));
        TS_ASSERT_EQUALS(path9.BaseName(),ckT("test"));
        TS_ASSERT_EQUALS(path10.BaseName(),ckT("test"));
        TS_ASSERT_EQUALS(path11.BaseName(),ckT("c:"));
#else
        TS_ASSERT_EQUALS(path8.BaseName(),ckT("c:\\this\\is\\a\\test"));
        TS_ASSERT_EQUALS(path9.BaseName(),ckT("c:\\this\\is\\a\\test"));
        TS_ASSERT_EQUALS(path10.BaseName(),ckT("c:\\test"));
        TS_ASSERT_EQUALS(path11.BaseName(),ckT("c:\\"));
#endif
    }

    void testExtName()
    {
        ckCore::Path path1 = ckT("/this/is/a/test");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT(""));

        path1 = ckT("/this/is/a/test/");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT(""));

        path1 = ckT("/this/is/a/test.gz");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT("gz"));

        path1 = ckT("/this/is/a/test.tar.gz");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT("gz"));

        path1 = ckT("/this/is/a/.gz");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT("gz"));

        path1 = ckT("/this/is/a/.");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT(""));

        // Special cases.
        path1 = ckT(".");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT(""));

        path1 = ckT("");
        TS_ASSERT_EQUALS(path1.ExtName(),ckT(""));
    }

    void testCompare()
    {
        ckCore::Path path1(ckT("/this/is/a/test/"));
        ckCore::Path path2(ckT("/this/is/a/test"));
        TS_ASSERT(path1 == path2);
        TS_ASSERT(!(path1 != path2));

        ckCore::Path path3(ckT("/this/is/a/test_"));
        TS_ASSERT(path1 != path3);
        TS_ASSERT(!(path1 == path3));

        ckCore::Path path4(ckT("/this/is/a/test/foo"));
        TS_ASSERT(path1 != path4);
        TS_ASSERT(!(path1 == path4));

        ckCore::Path path5(ckT("/this/is/a/test/foo/"));
        TS_ASSERT(path1 != path5);
        TS_ASSERT(!(path1 == path5));

        ckCore::Path path6(ckT("/this/is/a"));
        TS_ASSERT(path1 != path6);
        TS_ASSERT(!(path1 == path6));

        ckCore::Path path7(ckT("/this/is/a/"));
        TS_ASSERT(path1 != path7);
        TS_ASSERT(!(path1 == path7));

        // Window paths.
        ckCore::Path path8(ckT("/this/is/a\\test/foo/"));
        ckCore::Path path9(ckT("/this/is/a\\"));

#ifdef _WINDOWS
        TS_ASSERT(path8 == path5);
        TS_ASSERT(!(path8 != path5));
        TS_ASSERT(path8 == path4);
        TS_ASSERT(!(path8 != path4));

        TS_ASSERT(path9 == path6);
        TS_ASSERT(!(path9 != path6));
        TS_ASSERT(path9 == path7);
        TS_ASSERT(!(path9 != path7));
#else
        TS_ASSERT(path8 != path5);
        TS_ASSERT(!(path8 == path5));
        TS_ASSERT(path8 != path4);
        TS_ASSERT(!(path8 == path4));

        TS_ASSERT(path9 != path6);
        TS_ASSERT(!(path9 == path6));
        TS_ASSERT(path9 != path7);
        TS_ASSERT(!(path9 == path7));
#endif
    }

    void testAssign()
    {
        ckCore::Path path1(ckT("/this/is/a/test"));
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));
        TS_ASSERT(path1 != ckT("/this/is/not/a/test"));

        path1 = ckT("/this/is/not/a/test");
        TS_ASSERT_EQUALS(path1,ckT("/this/is/not/a/test"));
        TS_ASSERT(path1 != ckT("/this/is/a/test"));

        ckCore::Path path2(ckT("/this/is/a/test"));
        path1 = path2;
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));
        TS_ASSERT(path1 != ckT("/this/is/not/a/test"));
    }

    void testAppend()
    {
        // + operator.
        ckCore::Path path1(ckT("/this/is"));
        ckCore::Path path2 = path1 + ckT("a/test");
        TS_ASSERT_EQUALS(path2,ckT("/this/is/a/test"));

        path2 = path1 + ckT("/a/test");
        TS_ASSERT_EQUALS(path2,ckT("/this/is/a/test"));

        path2 = path1 + ckT("/a/test/");
        TS_ASSERT_EQUALS(path2,ckT("/this/is/a/test"));

        path1 = ckT("/this/is/");
        path2 = path1 + ckT("/a/test/");
        TS_ASSERT_EQUALS(path2,ckT("/this/is/a/test"));

        path2 = path1 + ckT("a/test/");
        TS_ASSERT_EQUALS(path2,ckT("/this/is/a/test"));

        // += operator.
        path1 += ckT("a/test");
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));

        path1 = ckT("/this/is");
        path1 += ckT("/a/test");
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));

        path1 = ckT("/this/is");
        path1 += ckT("/a/test/");
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));

        path1 = ckT("/this/is/");
        path1 += ckT("/a/test/");
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));

        path1 = ckT("/this/is/");
        path1 += ckT("a/test/");
        TS_ASSERT_EQUALS(path1,ckT("/this/is/a/test"));
    }
};

