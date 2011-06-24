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
#include "ckcore/locker.hh"
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

class TestThread3 : public ckcore::Thread
{
private:
    int &value_;
    ckcore::thread::Mutex &mutex_;

    void run()
    {
        ckcore::thread::sleep(20);

        ckcore::Locker<ckcore::thread::Mutex> lock(mutex_);

        for (int i = 0; i < 1024; i++)
            value_++;
    }

public:
    TestThread3(int &value,ckcore::thread::Mutex &mutex) : value_(value),mutex_(mutex) {}
};

class ThreadTestSuite : public CxxTest::TestSuite
{
public:
    void testThreadReuse()
    {
        TestThread1 thread;
        TS_ASSERT_EQUALS(thread.result_,0);
        TS_ASSERT(thread.start());
        //TS_ASSERT(thread.running());
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

    void testThreadMutex()
    {
        // This test is based on the idea that it's unlikeley that a large
        // number of threads reading/writing to the same memory will generate
        // expected results without a proper locking mechanism.
        int value = 0;
        ckcore::thread::Mutex mutex;

        TestThread3 thread[32] =
        {
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),
            TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex),TestThread3(value,mutex)
        };

        for (size_t i = 0; i < 32; i++)
        {
            thread[i].start();
        }

        // Wait for threads to finish.
        for (size_t i = 0; i < 32; i++)
        {
            while (thread[i].running())
                ckcore::thread::sleep(20);
        }

        // Verify result.
        TS_ASSERT_EQUALS(value,32 * 1024);
    }
};
