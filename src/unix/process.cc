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

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <map>
#include "ckcore/stream.hh"
#include "ckcore/process.hh"

namespace ckcore
{
    /**
     * Singleton class for monitoring child processes.
     */
    class ProcessMonitor
    {
    private:
        void (*old_sigchld_handler_)(int);
        std::map<pid_t,Process *> pid_map_;

        ProcessMonitor() : old_sigchld_handler_(NULL)
        {
            // Assign a action handler for the SIGCHLD signal.
            struct sigaction new_action,old_action;
            memset(&new_action,0,sizeof(new_action));

            new_action.sa_handler = sigchld_handler;
            new_action.sa_flags = SA_NOCLDSTOP;

            if (sigaction(SIGCHLD,&new_action,&old_action) == 0)
            {
                // Remember the old handler so we can restore it when done.
                if (old_action.sa_handler != sigchld_handler)
                    old_sigchld_handler_ = old_action.sa_handler;
            }
        }

        ~ProcessMonitor()
        {
            // Restore the old SIGCHLD signal handler.
            struct sigaction new_action,old_action;
            memset(&new_action,0,sizeof(new_action));

            new_action.sa_handler = old_sigchld_handler_;
            new_action.sa_flags = SA_NOCLDSTOP;

            if (sigaction(SIGCHLD,&new_action,&old_action) == 0)
            {
                // If the old signal handler was not our new one, switch back.
                if (old_action.sa_handler != sigchld_handler) {
                    sigaction(SIGCHLD,&old_action,0);
                }
            }
        }

        ProcessMonitor(const ProcessMonitor &rhs);
        ProcessMonitor &operator=(const ProcessMonitor &rhs);

        static void sigchld_handler(int signum)
        {
            // Obtain process identifier.
            int status;
            pid_t pid = wait(&status);

            // Change process status to stopped.
            if (ProcessMonitor::instance().pid_map_.count(pid) > 0)
                ProcessMonitor::instance().pid_map_[pid]->state_ = Process::STATE_STOPPED;

            // Call the old SIGCHLD signal handler.
            void (*old_sigchld_handler)(int) = ProcessMonitor::instance().old_sigchld_handler_;
            if (old_sigchld_handler != NULL && old_sigchld_handler != SIG_IGN)
                old_sigchld_handler(signum);
        }

    public:
        /**
         * Returns the process monitor instance.
         * @return The process monitor instance.
         */
        static ProcessMonitor &instance()
        {
            static ProcessMonitor instance;
            return instance;
        }

        /**
         * Registers a new process in the process monitor.
         * @param [in] pid The process identifier of the process to monitor.
         */
        void register_process(pid_t pid,Process *process)
        {
            pid_map_[pid] = process;
        }
    };

    /**
     * Constructs a Process object.
     */
    Process::Process() : invalid_inheritor_(false),pid_(-1),state_(STATE_STOPPED)
    {
        pipe_stdin_[0] = pipe_stdin_[1] = -1;
        pipe_stdout_[0] = pipe_stdout_[1] = -1;
        pipe_stderr_[0] = pipe_stderr_[1] = -1;

        pthread_mutex_init(&mutex_,NULL);
        pthread_mutex_init(&mutex_exec_,NULL);
    }

    /**
     * Destructs the Process object.
     */
    Process::~Process()
    {
        // Make sure that the execution is completed before destroying this object.
        bool locked = pthread_mutex_lock(&mutex_exec_);
        if (locked)
            pthread_mutex_unlock(&mutex_exec_);

        close();

        pthread_mutex_destroy(&mutex_exec_);
        pthread_mutex_destroy(&mutex_);
    }

    /**
     * Closes all internal pipes and resets the internal state of the object.
     */
    void Process::close()
    {
        bool locked = pthread_mutex_lock(&mutex_) == 0;

        // Close handles.
        for (int i = 0; i < 2; i++)
        {
            // Close STDIN.
            if (pipe_stdin_[i] != -1)
            {
                ::close(pipe_stdin_[i]);
                pipe_stdin_[i] = -1;
            }

            // Close STDOUT.
            if (pipe_stdout_[i] != -1)
            {
                ::close(pipe_stdout_[i]);
                pipe_stdout_[i] = -1;
            }

            // Close STDERR.
            if (pipe_stderr_[i] != -1)
            {
                ::close(pipe_stderr_[i]);
                pipe_stderr_[i] = -1;
            }
        }

        // Reset state.
        pid_ = -1;
        state_ = STATE_STOPPED;

        if (locked)
            pthread_mutex_unlock(&mutex_);
    }

