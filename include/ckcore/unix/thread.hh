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
 * @file include/ckcore/unix/thread.hh
 * @brief Defines the Unix thread implementation.
 */
#pragma once
#include <limits>
#include "ckcore/types.hh"
#include "ckcore/locker.hh"

namespace ckcore
{
    namespace thread
    {
        bool create(tfunction func,void *param);
        bool sleep(ckcore::tuint32 milliseconds);
        tuint32 ideal_count();
        thandle identifier();

        /**
         * @brief Thead mutex class.
         */
        class Mutex
        {
        private:
            pthread_mutex_t mutex_;

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
            tuint32 waiters_;
            tuint32 wakeups_;
            pthread_mutex_t mutex_;
            pthread_cond_t cond_;

            bool wait(tuint32 timeout);

        public:
            WaitCondition();
            ~WaitCondition();

            bool wait(Mutex &mutex,tuint32 timeout = std::numeric_limits<tuint32>::max());
            void signal_one();
            void signal_all();
        };
    }

    /**
     * @brief Thread class.
     */
    class Thread
    {
    private:
        pthread_t thread_;
        bool running_;
        mutable thread::Mutex mutex_;
        thread::WaitCondition thread_done_;

        static void *native_thread(void *param);
        static void cleanup(void *param);

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
}

