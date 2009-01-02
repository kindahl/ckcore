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

#include <queue>
#include <cxxtest/TestSuite.h>
#include "ckcore/types.hh"
#include "ckcore/process.hh"

class ProcessWrapper : public ckcore::Process
{
private:
    std::queue<std::string> line_buffer_;

public:
    ~ProcessWrapper()
    {
        invalid_inheritor_ = true;
    }

    void event_finished()
    {
    }

    void event_output(const std::string &line)
    {
        line_buffer_.push(line);
    }

    std::string next()
    {
        if (line_buffer_.size() == 0)
            return "";

        std::string result = line_buffer_.front();
        line_buffer_.pop();
        return result;
    }
};

class ProcessTestSuite : public CxxTest::TestSuite
{
public:
    void testFragmented()
    {
        ProcessWrapper process;

        std::vector<ckcore::tstring> args;

        TS_ASSERT(!process.running());
        TS_ASSERT(process.create(ckT("./bin/smallclient"),args));
        process.wait();
        TS_ASSERT_SAME_DATA(process.next().c_str(),"SmallClient",12);
        TS_ASSERT_SAME_DATA(process.next().c_str(),"MESSAGE 1",9);
	}

    void testInterleaved()
    {
        ProcessWrapper process;

        std::vector<ckcore::tstring> args;
        args.push_back(ckT("-m2"));

        TS_ASSERT(!process.running());
        TS_ASSERT(process.create(ckT("./bin/smallclient"),args));
        process.wait();
        TS_ASSERT_SAME_DATA(process.next().c_str(),"SmallClient",12);
        TS_ASSERT_SAME_DATA(process.next().c_str(),"MESSAGE 1",9);
        TS_ASSERT_SAME_DATA(process.next().c_str(),"MESSAGE 2",9);
	}

    void testBadExec()
    {
        ProcessWrapper process;

        std::vector<ckcore::tstring> args;
        args.push_back(ckT("-l"));

        TS_ASSERT(!process.running());
        TS_ASSERT(process.create(ckT("ls"),args));
        TS_ASSERT(!process.running());
	}
};

