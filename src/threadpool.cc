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

#include <cassert>
#include "ckcore/locker.hh"
#include "ckcore/thread.hh"
#include "ckcore/threadpool.hh"

namespace ckcore
{
    /**
     * Constructs an internal thread object.
     * @param [in] host The hosting thread pool object.
     * @param [in] task The task to execute in the thread.
     */
    ThreadPool::InternalThread::InternalThread(ThreadPool &host,Task *task)
        : host_(host),task_(task)
    {
    }

    /**
     * Executes the thread.
     */
    void ThreadPool::InternalThread::run()
    {
        Locker<thread::Mutex> lock(host_.mutex_);

        while (true)
        {
            // Check if we have a task to execute.
            while (task_ != NULL)
            {
                lock.unlock();

                try
                {
                    task_->start();
                }
                catch (...)
                {
                }

                lock.relock();

                if (task_->auto_delete())
                    delete task_;
                task_ = NULL;

                // Don't fetch new tasks if we're overworking.
                if (host_.overworking())
                    break;

                // If we have more tasks in the queue, pick the first one and
                // execute it.
                if (!host_.queue_.empty())
                {
                    task_ = host_.queue_.top().first;
                    host_.queue_.pop();
                }
            }

            if (host_.exiting_)
            {
                host_.pol_threads_--;
                return;
            }

            bool expired = host_.overworking();
            if (!expired)
            {
                host_.idl_threads_++;

                host_.pol_threads_--;
                expired = !host_.task_ready_.wait(host_.mutex_,host_.ret_timeout_);
                host_.pol_threads_++;

                if (expired)
                    host_.idl_threads_--;
            }

            if (expired)
            {
                host_.ret_threads_.push_back(this);
                host_.pol_threads_--;
                return;
            }

            // We're ready for some more work, if there is any.
            if (!host_.queue_.empty())
            {
                task_ = host_.queue_.top().first;
                host_.queue_.pop();
            }
        }
    }

    /**
     * Constructs a thread pool object. The pool will configure itself to
     * handle the ideal number of threads for the current system. That is the
     * ideal number of threads the system can execute in parallel.
     */
    ThreadPool::ThreadPool()
        : exiting_(false),
          max_threads_(thread::ideal_count()),pol_threads_(0),res_threads_(0),idl_threads_(0),
          ret_timeout_(THREAD_RETIRE_TIMEOUT)
    {
    }

    /**
     * Destructs the thread pool object.
     */
    ThreadPool::~ThreadPool()
    {
        // Wait for all tasks to complete.
        wait();
    }

    /**
     * Returns the single instance to the thread pool.
     * @return The single instance to the thread pool.
     */
    ThreadPool &ThreadPool::instance()
    {
        static ThreadPool instance;
        return instance;
    }

    /**
     * Returns the total number of active threads in the application. This is
     * the number of threads used by the pool plus the number of reserved
     * threads.
     * @return The total number of active threads.
     */
    tuint32 ThreadPool::active_threads() const
    {
        return static_cast<tuint32>(all_threads_.size()) + res_threads_ -
               static_cast<tuint32>(ret_threads_.size()) - idl_threads_;
    }

    /**
     * Returns the number of idle threads.
     * @return The number of idle threads.
     */
    tuint32 ThreadPool::idle_threads() const
    {
        return idl_threads_;
    }

    /**
     * Returns the number of retired threads.
     * @return The number of retired threads.
     */
    tuint32 ThreadPool::retired_threads() const
    {
        return static_cast<tuint32>(ret_threads_.size());
    }

    /**
     * Returns the number of queued tasks that are not yet assigned to a
     * thread.
     * @return The number of queued tasks.
     */
    tuint32 ThreadPool::queued() const
    {
        return static_cast<tuint32>(queue_.size());
    }

    /**
     * Puts a task into the work queue.
     * @param [in] task Task to enqueue.
     * @param [in] priority Task priority.
     */
    void ThreadPool::enqueue(Task *task,tuint32 priority)
    {
        queue_.push(std::make_pair(task,priority));

        // Signal one thread to start processing the top priority task.
        task_ready_.signal_one();
    }

    /**
     * Spawn a new thread and start executing the task.
     * @param [in] task The task to execute.
     * @return If the task could not be spawned in a new thread false is
     *         returned, otherwise true is returned.
     */
    bool ThreadPool::spawn(Task *task)
    {
        InternalThread *thread = new InternalThread(*this,task);

        all_threads_.push_back(thread);
        pol_threads_++;

        return thread->start();
    }

