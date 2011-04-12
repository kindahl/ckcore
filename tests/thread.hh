/*
 * The ckCore library provides core software functionality.
 * Copyright (C) 2006-2011 Christian Kindahl
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
#include "ckcore/types.hh"
#include "ckcore/thread.hh"

class TestThread1 : public ckcore::Thread
{
private:
    void run()
    {
        result_++;
    }

public:
    int result_;

    TestThread1() : result_(0) {}
};

class TestThread2 : public ckcore::Thread
{
private:
    void run()
    {
        ckcore::thread::sleep(20);
        result_++;
    }

public:
    int result_;

    TestThread2() : result_(0) {}
};

class ThreadTestSuite : public CxxTest::TestSuite
{
public:
    void testThreadReuse()
    {
        TestThread1 thread;
        TS_ASSERT_EQUALS(thread.result_,0);
        TS_ASSERT(thread.start());
        TS_ASSERT(thread.running());
        ckcore::thread::sleep(20);
        TS_ASSERT_EQUALS(thread.result_,1);

        for (size_t i = 0; i < 9; i++)
        {
            thread.start();
            thread.wait();
        }

        TS_ASSERT_EQUALS(thread.result_,10);
    }

    void testThreadWait()
    {
        TestThread2 thread;
        TS_ASSERT_EQUALS(thread.result_,0);
        TS_ASSERT(thread.start());
        TS_ASSERT(thread.running());
        TS_ASSERT(thread.wait());
        TS_ASSERT(!thread.running());
        TS_ASSERT_EQUALS(thread.result_,1);
    }

    void testThreadKill()
    {
        TestThread2 thread;
        TS_ASSERT_EQUALS(thread.result_,0);
        TS_ASSERT(thread.start());
        TS_ASSERT(thread.running());
        TS_ASSERT(thread.kill());
        TS_ASSERT(!thread.running());
        TS_ASSERT_EQUALS(thread.result_,0);

        // Test that a killed thread can be restarted.
        TS_ASSERT(thread.start());
        TS_ASSERT(thread.running());
        TS_ASSERT(thread.wait());
        TS_ASSERT_EQUALS(thread.result_,1);
    }
};
