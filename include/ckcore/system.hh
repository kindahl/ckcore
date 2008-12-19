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

/**
 * @file include/ckcore/system.hh
 * @brief Defines the system class.
 */

#pragma once
#include "ckcore/types.hh"

namespace ckcore
{
    /**
     * @brief System class.
     */
    class System
    {
    public:
        /**
         * Defines different cache levels.
         */
        enum CacheLevel
        {
            ckLEVEL_1 = 0x01,
            ckLEVEL_2,
            ckLEVEL_3
        };

    private:
        static void cpuid(unsigned long func,unsigned long arg,
                          unsigned long &a,unsigned long &b,
                          unsigned long &c,unsigned long &d);

        static unsigned long cache_size_intel(CacheLevel level);
        static unsigned long cache_size_amd(CacheLevel level);

    public:
        static tuint64 time();
        static tuint64 ticks();
        static unsigned long cache_size(CacheLevel level);
    };
};

