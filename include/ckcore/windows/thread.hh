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

/**
 * @file include/ckcore/windows/thread.hh
 * @brief Defines the Windows thread implementation.
 */
#pragma once
#include <limits>
#include <windows.h>
#include "ckcore/types.hh"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace ckcore
{
    namespace thread
    {
        bool create(tfunction func,void *param);
        bool sleep(tuint32 milliseconds);
        tuint32 ideal_count();
        thandle identifier();

        /**
         * @brief Thead mutex class.
         */
        class Mutex
        {
        public:
            friend class WaitCondition;

        private:
            HANDLE handle_;

        public:
            Mutex();
            ~Mutex();

            bool lock();
            bool unlock();
        };

        /**
         * @brief Wait condition class.
         */
        class WaitCondition
        {
        private:
            bool broadcast_;
            tuint32 waiters_;
            CRITICAL_SECTION critical_;
            HANDLE sema_;
            HANDLE waiters_done_;

        public:
            WaitCondition();
            ~WaitCondition();

            bool wait(Mutex &mutex,tuint32 timeout = std::numeric_limits<tuint32>::max());
            void signal_one();
            void signal_all();
        };
    };

    /**
     * @brief Thread class.
     */
    class Thread
    {
    private:
        HANDLE thread_;
        bool running_;
        mutable thread::Mutex mutex_;
        thread::WaitCondition thread_done_;

        static unsigned long __stdcall native_thread(void *param);

    protected:
        virtual void run() = 0;

    public:
        Thread();
        ~Thread();

        bool start();
        bool wait(tuint32 timeout = std::numeric_limits<tuint32>::max());
        bool kill();
        bool running() const;
    };
};

