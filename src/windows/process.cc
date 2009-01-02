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
     * Constructs a Process object.
     */
    Process::Process() : invalid_inheritor_(false),
        pipe_stdin_(NULL),pipe_output_(NULL),
        process_handle_(NULL),stop_event_(NULL),thread_handle_(NULL),
        thread_id_(0),state_(STATE_STOPPED),
        mutex_(NULL),mutex_exec_(NULL)
    {
        mutex_ = CreateMutex(NULL,FALSE,NULL);
        mutex_exec_ = CreateMutex(NULL,FALSE,NULL);
    }

    /**
     * Destructs the Process object.
     */
    Process::~Process()
    {
        // Make sure that the execution is completed before destroying this object.
        bool locked = WaitForSingleObject(mutex_exec_,INFINITE) == WAIT_OBJECT_0;
        if (locked)
            ReleaseMutex(mutex_exec_);

        close();

        // Closes mutexes.
        if (mutex_exec_ != NULL)
        {
            CloseHandle(mutex_exec_);
            mutex_exec_ = NULL;
        }

        if (mutex_ != NULL)
        {
            CloseHandle(mutex_);
            mutex_ = NULL;
        }
    }

    /**
     * Closes all internal pipes and resets the internal state of the object.
     */
    void Process::close()
    {
        bool locked = WaitForSingleObject(mutex_,INFINITE) == WAIT_OBJECT_0;

        // Close handles.
        if (thread_handle_ != NULL)
        {
            if (GetCurrentThreadId() != thread_id_)
            {
                // If the thread does not respond within 5 seconds it will be
                // forced to terminate.
                SetEvent(stop_event_);

                if (WaitForSingleObject(thread_handle_,5000) == WAIT_TIMEOUT)
                    TerminateThread(thread_handle_,-2);
            }

            CloseHandle(thread_handle_);
            thread_handle_ = NULL;
        }

        if (stop_event_ != NULL)
        {
            CloseHandle(stop_event_);
            stop_event_ = NULL;
        }

        if (process_handle_ != NULL)
        {
            CloseHandle(process_handle_);
            process_handle_ = NULL;
        }

        if (pipe_stdin_ != NULL)
        {
            CloseHandle(pipe_stdin_);
            pipe_stdin_ = NULL;
        }

        if (pipe_output_ != NULL)
        {
            CloseHandle(pipe_output_);
            pipe_output_ = NULL;
        }

        /*if (g_hCloseHandle != NULL)
        {
            ::CloseHandle(g_hCloseHandle);
            g_hCloseHandle = NULL;
        }*/

        // Reset state.
        thread_id_ = -1;
        state_ = STATE_STOPPED;

        if (locked)
            ReleaseMutex(mutex_);
    }

    /**
     * Reads from the specified file descriptor to the standard output buffer.
     * @return If successful true is returned, if unsuccessful false is
     *         returned.
     */
    bool Process::read_output(int fd)
    {
        char buffer[READ_BUFFER_SIZE];

        while (true)
        {
            unsigned long bytes_avail = 0;
            if (!PeekNamedPipe(pipe_output_,NULL,0,NULL,&bytes_avail,NULL))
                break;

            if (bytes_avail == 0)
                return true;

            unsigned long read = 0;
            if (!ReadFile(pipe_output_,buffer,min(bytes_avail,READ_BUFFER_SIZE - 1),&read,NULL) || read == 0)
                break;

            buffer[read] = '\0';

            // Split the buffer into lines.
            for (unsigned long i = 0; i < read; i++)
            {
                if (buffer[i] == '\n' || buffer[i] == '\r')
                {
                    // Avoid flushing an empty buffer.
                    if (line_buffer_.size() >  0)
                    {
                        if (!invalid_inheritor_)
                            event_output(line_buffer_);

                        line_buffer_.resize(0);
                    }
                }
                else
                {
                    line_buffer_.push_back(buffer[i]);
                }
            }
        }

        /*unsigned long last_err = GetLastError();
        if (last_err == ERROR_BROKEN_PIPE || last_err == ERROR_NO_DATA)
            return false;*/

        return false;
    }

    /**
     * The thread entry for listening on the process output pipes.
     * @param [in] param A pointer to the Process object being executed.
     */
    unsigned long WINAPI Process::listen(void *param)
    {
        Process *process = static_cast<Process *>(param);

        // Prevent the object from being destroyed while running the separate thread.
        WaitForSingleObject(process->mutex_exec_,INFINITE);

        HANDLE handles[2];
        handles[0] = progress->process_handle_;
        handles[1] = progress->stop_event_;

        line_buffer_.resize(0);

        while (true)
        {
            if (!progress->read_output())
                break;

            unsigned long wait_res = WaitForMultipleObjects(2,handles,false,100);
            
            if (wait_res == WAIT_OBJECT_0)
            {
                progress->read_output();
                break;
            }
            else if (wait_res == WAIT_OBJECT_0 + 1)       // Check if the stop event has been signaled.
            {
                break;
            }
        }

        // Notify that the process has finished.
        if (!process->invalid_inheritor_)
            process->event_finished();

        process->close();

        ReleaseMutex(process->mutex_exec_);
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
        sleep(200);

        return true;
    }

    /**
     * Checks if a process is running.
     * @return If a process is running true is returned, if not false is
     *         returned.
     */
    bool Process::running() const
    {
        bool locked = WaitForSingleObject(mutex_,INFINITE) == WAIT_OBJECT_0;
        bool running = state_ == STATE_RUNNING;
        if (locked)
            ReleaseMutex(mutex_);

        return running;
    }

    /**
     * Wait until the running process completes.
     * @return If successful and a process is running true is returned,
     *         otherwise false is returned.
     */
    bool Process::wait() const
    {
        bool locked = WaitForSingleObject(mutex_,INFINITE) == WAIT_OBJECT_0;
        pid_t pid = pid_;
        if (locked)
            ReleaseMutex(mutex_);

        if (pid == -1 || !running())
            return false;

        int status = 0;
        if (waitpid(pid,&status,0) == -1)
            return false;

        // Make sure that the state is valid.
        while (state_ != STATE_STOPPED)
            sleep(2);

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
        // Wait if the process has not been started.
        while (!running())
            Sleep(100);

        unsigned long written = 0;
        WriteFile(pipe_stdin_,buffer,count,&written,NULL);

        return written;
    }
};