    /**
     * Reads from the specified file descriptor to the standard output buffer.
     * @return If successful true is returned, if unsuccessful false is
     *         returned.
     */
    bool Process::read_stdout(int fd)
    {
        char buffer[READ_BUFFER_SIZE];
        ssize_t read_bytes = ::read(fd,buffer,sizeof(buffer) - 1);

        // Check for read errors.
        if (read_bytes <= 0)
            return false;

        buffer[read_bytes] = '\0';

        // Split the buffer into lines.
        for (ssize_t i = 0; i < read_bytes; i++)
        {
            if (buffer[i] == '\n' || buffer[i] == '\r')
            {
                // Avoid flushing an empty buffer.
                if (line_buffer_out_.size() > 0)
                {
                    if (!invalid_inheritor_)
                        event_output(line_buffer_out_);

                    line_buffer_out_.resize(0);
                }
            }
            else
            {
                line_buffer_out_.push_back(buffer[i]);
            }
        }

        return true;
    }

    /**
     * Reads from the specified file descriptor to the standard error buffer.
     * @return If successful true is returned, if unsuccessful false is
     *         returned.
     */
    bool Process::read_stderr(int fd)
    {
        char buffer[READ_BUFFER_SIZE];
        ssize_t read_bytes = ::read(fd,buffer,sizeof(buffer) - 1);

        // Check for read errors.
        if (read_bytes <= 0)
            return false;

        buffer[read_bytes] = '\0';

        // Split the buffer into lines.
        for (ssize_t i = 0; i < read_bytes; i++)
        {
            if (buffer[i] == '\n' || buffer[i] == '\r')
            {
                // Avoid flushing an empty buffer.
                if (line_buffer_err_.size() > 0)
                {
                    if (!invalid_inheritor_)
                        event_output(line_buffer_err_);

                    line_buffer_err_.resize(0);
                }
            }
            else
            {
                line_buffer_err_.push_back(buffer[i]);
            }
        }

        return true;
    }

    /**
     * The thread entry for listening on the process output pipes.
     * @param [in] param A pointer to the Process object being executed.
     */
    void *Process::listen(void *param)
    {
        Process *process = static_cast<Process *>(param);

        // Prevent the object from being destroyed while running the separate thread.
        pthread_mutex_lock(&process->mutex_exec_);

        process->line_buffer_out_.resize(0);
        process->line_buffer_err_.resize(0);

        // Loop while the program is running.
        while (process->state_ == STATE_RUNNING)
        {
            fd_set read_set;
            FD_ZERO(&read_set);

            if (process->pipe_stdout_[FD_READ] != -1)
                FD_SET(process->pipe_stdout_[FD_READ],&read_set);
            if (process->pipe_stderr_[FD_READ] != -1)
                FD_SET(process->pipe_stderr_[FD_READ],&read_set);

            int timeout = 100;  // 100 miliseconds.
            struct timeval tv;
            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;

            int res;
            do
            {
                res = select(FD_SETSIZE,&read_set,NULL,NULL,&tv);
            } while (res < 0 && errno == EINTR);

            // Read from standard output.
            if (FD_ISSET(process->pipe_stdout_[FD_READ],&read_set))
            {
                if (!process->read_stdout(process->pipe_stdout_[FD_READ]))
                {
                    process->close();
                    break;
                }
            }

            // Read from standard error.
            if (FD_ISSET(process->pipe_stderr_[FD_READ],&read_set))
            {
                if (!process->read_stderr(process->pipe_stderr_[FD_READ]))
                {
                    process->close();
                    break;
                }
            }
        }

        // Notify that the process has finished.
        if (!process->invalid_inheritor_)
            process->event_finished();

        process->close();

        pthread_mutex_unlock(&process->mutex_exec_);
    }

