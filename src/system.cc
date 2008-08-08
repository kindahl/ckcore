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

#include <string.h>
#ifdef _LINUX
#include <sys/time.h>
#endif
#include "system.hh"

namespace ckCore
{
    /**
     * Returns the number of milliseconds that has elapsed since the system was
     * started.
     * @return The number of milliseconds since the system was started.
     */
    tuint64 System::Time()
    {
#ifdef _WINDOWS
        return GetTickCount64();
#else
        struct timeval time;
        gettimeofday(&time,(struct timezone *)0);
        return (tuint64)(time.tv_sec * 1000 + (time.tv_usec / 1000));
#endif
    }

    /**
     * Returnes the number of clock cycles executed by the host processor since
     * the system was started.
     * @return The number of executed clock cycles since the system was
     *         started.
     */
    tuint64 System::Ticks()
    {
#ifdef _WINDOWS
        return __rdtsc();
#else
        unsigned long low = 0,high = 0;

        asm("rdtsc"
            :"=a"(low),"=d"(high)
            :
            :);

        tuint64 result = high;
        result <<= 32;
        result |= low;
        return result;
#endif
    }

    void System::Cpuid(unsigned long func,unsigned long arg,
                       unsigned long &a,unsigned long &b,
                       unsigned long &c,unsigned long &d)
    {
#ifdef _WINDOWS
        __asm
        {
            mov eax,func
            cpuid
            mov dword a,eax
            mov dword b,ebx
            mov dword c,ecx
            mov dword d,edx
        };
#else
        asm("cpuid"
            :"=a"(a),
             "=b"(b),
             "=c"(c),
             "=d"(d)
            :"a"(func),
             "c"(arg));
#endif
    }

    /**
     * Determines the size of the specified cache. This function will only be
     * able to obtain the cache sizes on Intel systems.
     * @return If successfull the size of the cache is returned in bytes, if
     *         unsuccessfull 0 is returned.
     */
    unsigned long System::CacheIntel(CacheLevel level)
    {
        unsigned long reg = 0;
        while (true)
        {
            unsigned long a,b,c,d;
            Cpuid(4,reg++,a,b,c,d);

            // Check if we have found the last cache.
            unsigned char cur_type = a & 0x1f;
            if (cur_type == 0)
                break;

            // We're only interested in the level 1 data cache.
            unsigned char cur_level = (a >> 5) & 0x07;
            if ((cur_type == 1 || cur_type == 3) && cur_level == level)
            {
                unsigned long ways = (b >> 22) & 0x3ff;
                unsigned long part = (b >> 12) & 0x3ff;
                unsigned long line = b & 0xfff;
                unsigned long sets = c;

                return (ways + 1) * (part + 1) * (line + 1) * (sets + 1);
            }
        }

        return 0;
    }

    /**
     * Determines the size of the specified cache. This function will only be
     * able to obtain the cache sizes on AMD systems.
     * @return If successfull the size of the cache is returned in bytes, if
     *         unsuccessfull 0 is returned.
     */
    unsigned long System::CacheAmd(CacheLevel level)
    {
        unsigned long a,b,c,d;

        if (level == System::ckLEVEL_1)
        {
            Cpuid(0x80000005,0,a,b,c,d);
            return ((c >> 24) & 0xff) * 1024;
        }
        else if (level == System::ckLEVEL_2)
        {
            Cpuid(0x80000006,0,a,b,c,d);
            return ((c >> 16) & 0xffff) * 1024;
        }

        // Level 3 can not be determined exactly.
        return 0;
    }

    /**
     * Determines the size of the specified cache. This function will only be
     * able to obtain the cache sizes on AMD and Intel systems.
     * @return If successfull the size of the cache is returned in bytes, if
     *         unsuccessfull 0 is returned.
     */
    unsigned long System::Cache(CacheLevel level)
    {
        // Obtain processor vendor identifier.
        unsigned long a,b,c,d;
        Cpuid(0,0,a,b,c,d);

        char vendor[13];
        memcpy(vendor    ,&b,4);
        memcpy(vendor + 4,&d,4);
        memcpy(vendor + 8,&c,4);
        vendor[12] = '\0';

        if (!strcmp(vendor,"GenuineIntel"))
            return CacheIntel(level);
        else if (!strcmp(vendor,"AuthenticAMD"))
            return CacheAmd(level);

        return 0;
    }
};

