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
        ckCore::Path path1("c:/this/is/a/test");
        ckCore::Path path2("c::/this/is/a/test");
        ckCore::Path path3("c:/this?/is/a/test");
        ckCore::Path path4("c:/this</is/a/test");
        ckCore::Path path5("c:/this>/is/a/test");
        ckCore::Path path6("c:/this|/is/a/test");
        ckCore::Path path7("c:/this\"/is/a/test");
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

    void testDirName()
    {
        ckCore::Path path1("/this/is/a/test");
        TS_ASSERT_EQUALS(path1.DirName(),"/this/is/a/");

        ckCore::Path path2("/this/is/a/test/");
        TS_ASSERT_EQUALS(path2.DirName(),"/this/is/a/");

        ckCore::Path path3("/this");
        TS_ASSERT_EQUALS(path3.DirName(),"/");

        ckCore::Path path4("/this/is/a/test/");
        TS_ASSERT_EQUALS(path4.DirName(),"/this/is/a/");

        ckCore::Path path5("this");
        TS_ASSERT_EQUALS(path5.DirName(),"");

        ckCore::Path path6("c:/this");
        TS_ASSERT_EQUALS(path6.DirName(),"c:/");

        ckCore::Path path7("c:\\this");
        ckCore::Path path8("c:\\this\\");
#ifdef _WINDOWS
        TS_ASSERT_EQUALS(path7.DirName(),"c:\\");
        TS_ASSERT_EQUALS(path8.DirName(),"c:\\");
#else
        TS_ASSERT_EQUALS(path7.DirName(),"");
        TS_ASSERT_EQUALS(path8.DirName(),"");
#endif
    }

    void testBaseName()
    {
        ckCore::Path path1("c:/this/is/a/test");
        TS_ASSERT_EQUALS(path1.BaseName(),"test");

        ckCore::Path path2("c:/this/is/a/test/");
        TS_ASSERT_EQUALS(path2.BaseName(),"test");

        ckCore::Path path3("c:/test");
        TS_ASSERT_EQUALS(path3.BaseName(),"test");

        ckCore::Path path4("c:/");
        TS_ASSERT_EQUALS(path4.BaseName(),"c:");

        ckCore::Path path5("/test");
        TS_ASSERT_EQUALS(path5.BaseName(),"test");

        ckCore::Path path6("test");
        TS_ASSERT_EQUALS(path6.BaseName(),"test");

        ckCore::Path path7("test/");
        TS_ASSERT_EQUALS(path7.BaseName(),"test");

        ckCore::Path path8("c:\\this\\is\\a\\test");
        ckCore::Path path9("c:\\this\\is\\a\\test/");
        ckCore::Path path10("c:\\test");
        ckCore::Path path11("c:\\");
#ifdef _WINDOWS
        TS_ASSERT_EQUALS(path8.BaseName(),"test");
        TS_ASSERT_EQUALS(path9.BaseName(),"test");
        TS_ASSERT_EQUALS(path10.BaseName(),"test");
        TS_ASSERT_EQUALS(path11.BaseName(),"c:");
#else
        TS_ASSERT_EQUALS(path8.BaseName(),"c:\\this\\is\\a\\test");
        TS_ASSERT_EQUALS(path9.BaseName(),"c:\\this\\is\\a\\test");
        TS_ASSERT_EQUALS(path10.BaseName(),"c:\\test");
        TS_ASSERT_EQUALS(path11.BaseName(),"c:\\");
#endif
    }

    void testExtName()
    {
        ckCore::Path path1 = "/this/is/a/test";
        TS_ASSERT_EQUALS(path1.ExtName(),"");

        path1 = "/this/is/a/test/";
        TS_ASSERT_EQUALS(path1.ExtName(),"");

        path1 = "/this/is/a/test.gz";
        TS_ASSERT_EQUALS(path1.ExtName(),"gz");

        path1 = "/this/is/a/test.tar.gz";
        TS_ASSERT_EQUALS(path1.ExtName(),"gz");

        path1 = "/this/is/a/.gz";
        TS_ASSERT_EQUALS(path1.ExtName(),"gz");

        path1 = "/this/is/a/.";
        TS_ASSERT_EQUALS(path1.ExtName(),"");

        // Special cases.
        path1 = ".";
        TS_ASSERT_EQUALS(path1.ExtName(),"");

        path1 = "";
        TS_ASSERT_EQUALS(path1.ExtName(),"");
    }

    void testFoo()
    {
        ckCore::Path path("/this/is/a/test");
        ckCore::tstring foo;
        const ckCore::tchar *c_str = foo;
    }

    void testCompare()
    {
        ckCore::Path path1("/this/is/a/test/");
        ckCore::Path path2("/this/is/a/test");
        TS_ASSERT(path1 == path2);
        TS_ASSERT(!(path1 != path2));

        ckCore::Path path3("/this/is/a/test_");
        TS_ASSERT(path1 != path3);
        TS_ASSERT(!(path1 == path3));

        ckCore::Path path4("/this/is/a/test/foo");
        TS_ASSERT(path1 != path4);
        TS_ASSERT(!(path1 == path4));

        ckCore::Path path5("/this/is/a/test/foo/");
        TS_ASSERT(path1 != path5);
        TS_ASSERT(!(path1 == path5));

        ckCore::Path path6("/this/is/a");
        TS_ASSERT(path1 != path6);
        TS_ASSERT(!(path1 == path6));

        ckCore::Path path7("/this/is/a/");
        TS_ASSERT(path1 != path7);
        TS_ASSERT(!(path1 == path7));

        // Window paths.
        ckCore::Path path8("/this/is/a\\test/foo/");
        ckCore::Path path9("/this/is/a\\");

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
        ckCore::Path path1("/this/is/a/test");
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");
        TS_ASSERT(path1 != "/this/is/not/a/test");

        path1 = "/this/is/not/a/test";
        TS_ASSERT_EQUALS(path1,"/this/is/not/a/test");
        TS_ASSERT(path1 != "/this/is/a/test");

        ckCore::Path path2("/this/is/a/test");
        path1 = path2;
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");
        TS_ASSERT(path1 != "/this/is/not/a/test");
    }

    void testAppend()
    {
        // + operator.
        ckCore::Path path1("/this/is");
        ckCore::Path path2 = path1 + "a/test";
        TS_ASSERT_EQUALS(path2,"/this/is/a/test");

        path2 = path1 + "/a/test";
        TS_ASSERT_EQUALS(path2,"/this/is/a/test");

        path2 = path1 + "/a/test/";
        TS_ASSERT_EQUALS(path2,"/this/is/a/test");

        path1 = "/this/is/";
        path2 = path1 + "/a/test/";
        TS_ASSERT_EQUALS(path2,"/this/is/a/test");

        path2 = path1 + "a/test/";
        TS_ASSERT_EQUALS(path2,"/this/is/a/test");

        // += operator.
        path1 += "a/test";
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");

        path1 = "/this/is";
        path1 += "/a/test";
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");

        path1 = "/this/is";
        path1 += "/a/test/";
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");

        path1 = "/this/is/";
        path1 += "/a/test/";
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");

        path1 = "/this/is/";
        path1 += "a/test/";
        TS_ASSERT_EQUALS(path1,"/this/is/a/test");
    }
};