    /**
     * Check if we're currently serving more threads than we should. This may
     * happen if threads are reserved while executing tasks.
     * @return If we'we overworking true is returned, if not false is returned.
     */
    bool ThreadPool::overworking() const
    {
        return active_threads() > max_threads_;
    }

    /**
     * Tries to start the specified task immediately. If that's not possible
     * the function will fail.
     * @param [in] task The task to execute.
     * @return If the task was started true is returned, otherwise false is
     *         returned.
     */
    bool ThreadPool::try_start(Task *task)
    {
        // Check if we have any free thread so that the task can start
        // immediately.
        if (active_threads() >= max_threads_)
            return false;

        // See if there is an idle thread waiting, in that case enqueue the
        // task.
        if (idl_threads_ > 0)
        {
            idl_threads_--;
            enqueue(task);
            return true;
        }

        // See if there are any retired threads that can be restarted.
        if (!ret_threads_.empty())
        {
            InternalThread *thread = ret_threads_.back();
            ret_threads_.pop_back();

            pol_threads_++;
            
            assert(thread->task_ == NULL);
            thread->task_ = task;
            thread->start();
            return true;
        }

        // Spawn a new thread for the task,
        return spawn(task);
    }

    /**
     * Tries to start the specified task immediately, if that's not possible
     * it will be queued with the specified task priority.
     * @param [in] task The task to execute.
     * @param [in] priority The task priority.
     * @return If successful true is returned, otherwise false is returned.
     */
    bool ThreadPool::start(Task *task,tuint32 priority)
    {
        if (task == NULL)
            return false;

        // Try to task the task immediately, if not enqueue it.
        Locker<thread::Mutex> lock(mutex_);
        if (!try_start(task))
            enqueue(task,priority);

        return true;
    }

    /**
     * Executes the specified task immediatly if there is a free thread
     * available. If there is no free thread available so that the task can
     * execute right away the function will fail and the task will not be
     * queued.
     * @param [in] task The task to execute.
     * @return If the task was executed right away true is returned, if not
     *         false is returned.
     */
    bool ThreadPool::start_now(Task *task)
    {
        if (task == NULL)
            return false;

        // Quick check to see if we're choked.
        if (active_threads() > max_threads_)
            return false;

        Locker<thread::Mutex> lock(mutex_);
        return try_start(task);
    }

    /**
     * Waits for all tasks to finish and shutdown all threads essentially
     * restoing the thread pool. This does not reset the number of reserved
     * threads.
     */
    void ThreadPool::wait()
    {
        Locker<thread::Mutex> lock(mutex_);

        // Signal all threads that a task is ready, which there is not and thus
        // will cause them to shutdown.
        exiting_ = true;

        task_ready_.signal_all();

        // We need to loop on the threads here since new threads might possibly
        // be added while waiting for the existing threads to finish.
        while (!all_threads_.empty())
        {
            // Make a copy of the threads so that we can wait for all threads
            // in parallel.
            std::vector<InternalThread *> all_threads = all_threads_;
            all_threads_.clear();

            lock.unlock();

            std::vector<InternalThread *>::iterator it_thread;
            for (it_thread = all_threads.begin(); it_thread != all_threads.end(); it_thread++)
            {
                InternalThread *thread = *it_thread;
                thread->wait();
                delete thread;
            }

            lock.relock();
        }

        ret_threads_.clear();

        pol_threads_ = 0;
        idl_threads_ = 0;

        exiting_ = false;
    }

    /**
     * Reserve a number of threads for use outside the thread pool.
     * @param [in] num_threads Number of threads to reserve.
     */
    void ThreadPool::reserve(tuint32 num_threads)
    {
        Locker<thread::Mutex> lock(mutex_);

        bool start_task = num_threads < res_threads_;

        res_threads_ = num_threads > max_threads_ ? max_threads_ : num_threads;

        // Start a previously blocked task.
        if (start_task && !queue_.empty())
        {
            std::pair<Task *,tuint32> entry = queue_.top();
            queue_.pop();

            if (!try_start(entry.first))
                queue_.push(entry);
        }
    }

    /**
     * Sets the timeout before retiring idle threads.
     * @param [in] timeout New timeout in milliseconds.
     */
    void ThreadPool::set_retire_timeout(tuint32 timeout)
    {
        Locker<thread::Mutex> lock(mutex_);
        ret_timeout_ = timeout;
    }
}
