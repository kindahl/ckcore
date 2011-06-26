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
 * @file include/ckcore/threadpool.hh
 * @brief Thread pool interface.
 */

#pragma once
#include <queue>
#include "ckcore/types.hh"
#include "ckcore/task.hh"
#include "ckcore/thread.hh"

namespace ckcore
{
    /**
     * @brief Thread pool singleton class.
     */
    class ThreadPool
    {
    public:
        /**
         * @brief Defines constants specifying the class behaviour.
         */
        enum
        {
            THREAD_RETIRE_TIMEOUT = 20000   ///< How long an idle thread will wait for a new task before retiring.
        };

    private:
        /**
         * @brief Internal thread class.
         */
        class InternalThread : public Thread
        {
        private:
            ThreadPool &host_;

            /*
             * Thread Interface
             */
            void run();

        public:
            Task *task_;

            InternalThread(ThreadPool &host,Task *task);
        };

    private:
        bool exiting_;          ///< Set to true when thread pool is exiting.
        const tuint32 max_threads_;   ///< Maximum number of threads.
        tuint32 pol_threads_;   ///< Number of active threads in the pool.
        tuint32 res_threads_;   ///< Number of reserved threads.
        tuint32 idl_threads_;   ///< Number of idle threads.
        thread::Mutex mutex_;

        thread::WaitCondition task_ready_;          ///< Signaled to a thread when a task is ready for execution.

        std::vector<InternalThread *> all_threads_; ///< All threads.
        std::vector<InternalThread *> ret_threads_; ///< Retired threads.

        tuint32 ret_timeout_;   ///< How long a thread can indle before being retired.

        std::priority_queue<std::pair<Task *,tuint32> > queue_;

        void enqueue(Task *task,tuint32 priority = 0);
        bool spawn(Task *task);
        bool overworking() const;
        bool try_start(Task *);

        ThreadPool();
        ThreadPool(const ThreadPool &rhs);
        ~ThreadPool();
        ThreadPool &operator=(const ThreadPool &rhs);

    public:
        static ThreadPool &instance();

        tuint32 active_threads() const;
        tuint32 idle_threads() const;
        tuint32 retired_threads() const;
        tuint32 queued() const;

        bool start(Task *task,tuint32 priority = 0);
        bool start_now(Task *task);
        void wait();
        void reserve(tuint32 num_threads);

        void set_retire_timeout(tuint32 timeout);
    };
}

