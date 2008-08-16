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
#include "../src/types.hh"
#include "../src/convert.hh"
#include "../src/string.hh"

class ConvertTestSuite : public CxxTest::TestSuite
{
public:
    void testConvert()
    {
		// Test boolean values.
		const ckcore::tchar *str11 = ckT("0");
		const ckcore::tchar *str12 = ckcore::convert::b_to_str(false);
		TS_ASSERT(!ckcore::string::compare(str11,str12));

		const ckcore::tchar *str21 = ckT("1");
		const ckcore::tchar *str22 = ckcore::convert::b_to_str(true);
		TS_ASSERT(!ckcore::string::compare(str21,str22));

		// Test 32-bit integer values.
		const ckcore::tchar *str31 = ckT("2147483647");
		const ckcore::tchar *str32 = ckcore::convert::i32_to_str(2147483647);
		TS_ASSERT(!ckcore::string::compare(str31,str32));

		const ckcore::tchar *str41 = ckT("-2147483647");
		const ckcore::tchar *str42 = ckcore::convert::i32_to_str(-2147483647);
		TS_ASSERT(!ckcore::string::compare(str41,str42));

		const ckcore::tchar *str51 = ckT("4294967295");
		const ckcore::tchar *str52 = ckcore::convert::ui32_to_str(4294967295UL);
		TS_ASSERT(!ckcore::string::compare(str51,str52));

		// Test 64-bit integer values.
		const ckcore::tchar *str61 = ckT("9223372036854775807");
		const ckcore::tchar *str62 = ckcore::convert::i64_to_str(9223372036854775807LL);
		TS_ASSERT(!ckcore::string::compare(str61,str62));

		const ckcore::tchar *str71 = ckT("-9223372036854775807");
		const ckcore::tchar *str72 = ckcore::convert::i64_to_str(-9223372036854775807LL);
		TS_ASSERT(!ckcore::string::compare(str71,str72));

		const ckcore::tchar *str81 = ckT("18446744073709551615");
		const ckcore::tchar *str82 = ckcore::convert::ui64_to_str(18446744073709551615ULL);
		TS_ASSERT(!ckcore::string::compare(str81,str82));
	}
};