    /**
     * Creates a new process.
     * @param [in] path Path to the file to execute.
     * @param [in] args A list of program arguments to pass to the executable.
     * @return If successful true is returned, if unsuccessful false is
     *         returned.
     */
    bool Process::create(const tchar *path,std::vector<tstring> &args)
    {
        // Check if a process is already running.
        if (running())
            return false;

        // Close prevous data.
        close();

        // Prepare argument list.
        char *arg_list[MAX_ARG_COUNT + 2];
        size_t num_args = args.size() > MAX_ARG_COUNT ? MAX_ARG_COUNT : args.size();

        arg_list[0] = (char *)path;
        
        for (size_t i = 0; i < num_args; i++)
            arg_list[i + 1] = (char *)args[i].c_str();

        arg_list[num_args + 1] = (char *)NULL;

        // Create pipes.
        if (pipe(pipe_stdin_) == -1 || pipe(pipe_stdout_) == -1 || pipe(pipe_stderr_) == -1)
            return false;

        fcntl(pipe_stdin_[FD_READ],F_SETFL,fcntl(pipe_stdin_[FD_READ],F_GETFL) | O_NONBLOCK);
        fcntl(pipe_stdin_[FD_WRITE],F_SETFL,fcntl(pipe_stdin_[FD_WRITE],F_GETFL) | O_NONBLOCK);
        fcntl(pipe_stdout_[FD_READ],F_SETFL,fcntl(pipe_stdout_[FD_READ],F_GETFL) | O_NONBLOCK);
        fcntl(pipe_stdout_[FD_WRITE],F_SETFL,fcntl(pipe_stdout_[FD_WRITE],F_GETFL) | O_NONBLOCK);
        fcntl(pipe_stderr_[FD_READ],F_SETFL,fcntl(pipe_stderr_[FD_READ],F_GETFL) | O_NONBLOCK);
        fcntl(pipe_stderr_[FD_WRITE],F_SETFL,fcntl(pipe_stderr_[FD_WRITE],F_GETFL) | O_NONBLOCK);

        // Change state to running (this will change on failure).
        state_ = STATE_RUNNING;

        // Fork process.
        pid_ = fork();
        if (pid_ == -1)
            return false;

        // Check if child process.
        if (pid_ == 0)
        {
            // Redirect STDIN, STDOUT and STDERR.
            if (dup2(pipe_stdin_[FD_READ],STDIN_FILENO) == -1 ||
                dup2(pipe_stdout_[FD_WRITE],STDOUT_FILENO) == -1 ||
                dup2(pipe_stderr_[FD_WRITE],STDERR_FILENO) == -1)
            {
                close();
                exit(-1);
            }

            execv(path,arg_list);

            // A successful execv replaces this exit call.
            close();
            exit(-1);
        }

        // Register child.
        ProcessMonitor::instance().register_process(pid_,this);

        // Create listen thread.
        pthread_t thread;
        if (pthread_create(&thread,NULL,listen,this) != 0)
        {
            // If we failed, kill the process.
            kill(pid_,SIGKILL);
            return false;
        }

        // Give the client application a chance to start.
        usleep(200 * 1000);

        return true;
    }

    /**
     * Checks if a process is running.
     * @return If a process is running true is returned, if not false is
     *         returned.
     */
    bool Process::running() const
    {
        bool locked = pthread_mutex_lock(const_cast<pthread_mutex_t *>(&mutex_)) == 0;
        bool running = state_ == STATE_RUNNING;
        if (locked)
            pthread_mutex_unlock(const_cast<pthread_mutex_t *>(&mutex_));

        return running;
    }

    /**
     * Wait until the running process completes.
     * @return If successful and a process is running true is returned,
     *         otherwise false is returned.
     */
    bool Process::wait() const
    {
        bool locked = pthread_mutex_lock(const_cast<pthread_mutex_t *>(&mutex_)) == 0;
        pid_t pid = pid_;
        if (locked)
            pthread_mutex_unlock(const_cast<pthread_mutex_t *>(&mutex_));

        if (pid == -1 || !running())
            return false;

        int status = 0;
        if (waitpid(pid,&status,0) == -1)
            return false;

        // Make sure that the state is valid.
        while (state_ != STATE_STOPPED)
            usleep(100 * 1000);

        return true;
    }

    /**
     * Writes raw data to the process standard input.
     * @param [in] buffer Pointer to the beginning of the bufferi
     *                    containing the data to be written.
     * @param [in] count The number of bytes to write.
     * @return If the operation failed -1 is returned, otherwise the
     *         function returns the number of bytes written (this may be
     *         zero).
     */
    tint64 Process::write(void *buffer,tuint32 count)
    {
        if (pid_ == -1 || !running())
            return -1;

        return ::write(pipe_stdin_[FD_WRITE],buffer,count);
    }
};

